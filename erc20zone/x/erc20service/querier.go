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
	abci "github.com/tendermint/tendermint/abci/types"
)

const (
	QueryToken = "token"
)

func NewQuerier(keeper Keeper) sdk.Querier {
	return func(ctx sdk.Context, path []string, req abci.RequestQuery) (res []byte, err sdk.Error) {
		switch path[0] {
		case QueryToken:
			return queryToken(ctx, path, req, keeper)
		default:
			return nil, sdk.ErrUnknownRequest("unknown erc20service query endpoint")
		}
	}
}

// Query result payload for a token query
type QueryResToken struct {
	Token sdk.Coins `json:"token:"`
}

func (r QueryResToken) String() string {
	return r.Token.String()
}

func queryToken(ctx sdk.Context, path []string, req abci.RequestQuery, keeper Keeper) ([]byte, sdk.Error) {
	strAddr := path[0]
	addr, err := sdk.AccAddressFromHex(strAddr)
	if err != nil {
		return []byte{}, sdk.ErrInvalidAddress(err.Error())
	}

	token := keeper.Balance(ctx, addr)
	result, err := codec.MarshalJSONIndent(keeper.cdc, QueryResToken{token})
	if err != nil {
		return []byte{}, sdk.ErrUnknownRequest(err.Error())
	}

	return result, nil
}
