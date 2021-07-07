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

package rest

import (
	"fmt"
	"net/http"

	clientrest "github.com/cosmos/cosmos-sdk/client/rest"

	"github.com/DE-labtory/erc20peggy/erc20zone/x/erc20service"

	"github.com/cosmos/cosmos-sdk/types/rest"

	"github.com/cosmos/cosmos-sdk/client/context"
	"github.com/cosmos/cosmos-sdk/codec"
	sdk "github.com/cosmos/cosmos-sdk/types"

	"github.com/gorilla/mux"
)

const (
	restName = "erc20zone"
)

func RegisterRoutes(cliCtx context.CLIContext, r *mux.Router, cdc *codec.Codec, accStoreName string) {
	r.HandleFunc(fmt.Sprintf("/%s/tokens", restName), tokenHandler(cdc, cliCtx, accStoreName)).Methods("GET")
	r.HandleFunc(fmt.Sprintf("/%s/tokens", restName), mintHandler(cdc, cliCtx)).Methods("POST")
	r.HandleFunc(fmt.Sprintf("/%s/tokens", restName), burnHandler(cdc, cliCtx)).Methods("DELETE")
	r.HandleFunc(fmt.Sprintf("/%s/lock", restName), lockHandler(cdc, cliCtx)).Methods("POST")
	r.HandleFunc(fmt.Sprintf("/%s/lock", restName), unlockHandler(cdc, cliCtx)).Methods("DELETE")
}

func tokenHandler(cdc *codec.Codec, cliCtx context.CLIContext, accStoreName string) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		vars := mux.Vars(r)
		paramType := vars[restName]

		res, err := cliCtx.QueryWithData(fmt.Sprint("custom/%s/token/%s", accStoreName, paramType), nil)
		if err != nil {
			rest.WriteErrorResponse(w, http.StatusNotFound, err.Error())
			return
		}

		rest.PostProcessResponse(w, cdc, res, cliCtx.Indent)
	}
}

type mintReq struct {
	BaseReq rest.BaseReq `json:"base_req"`
	Address string       `json:"address"`
	Balance string       `json:"balance"`
}

func mintHandler(cdc *codec.Codec, cliCtx context.CLIContext) http.HandlerFunc {
	return func(w http.ResponseWriter, req *http.Request) {
		mReq := mintReq{}
		if !rest.ReadRESTReq(w, req, cdc, &mReq) {
			rest.WriteErrorResponse(w, http.StatusBadRequest, "failed to parse request")
			return
		}

		baseReq := mReq.BaseReq.Sanitize()
		if !baseReq.ValidateBasic(w) {
			return
		}

		addr, err := sdk.AccAddressFromBech32(mReq.Address)
		if err != nil {
			rest.WriteErrorResponse(w, http.StatusBadRequest, err.Error())
			return
		}

		balance, err := sdk.ParseCoins(mReq.Balance)
		if err != nil {
			rest.WriteErrorResponse(w, http.StatusBadRequest, err.Error())
			return
		}

		msg := erc20service.NewMsgMint(addr, balance)
		if err = msg.ValidateBasic(); err != nil {
			rest.WriteErrorResponse(w, http.StatusBadRequest, err.Error())
			return
		}

		clientrest.WriteGenerateStdTxResponse(w, cdc, cliCtx, baseReq, []sdk.Msg{msg})
	}
}

type burnReq struct {
	BaseReq rest.BaseReq `json:"base_req"`
	Address string       `json:"address"`
	Balance string       `json:"balance"`
}

func burnHandler(cdc *codec.Codec, cliCtx context.CLIContext) http.HandlerFunc {
	return func(w http.ResponseWriter, req *http.Request) {
		bReq := burnReq{}
		if !rest.ReadRESTReq(w, req, cdc, &bReq) {
			rest.WriteErrorResponse(w, http.StatusBadRequest, "failed to parse request")
			return
		}

		baseReq := bReq.BaseReq.Sanitize()
		if !baseReq.ValidateBasic(w) {
			return
		}

		addr, err := sdk.AccAddressFromBech32(bReq.Address)
		if err != nil {
			rest.WriteErrorResponse(w, http.StatusBadRequest, err.Error())
			return
		}

		balance, err := sdk.ParseCoins(bReq.Balance)
		if err != nil {
			rest.WriteErrorResponse(w, http.StatusBadRequest, err.Error())
			return
		}

		msg := erc20service.NewMsgBurn(addr, balance)
		if err = msg.ValidateBasic(); err != nil {
			rest.WriteErrorResponse(w, http.StatusBadRequest, err.Error())
			return
		}

		clientrest.WriteGenerateStdTxResponse(w, cdc, cliCtx, baseReq, []sdk.Msg{msg})
	}
}

type lockReq struct {
	BaseReq rest.BaseReq `json:"base_req"`
	Address string       `json:"address"`
	Balance string       `json:"balance"`
}

func lockHandler(cdc *codec.Codec, cliCtx context.CLIContext) http.HandlerFunc {
	return func(w http.ResponseWriter, req *http.Request) {
		lReq := lockReq{}
		if !rest.ReadRESTReq(w, req, cdc, &lReq) {
			rest.WriteErrorResponse(w, http.StatusBadRequest, "failed to parse request")
			return
		}

		baseReq := lReq.BaseReq.Sanitize()
		if !baseReq.ValidateBasic(w) {
			return
		}

		addr, err := sdk.AccAddressFromBech32(lReq.Address)
		if err != nil {
			rest.WriteErrorResponse(w, http.StatusBadRequest, err.Error())
			return
		}

		balance, err := sdk.ParseCoins(lReq.Balance)
		if err != nil {
			rest.WriteErrorResponse(w, http.StatusBadRequest, err.Error())
			return
		}

		msg := erc20service.NewMsgLock(addr, balance)
		if err = msg.ValidateBasic(); err != nil {
			rest.WriteErrorResponse(w, http.StatusBadRequest, err.Error())
			return
		}

		clientrest.WriteGenerateStdTxResponse(w, cdc, cliCtx, baseReq, []sdk.Msg{msg})
	}
}

type unlockReq struct {
	BaseReq rest.BaseReq `json:"base_req"`
	Address string       `json:"address"`
	Balance string       `json:"balance"`
}

func unlockHandler(cdc *codec.Codec, cliCtx context.CLIContext) http.HandlerFunc {
	return func(w http.ResponseWriter, req *http.Request) {
		uReq := unlockReq{}
		if !rest.ReadRESTReq(w, req, cdc, &uReq) {
			rest.WriteErrorResponse(w, http.StatusBadRequest, "failed to parse request")
			return
		}

		baseReq := uReq.BaseReq.Sanitize()
		if !baseReq.ValidateBasic(w) {
			return
		}

		addr, err := sdk.AccAddressFromBech32(uReq.Address)
		if err != nil {
			rest.WriteErrorResponse(w, http.StatusBadRequest, err.Error())
			return
		}

		balance, err := sdk.ParseCoins(uReq.Balance)
		if err != nil {
			rest.WriteErrorResponse(w, http.StatusBadRequest, err.Error())
			return
		}

		msg := erc20service.NewMsgUnlock(addr, balance)
		if err = msg.ValidateBasic(); err != nil {
			rest.WriteErrorResponse(w, http.StatusBadRequest, err.Error())
			return
		}

		clientrest.WriteGenerateStdTxResponse(w, cdc, cliCtx, baseReq, []sdk.Msg{msg})
	}
}
