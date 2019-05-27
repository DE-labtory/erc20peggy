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

#include "erc721.hpp"
using namespace eosio;

ACTION erc721::create( name issuer, string sym ) {
    require_auth( _self );

	// Check if issuer account exists
	check( is_account( issuer ), "issuer account does not exist");

    // Valid symbol
    asset supply(0, symbol( symbol_code( sym.c_str() ), 0) );


    auto symbol = supply.symbol;
    check( symbol.is_valid(), "invalid symbol name" );

    // Check if currency with symbol already exists
    stats statstable( _self, symbol.code().raw() );
    auto existing = statstable.find( symbol.code().raw() );
    check( existing == statstable.end(), "symbol already exists" );

    // Create new currency
    statstable.emplace( _self, [&]( auto& s ) {
        s.supply = supply;
        s.issuer = issuer;
    });
}

ACTION erc721::issue( name to, asset quantity, vector<std::pair<uint64_t, std::pair<name, asset>>> tokenInfos, string memo ) {
    check( is_account(to), "to account does not exist" );

    auto sym = quantity.symbol;
    check( sym.is_valid(), "invalid symbol name" );
    check( sym.precision() == 0, "quantity must be a whole number" );
    check( memo.size() <= 256, "memo has more than 256 bytes" );

    stats statstable( _self, sym.code().raw() );
    auto existing_st = statstable.find( sym.code().raw() );
    check( existing_st != statstable.end(), "token with symbol does not exist, create token before issue" );
    const auto& st = *existing_st;

    require_auth( st.issuer );
    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must issue positive quantity of NFT" );
    check( sym == st.supply.symbol, "symbol precision mismatch" );
    check( quantity.amount == tokenInfos.size(), "mismatch between issue amount and token info" );

    statstable.modify( st, same_payer, [&]( auto& s ) {
        s.supply += quantity;
    });

    tokens tokenstable( _self, sym.code().raw() );

    auto payer = has_auth( to ) ? to : st.issuer;

    for( auto const& tk : tokenInfos ) {
        auto existing_tk = tokenstable.find( tk.first );
        check( existing_tk == tokenstable.end(), "token with symbol already exists" );
        tokenstable.emplace( payer, [&]( auto& token ) {
            token.tokenId   = tk.first;
            token.sym       = sym;
            token.owner     = to;
            token.value     = tk.second.second;
            token.tokenName = tk.second.first;
        });
    }    
    add_balance( to, quantity, payer );
}

ACTION erc721::burn( name owner, asset quantity, vector<uint64_t> tokenIds, string memo ) {
    require_auth( owner );

    auto sym = quantity.symbol;
    check( sym.is_valid(), "invalid symbol name" );
    check( sym.precision() == 0, "quantity must be a whole number" );

    check( memo.size() <= 256, "memo has more than 256 bytes" );

    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must burn positive quantity" );
    check( quantity.amount == tokenIds.size(), "mismatch between burn amount and token info" );

    stats statstable( _self, sym.code().raw() );
    auto existing_st = statstable.find( sym.code().raw() );
    check( existing_st != statstable.end(), "symbol does not exist at stats" );
    const auto& st = *existing_st;

    tokens tokenstable( _self, sym.code().raw() );
    for( auto const& tokenId : tokenIds ) {
        auto it = tokenstable.find( tokenId );
        check( it != tokenstable.end(), "token with symbol does not exists" );
        check( it->owner == owner, "not the owner of token" );
        tokenstable.erase( it );
    }

    sub_balance( owner, quantity );
    statstable.modify( st, same_payer, [&]( auto& s ) {
        s.supply -= quantity;
    });
}

