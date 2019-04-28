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
	"encoding/json"

	sdk "github.com/cosmos/cosmos-sdk/types"
)

// MsgLock defines a Lock message.
type MsgLock struct {
	Owner   sdk.AccAddress
	Balance sdk.Coins
}

// NewMsgLock() is a constructor for MsgLock.
func NewMsgLock(addr sdk.AccAddress, balance sdk.Coins) MsgLock {
	return MsgLock{
		Owner:   addr,
		Balance: balance,
	}
}

// Route() should return the name of the module.
func (m MsgLock) Route() string {
	return "erc20service"
}

// Type() should return the action
func (m MsgLock) Type() string {
	return "lock"
}

// ValidateBasic() runs stateless checks on the message.
func (m MsgLock) ValidateBasic() sdk.Error {
	if m.Owner.Empty() {
		return sdk.ErrInvalidAddress(m.Owner.String())
	}

	if !m.Balance.IsAllPositive() {
		return sdk.ErrInsufficientCoins("Balances must be positive")
	}

	return nil
}

// GetSignBytes() encodes the message for signing.
func (m MsgLock) GetSignBytes() []byte {
	b, err := json.Marshal(m)
	if err != nil {
		panic(err)
	}

	return sdk.MustSortJSON(b)
}

// GetSigners() defines whose signature is required.
func (m MsgLock) GetSigners() []sdk.AccAddress {
	return []sdk.AccAddress{m.Owner}
}

type MsgUnlock struct {
	Owner   sdk.AccAddress
	Balance sdk.Coins
}

func NewMsgUnlock(addr sdk.AccAddress, balance sdk.Coins) MsgUnlock {
	return MsgUnlock{
		Owner:   addr,
		Balance: balance,
	}
}

func (m MsgUnlock) Route() string {
	return "erc20service"
}

func (m MsgUnlock) Type() string {
	return "unlock"
}

func (m MsgUnlock) ValidateBasic() sdk.Error {
	if m.Owner.Empty() {
		return sdk.ErrInvalidAddress(m.Owner.String())
	}

	if !m.Balance.IsAllPositive() {
		return sdk.ErrInsufficientCoins("Balances must be positive")
	}

	return nil
}

func (m MsgUnlock) GetSignBytes() []byte {
	b, err := json.Marshal(m)
	if err != nil {
		panic(err)
	}

	return sdk.MustSortJSON(b)
}

func (m MsgUnlock) GetSigners() []sdk.AccAddress {
	return []sdk.AccAddress{m.Owner}
}

type MsgMint struct {
	Owner   sdk.AccAddress
	Balance sdk.Coins
}

func NewMsgMint(addr sdk.AccAddress, balance sdk.Coins) MsgMint {
	return MsgMint{
		Owner:   addr,
		Balance: balance,
	}
}

func (m MsgMint) Route() string {
	return "erc20service"
}

func (m MsgMint) Type() string {
	return "mint"
}

func (m MsgMint) ValidateBasic() sdk.Error {
	if m.Owner.Empty() {
		return sdk.ErrInvalidAddress(m.Owner.String())
	}

	if !m.Balance.IsAllPositive() {
		return sdk.ErrInsufficientCoins("Balances must be positive")
	}

	return nil
}

func (m MsgMint) GetSignBytes() []byte {
	b, err := json.Marshal(m)
	if err != nil {
		panic(err)
	}

	return sdk.MustSortJSON(b)
}

func (m MsgMint) GetSigners() []sdk.AccAddress {
	return []sdk.AccAddress{m.Owner}
}

type MsgBurn struct {
	Owner   sdk.AccAddress
	Balance sdk.Coins
}

func NewMsgBurn(addr sdk.AccAddress, balance sdk.Coins) MsgBurn {
	return MsgBurn{
		Owner:   addr,
		Balance: balance,
	}
}

func (m MsgBurn) Route() string {
	return "erc20service"
}

func (m MsgBurn) Type() string {
	return "burn"
}

func (m MsgBurn) ValidateBasic() sdk.Error {
	if m.Owner.Empty() {
		return sdk.ErrInvalidAddress(m.Owner.String())
	}

	if !m.Balance.IsAllPositive() {
		return sdk.ErrInsufficientCoins("Balances must be positive")
	}

	return nil
}

func (m MsgBurn) GetSignBytes() []byte {
	b, err := json.Marshal(m)
	if err != nil {
		panic(err)
	}

	return sdk.MustSortJSON(b)
}

func (m MsgBurn) GetSigners() []sdk.AccAddress {
	return []sdk.AccAddress{m.Owner}
}
