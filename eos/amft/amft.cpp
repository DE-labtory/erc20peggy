#include "amft.hpp"
using namespace eosio;

ACTION amft::issue( name to, asset quantity, string memo ) {
    require_auth( get_self() );

    auto sym = quantity.symbol;
    check( sym.is_valid(), "invalid symbol name" );
    check( memo.size() <= 256, "memo has more than 256 bytes" );

    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must issue positive quantity" );
    check( quantity.symbol == _stat_state.supply.symbol, "symbol precision mismatch" );

    _stat_state.supply += quantity;

    add_balance( get_self(), quantity, get_self() );

    if( to != get_self() ) {
        SEND_INLINE_ACTION( *this, transfer, { {get_self(), "active"_n} },
                            { get_self(), to, quantity, memo }
        );
    }
}

ACTION amft::burn( name owner, asset quantity, string memo ) {
    require_auth( owner );

    auto sym = quantity.symbol;
    check( sym.is_valid(), "invalid symbol name" );

    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must burn positive quantity" );
    check( quantity.symbol == _stat_state.supply.symbol, "symbol precision mismatch" );
    check( memo.size() <= 256, "memo has more than 256 bytes" );

    sub_balance( owner, quantity );
    _stat_state.supply -= quantity;

    allowances allws( get_self(), owner.value );
    auto existing_allw = allws.find( sym.code().raw() );
    if( existing_allw != allws.end() ) {
        accounts acnts( get_self(), owner.value );
        auto existing_ac = acnts.find( sym.code().raw() );
        if( !existing_ac->balance.amount ) allws.erase(existing_allw);
        else if( existing_allw->balance.amount > existing_ac->balance.amount ){
            allws.modify( existing_allw, same_payer, [&]( auto& a ) {
                a.balance.amount = existing_ac->balance.amount;
            });
        }
    }
}

ACTION amft::burnfrom( name burner, name owner, asset quantity, string memo ) {
    require_auth( burner );
    check( burner != owner, "cannot burnfrom self" );
    check( is_account( owner ), "owner account does not exist");

    require_recipient( owner );

    auto sym = quantity.symbol;
    check( sym.is_valid(), "invalid symbol name" );
    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must be positive quantity" );
    check( quantity.symbol == _stat_state.supply.symbol, "symbol precision mismatch" );
    check( memo.size() <= 256, "memo has more than 256 bytes" );

    allowances allws( get_self(), owner.value );
    auto existing_allw = allws.require_find( sym.code().raw(), "no symbol in the allowance table" );
    check( existing_allw->spender == burner, "you are not a spender" );
    check( existing_allw->balance.amount >= quantity.amount, "burner does not have enough allowed amount" );
    
    if( existing_allw->balance.amount == quantity.amount ) allws.erase( existing_allw );
    else {
        allws.modify( existing_allw, same_payer, [&]( auto& a ) {
            a.balance -= quantity;
        });
    }

    sub_balance( owner, quantity );

    _stat_state.supply -= quantity;
}

ACTION amft::transfer(  name         from,
                        name         to,
                        asset        quantity,
                        string       memo ) {
    check( from != to, "cannot transfer to self" );
    require_auth( from );
    check( is_account( to ), "to account does not exist");

    require_recipient( from );
    require_recipient( to );

    auto sym = quantity.symbol;
    check( sym.is_valid(), "invalid symbol name" );
    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must transfer positive quantity" );
    check( quantity.symbol == _stat_state.supply.symbol, "symbol precision mismatch" );
    check( memo.size() <= 256, "memo has more than 256 bytes" );

    auto payer = has_auth( to ) ? to : from;

    sub_balance( from, quantity );
    add_balance( to, quantity, payer );

    allowances allws( get_self(), from.value );
    auto existing_allw = allws.find( sym.code().raw() );
    if( existing_allw != allws.end() ) {
        accounts acnts( get_self(), from.value );
        auto existing_ac = acnts.find( sym.code().raw() );
        if( !existing_ac->balance.amount ) allws.erase(existing_allw);
        else if( existing_allw->balance.amount > existing_ac->balance.amount ){
            allws.modify( existing_allw, same_payer, [&]( auto& a ) {
                a.balance.amount = existing_ac->balance.amount;
            });
        }
    }
}

