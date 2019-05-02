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

package witness

import (
	"encoding/json"

	sdk "github.com/cosmos/cosmos-sdk/types"
)

type MsgLock struct {
	Data    []byte
	Address string
	Owner   sdk.AccAddress
}

func NewMsgLock(data []byte, address string, owner sdk.AccAddress) MsgLock {
	return MsgLock{
		Address: address,
		Data:    data,
		Owner:   owner,
	}
}

func (msg MsgLock) Route() string {
	return "witness"
}

func (msg MsgLock) Type() string {
	return "lock"
}

func (msg MsgLock) ValidateBasic() sdk.Error {
	if msg.Owner.Empty() {
		return sdk.ErrInvalidAddress(msg.Owner.String())
	}
	if len(msg.Address) == 0 || len(msg.Data) == 0 {
		return sdk.ErrUnknownRequest("Name and/or Value cannot be empty")
	}
	return nil
}

func (msg MsgLock) GetSignBytes() []byte {
	b, err := json.Marshal(msg)
	if err != nil {
		panic(err)
	}
	return sdk.MustSortJSON(b)
}

func (msg MsgLock) GetSigners() []sdk.AccAddress {
	return []sdk.AccAddress{msg.Owner}
}
