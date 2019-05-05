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

package cli

import (
	"github.com/cosmos/cosmos-sdk/client/utils"
	"github.com/spf13/cobra"

	"github.com/DE-labtory/erc20peggy/erc20zone/x/erc20service"
	"github.com/cosmos/cosmos-sdk/client/context"
	"github.com/cosmos/cosmos-sdk/codec"

	sdk "github.com/cosmos/cosmos-sdk/types"
	authtxb "github.com/cosmos/cosmos-sdk/x/auth/client/txbuilder"
)

func GetCmdMint(cdc *codec.Codec) *cobra.Command {
	return &cobra.Command{
		Use:  "mint [address] [balance]",
		Args: cobra.ExactArgs(2),
		RunE: func(cmd *cobra.Command, args []string) error {
			cliCtx := context.NewCLIContext().WithCodec(cdc).WithAccountDecoder(cdc)
			if err := cliCtx.EnsureAccountExists(); err != nil {
				return err
			}

			addr, err := sdk.AccAddressFromBech32(args[0])
			if err != nil {
				return err
			}

			balance, err := sdk.ParseCoins(args[1])
			if err != nil {
				return err
			}

			msg := erc20service.NewMsgMint(addr, balance)
			if err := msg.ValidateBasic(); err != nil {
				return err
			}

			cliCtx.PrintResponse = true

			return utils.GenerateOrBroadcastMsgs(
				cliCtx,
				authtxb.NewTxBuilderFromCLI().WithTxEncoder(utils.GetTxEncoder(cdc)),
				[]sdk.Msg{msg},
				false,
			)
		},
	}
}

func GetCmdBurn(cdc *codec.Codec) *cobra.Command {
	return &cobra.Command{
		Use:  "burn [address] [balance]",
		Args: cobra.ExactArgs(2),
		RunE: func(cmd *cobra.Command, args []string) error {
			cliCtx := context.NewCLIContext().WithCodec(cdc).WithAccountDecoder(cdc)
			if err := cliCtx.EnsureAccountExists(); err != nil {
				return err
			}

			addr, err := sdk.AccAddressFromBech32(args[0])
			if err != nil {
				return err
			}

			balance, err := sdk.ParseCoins(args[1])
			if err != nil {
				return err
			}

			msg := erc20service.NewMsgBurn(addr, balance)
			if err := msg.ValidateBasic(); err != nil {
				return err
			}

			cliCtx.PrintResponse = true

			return utils.GenerateOrBroadcastMsgs(
				cliCtx,
				authtxb.NewTxBuilderFromCLI().WithTxEncoder(utils.GetTxEncoder(cdc)),
				[]sdk.Msg{msg},
				false,
			)
		},
	}
}

func GetCmdLock(cdc *codec.Codec) *cobra.Command {
	return &cobra.Command{
		Use:  "lock [address] [balance]",
		Args: cobra.ExactArgs(2),
		RunE: func(cmd *cobra.Command, args []string) error {
			cliCtx := context.NewCLIContext().WithCodec(cdc).WithAccountDecoder(cdc)
			if err := cliCtx.EnsureAccountExists(); err != nil {
				return err
			}

			balance, err := sdk.ParseCoins(args[1])
			if err != nil {
				return err
			}

			addr, err := sdk.AccAddressFromBech32(args[0])
			if err != nil {
				return err
			}

			msg := erc20service.NewMsgLock(addr, balance)
			if err := msg.ValidateBasic(); err != nil {
				return err
			}

			cliCtx.PrintResponse = true

			return utils.GenerateOrBroadcastMsgs(
				cliCtx,
				authtxb.NewTxBuilderFromCLI().WithTxEncoder(utils.GetTxEncoder(cdc)),
				[]sdk.Msg{msg},
				false,
			)
		},
	}
}

func GetCmdUnlock(cdc *codec.Codec) *cobra.Command {
	return &cobra.Command{
		Use:  "unlock [address] [balance]",
		Args: cobra.ExactArgs(2),
		RunE: func(cmd *cobra.Command, args []string) error {
			cliCtx := context.NewCLIContext().WithCodec(cdc).WithAccountDecoder(cdc)
			if err := cliCtx.EnsureAccountExists(); err != nil {
				return err
			}

			balance, err := sdk.ParseCoins(args[1])
			if err != nil {
				return err
			}

			addr, err := sdk.AccAddressFromBech32(args[0])
			if err != nil {
				return err
			}

			msg := erc20service.NewMsgUnlock(addr, balance)
			if err := msg.ValidateBasic(); err != nil {
				return err
			}

			cliCtx.PrintResponse = true

			return utils.GenerateOrBroadcastMsgs(
				cliCtx,
				authtxb.NewTxBuilderFromCLI().WithTxEncoder(utils.GetTxEncoder(cdc)),
				[]sdk.Msg{msg},
				false,
			)
		},
	}
}