ACTION amft::approve( name owner, name spender, asset quantity ) {
    require_auth( owner );

    auto sym = quantity.symbol;
    check( sym.is_valid(), "invalid symbol name" );
    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must issue positive quantity" );
    check( quantity.symbol == _stat_state.supply.symbol, "symbol precision mismatch" );

    accounts acnts( get_self(), owner.value );
    auto existing_ac = acnts.require_find( sym.code().raw(), "owner does not have token with symbol" );

    check( existing_ac->balance.amount >= quantity.amount, "not enough balance" );
    
    allowances allws( get_self(), owner.value );
    auto existing_allw = allws.find( sym.code().raw() );
    if( existing_allw == allws.end() ) {
        allws.emplace( owner, [&]( auto& a ){
            a.balance = quantity;
            a.spender = spender;
        });
    } else {
        allws.modify( existing_allw, same_payer, [&]( auto& a ){
            a.balance = quantity;
            a.spender = spender;
        });
    }
}

ACTION amft::transferfrom( name spender, name from, name to, asset quantity, string memo ) {
    require_auth( spender );
    check( from != to, "cannot transfer to self" );
    check( is_account( from ), "from account does not exist");
    check( is_account( to ), "to account does not exist");

    require_recipient( from );
    require_recipient( to );

    auto sym = quantity.symbol;
    check( sym.is_valid(), "invalid symbol name" );
    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must transfer positive quantity" );
    check( quantity.symbol == _stat_state.supply.symbol, "symbol precision mismatch" );
    check( memo.size() <= 256, "memo has more than 256 bytes" );

    allowances allws( get_self(), from.value );
    auto existing_allw = allws.require_find( sym.code().raw(), "no symbol in the allowance table" );
    check( existing_allw->balance.amount >= quantity.amount, "spender does not have enough allowed amount" );

    if( existing_allw->balance.amount == quantity.amount ) allws.erase( existing_allw );
    else {
        allws.modify( existing_allw, same_payer, [&]( auto& a ) {
            a.balance -= quantity;
        });
    }

    auto payer = has_auth( to ) ? to : spender;

    sub_balance( from, quantity );
    add_balance( to, quantity, payer );
}

ACTION amft::incallowance( name owner, asset quantity ) {
    require_auth( owner );

    auto sym = quantity.symbol;
    
    accounts acnts( get_self(), owner.value );
    auto existing_ac = acnts.require_find( sym.code().raw(), "owner does not have token with symbol" );

    allowances allws( get_self(), owner.value );
    auto existing_allw = allws.require_find( sym.code().raw(), "spender is not registed" );

    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must issue positive quantity" );
    check( quantity.symbol == _stat_state.supply.symbol, "symbol precision mismatch" );
    check( existing_ac->balance.amount >= existing_allw->balance.amount + quantity.amount, "owner does not have enough increase allow amount" );

    allws.modify( existing_allw, same_payer, [&]( auto& a ) {
        a.balance += quantity;
    });
}

ACTION amft::decallowance( name owner, asset quantity ) {
    require_auth( owner );

    auto sym = quantity.symbol;

    allowances allws( get_self(), owner.value );
    auto existing_allw = allws.require_find( sym.code().raw(), "spender is not registed" );

    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must issue positive quantity" );
    check( quantity.symbol == _stat_state.supply.symbol, "symbol precision mismatch" );
    check( existing_allw->balance >= quantity, "there is not enough balance" );

    allws.modify( existing_allw, same_payer, [&]( auto& a ) {
        a.balance -= quantity;
    });
}

