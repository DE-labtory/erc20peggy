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

package client

import (
	erc20servicecmd "github.com/DE-labtory/erc20peggy/erc20zone/x/erc20service/client/cli"
	"github.com/cosmos/cosmos-sdk/client"
	"github.com/spf13/cobra"
	amino "github.com/tendermint/go-amino"
)

type ModuleClient struct {
	accStoreKey  string
	lockStoreKey string
	cdc          *amino.Codec
}

func NewModuleClient(accStoreKey string, lockStoreKey string, cdc *amino.Codec) ModuleClient {
	return ModuleClient{
		accStoreKey:  accStoreKey,
		lockStoreKey: lockStoreKey,
		cdc:          cdc,
	}
}

func (c ModuleClient) GetQueryCmd() *cobra.Command {
	erc20svcQueryCmd := &cobra.Command{
		Use: "erc20service",
	}

	erc20svcQueryCmd.AddCommand(client.GetCommands(
		erc20servicecmd.GetCmdToken(c.accStoreKey, c.cdc),
	)...)

	return erc20svcQueryCmd
}

func (c ModuleClient) GetTxCmd() *cobra.Command {
	erc20svcTxCmd := &cobra.Command{
		Use: "erc20service",
	}

	erc20svcTxCmd.AddCommand(client.PostCommands(
		erc20servicecmd.GetCmdMint(c.cdc),
		erc20servicecmd.GetCmdBurn(c.cdc),
		erc20servicecmd.GetCmdLock(c.cdc),
		erc20servicecmd.GetCmdUnlock(c.cdc),
	)...)

	return erc20svcTxCmd
}
