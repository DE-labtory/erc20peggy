/*
 * Copyright 2019 DE-labtory
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package erc20service

import (
	"github.com/cosmos/cosmos-sdk/codec"
	sdk "github.com/cosmos/cosmos-sdk/types"
)

type Keeper struct {
	accStoreKey  sdk.StoreKey
	lockStoreKey sdk.StoreKey
	cdc          *codec.Codec
}

func (k Keeper) Mint(ctx sdk.Context, addr sdk.AccAddress, balance sdk.Coins) {
	acc := k.account(ctx, addr)
	acc.Balance = acc.Balance.Add(balance)
	k.setAccount(ctx, acc)
}

func (k Keeper) Burn(ctx sdk.Context, addr sdk.AccAddress, balance sdk.Coins) {
	acc := k.account(ctx, addr)
	acc.Balance = acc.Balance.Sub(balance)
	k.setAccount(ctx, acc)
}

func (k Keeper) Balance(ctx sdk.Context, addr sdk.AccAddress) sdk.Coins {
	acc := k.account(ctx, addr)
	return acc.Balance
}

func (k Keeper) account(ctx sdk.Context, addr sdk.AccAddress) Account {
	accStore := ctx.KVStore(k.accStoreKey)
	if !accStore.Has(addr.Bytes()) {
		return NewAccount(addr)
	}

	binAcc := accStore.Get(addr.Bytes())
	acc := Account{}
	k.cdc.MustUnmarshalBinaryBare(binAcc, &acc)

	return acc
}

func (k Keeper) setAccount(ctx sdk.Context, acc Account) {
	if acc.Owner.Empty() {
		return
	}

	accStore := ctx.KVStore(k.accStoreKey)
	accStore.Set(acc.Owner.Bytes(), k.cdc.MustMarshalBinaryBare(acc))
}

func (k Keeper) Lock(ctx sdk.Context, addr sdk.AccAddress, balance sdk.Coins) {
	lockedTokens := k.lockedTokens(ctx, addr)
	lockedTokens = lockedTokens.Add(balance)
	k.setLockedTokens(ctx, addr, lockedTokens)

	k.Burn(ctx, addr, balance)
}

func (k Keeper) UnLock(ctx sdk.Context, addr sdk.AccAddress, balance sdk.Coins) {
	lockedTokens := k.lockedTokens(ctx, addr)
	lockedTokens = lockedTokens.Sub(balance)
	k.setLockedTokens(ctx, addr, lockedTokens)

	k.Mint(ctx, addr, balance)
}

func (k Keeper) lockedTokens(ctx sdk.Context, addr sdk.AccAddress) sdk.Coins {
	lockStore := ctx.KVStore(k.lockStoreKey)
	if !lockStore.Has(addr.Bytes()) {
		return InitBalance
	}

	binLocked := lockStore.Get(addr.Bytes())
	lockedTokens := sdk.Coins{}
	k.cdc.MustUnmarshalBinaryBare(binLocked, &lockedTokens)

	return lockedTokens
}

func (k Keeper) setLockedTokens(ctx sdk.Context, addr sdk.AccAddress, lockedTokens sdk.Coins) {
	if addr.Empty() {
		return
	}

	lockStore := ctx.KVStore(k.lockStoreKey)
	lockStore.Set(addr.Bytes(), k.cdc.MustMarshalBinaryBare(lockedTokens))
}

func NewKeeper(storeKey sdk.StoreKey, lockStoreKey sdk.StoreKey, cdc *codec.Codec) Keeper {
	return Keeper{
		accStoreKey:  storeKey,
		lockStoreKey: lockStoreKey,
		cdc:          cdc,
	}
}