ACTION amft::open( name owner, const symbol& symbol, name ram_payer ) {
    require_auth( ram_payer );

    auto sym_code_raw = symbol.code().raw();

    check( _stat_state.supply.symbol == symbol, "symbol precision mismatch" );

    accounts acnts( get_self(), owner.value );
    auto existing_ac = acnts.find( sym_code_raw );
    if( existing_ac == acnts.end() ) {
        acnts.emplace( ram_payer, [&]( auto& a ){
            a.balance = asset{0, symbol};
        });
    }
}

ACTION amft::close( name owner, const symbol& symbol ) {
    require_auth( owner );
    accounts acnts( get_self(), owner.value );
    auto existing_ac = acnts.find( symbol.code().raw() );
    check( existing_ac != acnts.end(), "Balance row already deleted or never existed. Action won't have any effect." );
    check( existing_ac->balance.amount == 0, "Cannot close because the balance is not zero." );
    acnts.erase( existing_ac );
}

ACTION amft::createnft( name issuer, string sym ) {
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
        s.supply        = supply;
        s.issuer        = issuer;
    });
}

ACTION amft::issuenft( name to, asset quantity, vector<std::pair<uint64_t, std::pair<name, asset>>> token_infos, string memo ) {
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
    check( quantity.amount == token_infos.size(), "mismatch between issue amount and token info" );

    statstable.modify( st, same_payer, [&]( auto& s ) {
        s.supply += quantity;
    });

    tokens tokenstable( _self, sym.code().raw() );

    auto payer = has_auth( to ) ? to : st.issuer;

    for( auto const& tk : token_infos ) {
        auto existing_tk = tokenstable.find( tk.first );
        check( existing_tk == tokenstable.end(), "token with symbol already exists" );
        tokenstable.emplace( payer, [&]( auto& token ) {
            token.token_id   = tk.first;
            token.owner     = to;
            token.value     = tk.second.second;
            token.tokenName = tk.second.first;
        });
    }    
    add_balance( to, quantity, payer );
}

ACTION amft::burnnft( name owner, asset quantity, vector<uint64_t> token_ids, string memo ) {
    require_auth( owner );

    auto sym = quantity.symbol;
    check( sym.is_valid(), "invalid symbol name" );
    check( sym.precision() == 0, "quantity must be a whole number" );

    check( memo.size() <= 256, "memo has more than 256 bytes" );

    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must burn positive quantity" );
    check( quantity.amount == token_ids.size(), "mismatch between burn amount and token info" );

    stats statstable( _self, sym.code().raw() );
    auto existing_st = statstable.find( sym.code().raw() );
    check( existing_st != statstable.end(), "symbol does not exist at stats" );
    const auto& st = *existing_st;

    tokens tokenstable( _self, sym.code().raw() );
    for( auto const& token_id : token_ids ) {
        auto existing_tk = tokenstable.find( token_id );
        check( existing_tk != tokenstable.end(), "token with symbol does not exists" );
        check( existing_tk->owner == owner, "not the owner of token" );
        tokenstable.erase( existing_tk );
    }

    sub_balance( owner, quantity );
    statstable.modify( st, same_payer, [&]( auto& s ) {
        s.supply -= quantity;
    });
}

ACTION amft::burnnftfrom( name burner, string sym, id_type token_id, string memo ) {
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
    auto existing_tk = tokenstable.find( token_id );
    check( existing_tk != tokenstable.end(), "token with symbol does not exists" );

    require_recipient( existing_tk->owner );

    check( burner == existing_tk->spender, "burner is not token spender" );

    asset unit( 1, symbol );

    sub_balance( existing_tk->owner, unit );

    tokenstable.erase( existing_tk );
    statstable.modify( st, same_payer, [&]( auto& s ) {
        s.supply -= unit;
    });
}

