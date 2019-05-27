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

#include "erc20.hpp"

namespace eosio {
ACTION erc20::issue( name to, asset quantity, string memo ) {
    require_auth( get_self() );

    auto sym = quantity.symbol;
    check( sym.is_valid(), "invalid symbol name" );
    check( memo.size() <= 256, "memo has more than 256 bytes" );

    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must issue positive quantity" );

    check( quantity.symbol == _stat_state.supply.symbol, "symbol precision mismatch" );

    _stat_state.supply += quantity;

    add_balance( to, quantity, get_self() );

    if( to != get_self() ) {
        SEND_INLINE_ACTION( *this, transfer, { {get_self(), "active"_n} },
                            { get_self(), to, quantity, memo }
        );
    }
}

ACTION erc20::burn( name owner, asset quantity, string memo ) {
    require_auth( owner );

    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must burn positive quantity" );
    check( quantity.symbol == _stat_state.supply.symbol, "symbol precision mismatch" );
    check( memo.size() <= 256, "memo has more than 256 bytes" );

    sub_balance( owner, quantity );
    _stat_state.supply -= quantity;
}

ACTION erc20::burnfrom( name burner, name owner, asset quantity, string memo ) {
    require_auth( burner );
    check( burner != owner, "cannot burnfrom self" );
    check( is_account( owner ), "owner account does not exist");

    require_recipient( owner );

    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must be positive quantity" );
    check( quantity.symbol == _stat_state.supply.symbol, "symbol precision mismatch" );
    check( memo.size() <= 256, "memo has more than 256 bytes" );

    allowances allws( get_self(), owner.value );
    auto itr = allws.require_find( burner.value, "burner is not exist" );
    check( itr->balance.amount >= quantity.amount, "burner does not have enough allowed amount" );
    allws.modify( itr, same_payer, [&]( auto& a ) {
        a.balance -= quantity;
    });

    sub_balance( owner, quantity );

    _stat_state.supply -= quantity;
}

ACTION erc20::transfer( name         from,
                        name         to,
                        asset        quantity,
                        string       memo ) {
    check( from != to, "cannot transfer to self" );
    require_auth( from );
    check( is_account( to ), "to account does not exist");

    require_recipient( from );
    require_recipient( to );

    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must transfer positive quantity" );
    check( quantity.symbol == _stat_state.supply.symbol, "symbol precision mismatch" );
    check( memo.size() <= 256, "memo has more than 256 bytes" );

    auto payer = has_auth( to ) ? to : from;

    sub_balance( from, quantity );
    add_balance( to, quantity, payer );
}

void erc20::sub_balance( name owner, asset value ) {
    accounts from_acnts( get_self(), owner.value );

    const auto& from = from_acnts.get( value.symbol.code().raw(), "no balance object found" );
    check( from.balance.amount >= value.amount, "overdrawn balance" );

    name payer = !has_auth(owner) ? same_payer : owner;

    from_acnts.modify( from, payer, [&]( auto& a ) {
        a.balance -= value;
    });
}

void erc20::add_balance( name owner, asset value, name ram_payer ) {
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

ACTION erc20::approve( name owner, name spender, asset quantity ) {
    require_auth( owner );

    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must issue positive quantity" );
    check( quantity.symbol == _stat_state.supply.symbol, "symbol precision mismatch" );
    
    allowances allws( get_self(), owner.value );
    auto it = allws.find( spender.value );
    check( it == allws.end(), "spender already exist" );
    allws.emplace( owner, [&]( auto& a ){
        a.spender = spender;
        a.balance = quantity;
    });
}

ACTION erc20::transferfrom( name spender, name from, name to, asset quantity, string memo ) {
    require_auth( spender );
    check( from != to, "cannot transfer to self" );
    check( is_account( from ), "from account does not exist");
    check( is_account( to ), "to account does not exist");

    require_recipient( from );
    require_recipient( to );

    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must transfer positive quantity" );
    check( quantity.symbol == _stat_state.supply.symbol, "symbol precision mismatch" );
    check( memo.size() <= 256, "memo has more than 256 bytes" );

    allowances allws( get_self(), from.value );
    auto it = allws.require_find( spender.value, "spender is not exist" );
    check( it->balance.symbol == quantity.symbol, "symbol precision mismatch" );
    check( it->balance.amount >= quantity.amount, "spender does not have enough allowed amount" );

    auto payer = has_auth( to ) ? to : spender;

    sub_balance( from, quantity );
    add_balance( to, quantity, payer );

    allws.modify( it, same_payer, [&]( auto& a ){
        a.balance -= quantity;
    });
}

ACTION erc20::incallowance( name owner, name spender, asset quantity ) {
    require_auth( owner );

    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must issue positive quantity" );
    check( quantity.symbol == _stat_state.supply.symbol, "symbol precision mismatch" );

    allowances allws( get_self(), owner.value );
    auto it = allws.require_find( spender.value, "spender is not registed" );
    allws.modify( it, same_payer, [&]( auto& a ) {
        a.balance += quantity;
    });
}

ACTION erc20::decallowance( name owner, name spender, asset quantity ) {
    require_auth( owner );

    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must issue positive quantity" );
    check( quantity.symbol == _stat_state.supply.symbol, "symbol precision mismatch" );

    allowances allws( get_self(), owner.value );
    auto it = allws.require_find( spender.value, "spender is not registed" );
    check( it->balance >= quantity, "there is not enough balance" );
    
    allws.modify( it, same_payer, [&]( auto& a ) {
        a.balance -= quantity;
    });
}

ACTION erc20::open( name owner, const symbol& symbol, name ram_payer ) {
    require_auth( ram_payer );

    auto sym_code_raw = symbol.code().raw();

    check( _stat_state.supply.symbol == symbol, "symbol precision mismatch" );

    accounts acnts( get_self(), owner.value );
    auto it = acnts.find( sym_code_raw );
    if( it == acnts.end() ) {
        acnts.emplace( ram_payer, [&]( auto& a ){
            a.balance = asset{0, symbol};
        });
    }
}

ACTION erc20::close( name owner, const symbol& symbol ) {
    require_auth( owner );
    accounts acnts( get_self(), owner.value );
    auto it = acnts.find( symbol.code().raw() );
    check( it != acnts.end(), "Balance row already deleted or never existed. Action won't have any effect." );
    check( it->balance.amount == 0, "Cannot close because the balance is not zero." );
    acnts.erase( it );
}

} // namespace eosio

EOSIO_DISPATCH( eosio::erc20, (issue)(burn)(burnfrom)(transfer)(approve)(transferfrom)(incallowance)(decallowance)(open)(close) )
