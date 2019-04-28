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

package erc20zone

import (
	bam "github.com/cosmos/cosmos-sdk/baseapp"
	"github.com/cosmos/cosmos-sdk/codec"
	"github.com/cosmos/cosmos-sdk/x/auth"
	dbm "github.com/tendermint/tendermint/libs/db"
	"github.com/tendermint/tendermint/libs/log"
)

const (
	appName = "erc20service"
)

type erc20ServiceApp struct {
	*bam.BaseApp
	cdc *codec.Codec
}

func NewErc20ServiceApp(logger log.Logger, db dbm.DB) *erc20ServiceApp {
	// First, define the top level codec that will be shared by the different modules.
	cdc := MakeCodec()

	// BaseApp handles interactions with Tendermint through the ABCI protocol.
	baseApp := bam.NewBaseApp(appName, logger, db, auth.DefaultTxDecoder(cdc))

	app := &erc20ServiceApp{
		BaseApp: baseApp,
		cdc:     cdc,
	}

	return app
}