ACTION amft::send( name from, name to, string sym, id_type token_id, string memo ) {
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
    auto existing_tk = tokenstable.find( token_id );
    check( existing_tk != tokenstable.end(), "token with symbol does not exists" );
    check( from == existing_tk->owner, "not the owner of token" );
    check( existing_tk->spender != _self, "if spender is _self, it can not transfer" );

    auto payer = has_auth( to ) ? to : from;
    
    tokenstable.modify( existing_tk, payer, [&]( auto& token ) {
        token.owner     = to;
        token.spender   = to;
    });

    asset unit( 1, symbol );

    sub_balance( from, unit );
    add_balance( to, unit, payer );
}

ACTION amft::approvenft( name owner, name spender, string sym, id_type token_id ) {
    require_auth( owner );

    asset as(0, symbol( symbol_code( sym.c_str() ), 0) );
    auto symbol = as.symbol;

    check( symbol.is_valid(), "invalid symbol name" );

    tokens tokenstable( _self, symbol.code().raw() );
    auto existing_tk = tokenstable.find( token_id );
    check( existing_tk != tokenstable.end(), "token with symbol does not exists" );
    check( owner == existing_tk->owner, "not the owner of token" );
    check( owner == _self || existing_tk->spender != _self, "if spender is _self, it can not be changed" );

    tokenstable.modify( existing_tk, same_payer, [&]( auto& token ) {
        token.spender = spender;
    });
}

ACTION amft::sendfrom( name spender, name to, string sym, id_type token_id, string memo ) {
    require_auth( spender );

    check( is_account( to ), "to account does not exist");

    asset as(0, symbol( symbol_code( sym.c_str() ), 0) );
    auto symbol = as.symbol;

    check( symbol.is_valid(), "invalid symbol name" );
    check( memo.size() <= 256, "memo has more than 256 bytes" );

    tokens tokenstable( _self, symbol.code().raw() );
    auto existing_tk = tokenstable.find( token_id );
    check( existing_tk != tokenstable.end(), "token with symbol does not exists" );
    check( spender == existing_tk->spender, "spender is not token spender" );
    name owner = existing_tk->owner;

    require_recipient( owner );
    require_recipient( to );
    
    auto payer = has_auth( to ) ? to : spender;

    tokenstable.modify( existing_tk, payer, [&]( auto& token ) {
        token.owner     = to;
        token.spender   = to;
    });

    asset unit( 1, symbol );

    sub_balance( owner, unit );
    add_balance( to, unit, payer );
}

void amft::sub_balance( name owner, asset value ) {
    accounts from_acnts( _self, owner.value );

    const auto& from = from_acnts.get( value.symbol.code().raw(), "no balance object found" );
    check( from.balance.amount >= value.amount, "overdrawn balance" );

    name payer = !has_auth(owner) ? same_payer : owner;

    from_acnts.modify( from, payer, [&]( auto& a ) {
        a.balance -= value;
    });
}

