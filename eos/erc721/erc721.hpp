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

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <string>
#include <vector>

using namespace eosio;
using std::string;
using std::vector;
typedef uint64_t id_type;

CONTRACT erc721 : public contract {
    public:
        erc721( name self, name first_receiver, datastream<const char*> ds )
		: contract( self, first_receiver, ds ) {}

        ACTION create( name issuer, string sym );

        ACTION issue( name to, asset quantity, vector<std::pair<uint64_t, std::pair<name, asset>>> tokenInfos, string memo );

        ACTION burn( name owner, asset quantity, vector<uint64_t> tokenIds, string memo );

        ACTION burnfrom( name burner, string sym, id_type tokenId, string memo );

        ACTION transfer( name from, name to, string sym, id_type tokenId, string memo );

        ACTION approve( name owner, name spender, string sym, id_type tokenId );

        ACTION transferfrom( name spender, name to, string sym, id_type tokenId, string memo );

        static asset get_supply( name token_contract_account, symbol_code sym_code ) {
            stats statstable( token_contract_account, sym_code.raw() );
            const auto& st = statstable.get( sym_code.raw() );
            return st.supply;
        }

        static asset get_balance( name token_contract_account, name owner, symbol_code sym_code ) {
            accounts accountstable( token_contract_account, owner.value );
            const auto& ac = accountstable.get( sym_code.raw() );
            return ac.balance;
        }

    private:
        TABLE account {
            asset balance;

            uint64_t primary_key() const { return balance.symbol.code().raw(); }
        };

        TABLE currency_stat {
            asset supply;
            name issuer;

            uint64_t primary_key() const { return supply.symbol.code().raw(); }
            uint64_t get_issuer() const { return issuer.value; }
        };

        TABLE token {
            id_type tokenId;     // Unique 64 bit identifier,
            symbol  sym;         // token symbol
            name    owner;  	 // token owner
            asset   value;       // token value
	        name    tokenName;	 // token name
            name    spender;     // token spender

            id_type     primary_key() const { return tokenId; }
            uint64_t    get_owner() const { return owner.value; }
        };

        typedef eosio::multi_index<"accounts"_n, account> accounts;

        typedef eosio::multi_index<"stat"_n, currency_stat,
                                    indexed_by< "byissuer"_n, const_mem_fun< currency_stat, uint64_t, &currency_stat::get_issuer> >
                                  > stats;

        typedef eosio::multi_index<"token"_n, token,
                                    indexed_by< "byowner"_n, const_mem_fun< token, uint64_t, &token::get_owner> >
                                  > tokens;

        void sub_balance(name owner, asset value);
        void add_balance(name owner, asset value, name ram_payer);
};