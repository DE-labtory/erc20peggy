package erc20service

import (
	"fmt"

	sdk "github.com/cosmos/cosmos-sdk/types"
)

// NewHandler() returns a handler for "erc20service" type messages.
func NewHandler(keeper Keeper) sdk.Handler {
	return func(ctx sdk.Context, msg sdk.Msg) sdk.Result {
		switch msg := msg.(type) {
		case MsgLock:
			return handleMsgLock(ctx, keeper, msg)
		case MsgUnlock:
			return handleMsgUnlock(ctx, keeper, msg)
		case MsgMint:
			return handleMsgMint(ctx, keeper, msg)
		case MsgBurn:
			return handleMsgBurn(ctx, keeper, msg)
		default:
			errMsg := fmt.Sprintf("Unrecognized erc20service Msg type: %v", msg.Type())
			return sdk.ErrUnknownRequest(errMsg).Result()
		}
	}
}

func handleMsgLock(ctx sdk.Context, keeper Keeper, msg MsgLock) sdk.Result {
	if !keeper.Balance(ctx, msg.Owner).IsAllGTE(msg.Balance) {
		return sdk.ErrInsufficientCoins("Balance not high enough").Result()
	}

	keeper.Lock(ctx, msg.Owner, msg.Balance)

	return sdk.Result{}
}

func handleMsgUnlock(ctx sdk.Context, keeper Keeper, msg MsgUnlock) sdk.Result {
	if !keeper.lockedTokens(ctx, msg.Owner).IsAllGTE(msg.Balance) {
		return sdk.ErrInsufficientCoins("Locked tokens not high enough").Result()
	}

	keeper.UnLock(ctx, msg.Owner, msg.Balance)

	return sdk.Result{}
}

func handleMsgMint(ctx sdk.Context, keeper Keeper, msg MsgMint) sdk.Result {
	keeper.Mint(ctx, msg.Owner, msg.Balance)

	return sdk.Result{}
}

func handleMsgBurn(ctx sdk.Context, keeper Keeper, msg MsgBurn) sdk.Result {
	if !keeper.Balance(ctx, msg.Owner).IsAllGTE(msg.Balance) {
		return sdk.ErrInsufficientCoins("Balance not high enough").Result()
	}

	keeper.Burn(ctx, msg.Owner, msg.Balance)

	return sdk.Result{}
}