void amft::add_balance( name owner, asset value, name ram_payer ) {
    accounts to_acnts( _self, owner.value );
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

ACTION amft::auctiontoken( name auctioneer, string sym, id_type token_id, asset min_price, uint32_t sec ) {
    require_auth( auctioneer );

    require_recipient( auctioneer );
    require_recipient( _self );

    asset as(0, symbol( symbol_code( sym.c_str() ), 0) );
    auto symbol = as.symbol;
    check( symbol.is_valid(), "invalid symbol name" );

    check( sec > 0, "sec must be a positive integer" );

    const time_point_sec deadline = time_point_sec(now()) + sec;

    tokens tokenstable( _self, symbol.code().raw() );
    auto existing_tk = tokenstable.find( token_id );
    check( existing_tk != tokenstable.end(), "token with symbol does not exists" );
    check( existing_tk->owner == auctioneer, "not the owner of token" );

    token_bids tokenbidstable( _self, symbol.code().raw() );
    auto existing_bid = tokenbidstable.find( token_id );
    check( existing_bid == tokenbidstable.end(), "token bid already exist" );

    check( min_price.amount > 0, "token bid already exist" );

    tokenbidstable.emplace( auctioneer, [&]( auto& b ){
        b.token_id    = token_id;
        b.high_bidder = auctioneer;
        b.high_bid    = min_price.amount;
        b.deadline    = deadline;
    });
    
    tokenstable.modify( existing_tk, same_payer, [&]( auto& token ) {
        token.spender = _self;
    });
}

ACTION amft::bidtoken( name bidder, string sym, id_type token_id, asset bid ) {
    require_auth( bidder );

    check( bid.symbol == key_symbol, "bid asset must be key currency symbol" );

    asset as(0, symbol( symbol_code( sym.c_str() ), 0) );
    auto symbol = as.symbol;
    check( symbol.is_valid(), "invalid symbol name" );

    tokens tokenstable( _self, symbol.code().raw() );
    auto existing_tk = tokenstable.find( token_id );
    check( existing_tk != tokenstable.end(), "token with symbol does not exists" );

    check( bidder != existing_tk->owner, "token owners can not bid" );

    token_bids tokenbidstable( _self, symbol.code().raw() );
    auto existing_bid = tokenbidstable.find( token_id );
    check( existing_bid != tokenbidstable.end(), "token auction is not exist" );

    const time_point_sec time_now = time_point_sec(now());
    check( existing_bid->deadline > time_now, "the auction deadline has passed" );
    check( bid.amount > existing_bid->high_bid, "the bid amount is insufficient" );

    if( existing_bid->high_bidder != existing_tk->owner ) {
        asset refund_kc(existing_bid->high_bid, key_symbol);
        // refund
        action(
            permission_level{ _self, "active"_n },
            _self, "transfer"_n,
            std::make_tuple( _self, existing_bid->high_bidder, refund_kc, std::string("refund bidding fee"))
        ).send();
    }

    // new high bidder
    tokenbidstable.modify( existing_bid, same_payer, [&]( auto& b ){
        b.high_bidder = bidder;
        b.high_bid    = bid.amount; 
    });
    
    sub_balance( bidder, bid );
    add_balance( _self, bid, _self );
    
    bidresult_action bid_act( bidder, std::vector<eosio::permission_level>{ } );
    bid_act.send( bid );
}

ACTION amft::claimtoken( name requester, string sym, id_type token_id ) {
    require_auth(requester);

    asset as(0, symbol( symbol_code( sym.c_str() ), 0) );
    auto symbol = as.symbol;
    check( symbol.is_valid(), "invalid symbol name" );

    tokens tokenstable( _self, symbol.code().raw() );
    auto existing_tk = tokenstable.find( token_id );
    check( existing_tk != tokenstable.end(), "token with symbol does not exists" );

    token_bids tokenbidstable( _self, symbol.code().raw() );
    auto existing_bid = tokenbidstable.find( token_id );
    check( existing_bid != tokenbidstable.end(), "token auction is not exist" );

    const time_point_sec time_now = time_point_sec(now());
    check( existing_bid->deadline <= time_now, "deadline not over" );
    check( requester == existing_tk->owner || requester == existing_bid->high_bidder, "the requester is not authorized" );

    if( existing_bid->high_bidder != existing_tk->owner ) {
        asset payment_kc(existing_bid->high_bid, key_symbol);

        // bidding fee payment
        action(
            permission_level{ _self, "active"_n },
            _self, "transfer"_n,
            std::make_tuple( _self, existing_tk->owner, payment_kc, std::string("receive auction sale money"))
        ).send();

        // nft ownership change
        action(
            permission_level{ _self, "active"_n },
            _self, "sendfrom"_n,
            std::make_tuple( _self, existing_bid->high_bidder, sym, token_id, std::string("receive bid tokens"))
        ).send();
    } else {
        tokenstable.modify( existing_tk, same_payer, [&]( auto& token ) {
            token.spender = existing_tk->owner;
        });
    }

    tokenbidstable.erase( existing_bid );
}