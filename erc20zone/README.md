# ERC20 Zone

ERC20 Zone is a zone of the Cosmos for ERC20 token.

This zone has a service which manages erc20 tokens.

## Interface

ERC20 Zone has following interfaces.

### Mint

`Mint(ctx sdk.Context, addr sdk.AccAddress, balance sdk.Coins)`

Mint erc20 tokens to the address.

### Burn

`Burn(ctx sdk.Context, addr sdk.AccAddress, balance sdk.Coins)`

Burn erc20 tokens of the address.

### Lock

Lock erc20 tokens of the address. When you transfer erc20 tokens to ehter, this will be used.

`Lock(ctx sdk.Context, addr sdk.AccAddress, balance sdk.Coins)`

### Unlock

Unlock erc20 tokens of the address. When you transfer ether to erc20 tokens, this will be used.

`Unlock(ctx sdk.Context, addr sdk.AccAddress, balance sdk.Coins)`

## Struct

### ERC20 Account State

```go
type Account struct {
	Owner   sdk.AccAddress `json:"owner"`
	Balance sdk.Coins      `json:"balance"`
}
```

### Keeper

```go
type Keeper struct {
	accStoreKey  sdk.StoreKey
	lockStoreKey sdk.StoreKey
	cdc          *codec.Codec
}
```

### Message

```go
type MsgLock struct {
	Owner   sdk.AccAddress
	Balance sdk.Coins
}
```

```go
type MsgUnlock struct {
	Owner   sdk.AccAddress
	Balance sdk.Coins
}
```

```go
type MsgMint struct {
	Owner   sdk.AccAddress
	Balance sdk.Coins
}
```

```go
type MsgBurn struct {
	Owner   sdk.AccAddress
	Balance sdk.Coins
}
```