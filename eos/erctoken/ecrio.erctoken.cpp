#include "ecrio.erctoken.hpp"

namespace eosio {
ACTION erctoken::issue( name to, asset quantity, string memo ) {
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

ACTION erctoken::burn( name account, asset quantity, string memo ) {
    require_auth( account );

    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must burn positive quantity" );
    check( quantity.symbol == _stat_state.supply.symbol, "symbol precision mismatch" );
    check( memo.size() <= 256, "memo has more than 256 bytes" );

    sub_balance( account, quantity );
    _stat_state.supply -= quantity;
}

ACTION erctoken::burnfrom( name burner, name owner, asset quantity, string memo ) {
    require_auth( burner );
    check( burner != owner, "cannot burn self" );
    check( is_account( owner ), "owner account does not exist");

    require_recipient( burner );
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
    // sub_balance( owner, quantity );
    accounts from_acnts( get_self(), owner.value );
    const auto& from = from_acnts.get( quantity.symbol.code().raw(), "no balance object found" );
    check( from.balance.amount >= quantity.amount, "overdrawn balance" );

    from_acnts.modify( from, burner, [&]( auto& a ) {
        a.balance -= quantity;
    });

    _stat_state.supply -= quantity;
}

ACTION erctoken::transfer( name         from,
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

void erctoken::sub_balance( name owner, asset value ) {
    accounts from_acnts( get_self(), owner.value );

    const auto& from = from_acnts.get( value.symbol.code().raw(), "no balance object found" );
    check( from.balance.amount >= value.amount, "overdrawn balance" );

    from_acnts.modify( from, owner, [&]( auto& a ) {
        a.balance -= value;
    });
}

void erctoken::add_balance( name owner, asset value, name ram_payer ) {
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

ACTION erctoken::approve( name owner, name spender, asset quantity ) {
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

ACTION erctoken::transferfrom( name spender, name from, name to, asset quantity, string memo ) {
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

    accounts from_acnts( get_self(), from.value );
    const auto& facnt = from_acnts.get( quantity.symbol.code().raw(), "no balance object found" );
    check( facnt.balance.amount >= quantity.amount, "overdrawn balance" );

    from_acnts.modify( facnt, spender, [&]( auto& a ) {
        a.balance -= quantity;
    });    
    // sub_balance( from, quantity );
    add_balance( to, quantity, payer );

    allws.modify( it, same_payer, [&]( auto& a ){
        a.balance -= quantity;
    });
}

ACTION erctoken::incallowance( name owner, name spender, asset quantity ) {
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

ACTION erctoken::decallowance( name owner, name spender, asset quantity ) {
    require_auth( owner );

    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must issue positive quantity" );
    check( quantity.symbol == _stat_state.supply.symbol, "symbol precision mismatch" );

    allowances allws( get_self(), owner.value );
    auto it = allws.require_find( spender.value, "spender is not registed" );
    allws.modify( it, same_payer, [&]( auto& a ) {
        a.balance -= quantity;
    });
}

ACTION erctoken::open( name owner, const symbol& symbol, name ram_payer ) {
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

ACTION erctoken::close( name owner, const symbol& symbol ) {
    require_auth( owner );
    accounts acnts( get_self(), owner.value );
    auto it = acnts.find( symbol.code().raw() );
    check( it != acnts.end(), "Balance row already deleted or never existed. Action won't have any effect." );
    check( it->balance.amount == 0, "Cannot close because the balance is not zero." );
    acnts.erase( it );
}

} // namespace eosio

EOSIO_DISPATCH( eosio::erctoken, (issue)(burn)(burnfrom)(transfer)(approve)(transferfrom)(incallowance)(decallowance)(open)(close) )