ACTION erc721::burnfrom( name burner, string sym, id_type tokenId, string memo ) {
    require_auth( burner );

    asset as(0, symbol( symbol_code( sym.c_str() ), 0) );
    auto symbol = as.symbol;
    check( symbol.is_valid(), "invalid symbol name" );
    check( memo.size() <= 256, "memo has more than 256 bytes" );

    stats statstable( _self, symbol.code().raw() );
    auto existing_st = statstable.find( symbol.code().raw() );
    check( existing_st != statstable.end(), "symbol does not exist at stats" );
    const auto& st = *existing_st;

    tokens tokenstable( _self, symbol.code().raw() );
    auto it = tokenstable.find( tokenId );
    check( it != tokenstable.end(), "token with symbol does not exists" );

    require_recipient( it->owner );

    check( burner == it->spender, "burner is not token spender" );

    asset unit( 1, symbol );

    sub_balance( it->owner, unit );

    tokenstable.erase( it );
    statstable.modify( st, same_payer, [&]( auto& s ) {
        s.supply -= unit;
    });
}

ACTION erc721::transfer( name from, name to, string sym, id_type tokenId, string memo ) {
    check( from != to, "cannot transfer to self" );
    require_auth( from );
    check( is_account( to ), "to account does not exist" );

    require_recipient( from );
    require_recipient( to );

    asset as(0, symbol( symbol_code( sym.c_str() ), 0) );
    auto symbol = as.symbol;

    check( symbol.is_valid(), "invalid symbol name" );
    check( memo.size() <= 256, "memo has more than 256 bytes" );

    tokens tokenstable( _self, symbol.code().raw() );
    auto it = tokenstable.find( tokenId );
    check( it != tokenstable.end(), "token with symbol does not exists" );
    check( from == it->owner, "not the owner of token" );

    auto payer = has_auth( to ) ? to : from;
    
    tokenstable.modify( it, payer, [&]( auto& token ) {
        token.owner     = to;
        token.spender   = to;
    });

    asset unit( 1, symbol );

    sub_balance( from, unit );
    add_balance( to, unit, payer );
}

ACTION erc721::approve( name owner, name spender, string sym, id_type tokenId ) {
    require_auth( owner );

    asset as(0, symbol( symbol_code( sym.c_str() ), 0) );
    auto symbol = as.symbol;

    check( symbol.is_valid(), "invalid symbol name" );

    tokens tokenstable( _self, symbol.code().raw() );
    auto it = tokenstable.find( tokenId );
    check( it != tokenstable.end(), "token with symbol does not exists" );
    check( owner == it->owner, "not the owner of token" );

    tokenstable.modify( it, same_payer, [&]( auto& token ) {
        token.spender = spender;
    });
}

ACTION erc721::transferfrom( name spender, name to, string sym, id_type tokenId, string memo ) {
    require_auth( spender );

    check( is_account( to ), "to account does not exist");

    asset as(0, symbol( symbol_code( sym.c_str() ), 0) );
    auto symbol = as.symbol;

    check( symbol.is_valid(), "invalid symbol name" );
    check( memo.size() <= 256, "memo has more than 256 bytes" );

    tokens tokenstable( _self, symbol.code().raw() );
    auto it = tokenstable.find( tokenId );
    check( it != tokenstable.end(), "token with symbol does not exists" );
    check( spender == it->spender, "spender is not token spender" );
    name owner = it->owner;

    require_recipient( owner );
    require_recipient( to );
    
    auto payer = has_auth( to ) ? to : spender;

    tokenstable.modify( it, payer, [&]( auto& token ) {
        token.owner     = to;
        token.spender   = to;
    });

    asset unit( 1, symbol );

    sub_balance( owner, unit );
    add_balance( to, unit, payer );
}

void erc721::sub_balance( name owner, asset value ) {
    accounts from_acnts( get_self(), owner.value );

    const auto& from = from_acnts.get( value.symbol.code().raw(), "no balance object found" );
    check( from.balance.amount >= value.amount, "overdrawn balance" );

    name payer = !has_auth(owner) ? same_payer : owner;

    from_acnts.modify( from, payer, [&]( auto& a ) {
        a.balance -= value;
    });
}

void erc721::add_balance( name owner, asset value, name ram_payer ) {
    accounts to_acnts( get_self(), owner.value );
    auto to = to_acnts.find( value.symbol.code().raw() );
    if( to == to_acnts.end() ) {
        to_acnts.emplace( ram_payer, [&]( auto& a ){
            a.balance = value;
        });
    } else {
        to_acnts.modify( to, same_payer, [&]( auto& a ) {
            a.balance += value;
        });
    }
}