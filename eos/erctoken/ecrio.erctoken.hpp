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

#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/singleton.hpp>

#include <string>

using namespace eosio;

namespace eosio{
    using std::string;

    CONTRACT erctoken : public contract {
        public:
            erctoken( name self, name first_receiver, datastream<const char*> ds )
            : contract( self, first_receiver, ds ), _stat( self, self.value ) {
                _stat_state = _stat.exists() ? _stat.get() : currency_stat{};
            }

            ~erctoken() {
                _stat.set( _stat_state, get_self() );
            }

            ACTION issue( name to, asset quantity, string memo );

            ACTION burn( name account, asset quantity, string memo );

            ACTION burnfrom( name burner, name owner, asset quantity, string memo );

            ACTION transfer( name from, name to, asset quantity, string memo );
        
            ACTION approve( name owner, name spender, asset quantity );

            ACTION transferfrom( name spender, name from, name to, asset quantity, string  memo );

            ACTION incallowance( name owner, name spender, asset quantity );

            ACTION decallowance( name owner, name spender, asset quantity );

            ACTION open( name owner, const symbol& symbol, name ram_payer );

            ACTION close( name owner, const symbol& symbol );

            static asset get_supply( name token_contract_account ){
                stat_singleton _stat( token_contract_account, token_contract_account.value );
                const auto& st = _stat.get();
                return st.supply;
            }

            static asset get_balance( name token_contract_account, name owner, symbol_code sym_code ){
                accounts accountstable( token_contract_account, owner.value );
                const auto& ac = accountstable.get( sym_code.raw() );
                return ac.balance;
            }

            using issue_action = eosio::action_wrapper<"issue"_n, &erctoken::issue>;
            using burn_action = eosio::action_wrapper<"burn"_n, &erctoken::burn>;
            using burnfrom_action = eosio::action_wrapper<"burnfrom"_n, &erctoken::burnfrom>;
            using transfer_action = eosio::action_wrapper<"transfer"_n, &erctoken::transfer>;
            using approve_action = eosio::action_wrapper<"approve"_n, &erctoken::approve>;
            using transferfrom_action = eosio::action_wrapper<"transferfrom"_n, &erctoken::transferfrom>;
            using incallowance_action = eosio::action_wrapper<"incallowance"_n, &erctoken::incallowance>;
            using decallowance_action = eosio::action_wrapper<"decallowance"_n, &erctoken::decallowance>;
            using open_action = eosio::action_wrapper<"open"_n, &erctoken::open>;
            using close_action = eosio::action_wrapper<"close"_n, &erctoken::close>;

        private:
            TABLE account {
                asset    balance;
                uint64_t primary_key()const { return balance.symbol.code().raw(); }
            };

            struct [[eosio::table("stat")]] currency_stat {
                asset    supply = asset{0, symbol(symbol_code("TEST"), 5)};
            };

            TABLE allowance {
                name     spender;
                asset    balance;

                uint64_t primary_key()const { return spender.value; }
            };

            typedef eosio::multi_index< "accounts"_n, account > accounts;
            typedef eosio::multi_index< "allowances"_n, allowance > allowances;
            typedef eosio::singleton< "stat"_n, currency_stat > stat_singleton;

            stat_singleton _stat;

            currency_stat  _stat_state;

            void sub_balance( name owner, asset value );
            void add_balance( name owner, asset value, name ram_payer );
    };

} // namespace eosio
