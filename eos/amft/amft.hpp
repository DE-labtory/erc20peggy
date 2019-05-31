#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/singleton.hpp>
#include <string>
#include <vector>

using namespace eosio;
using std::string;
using std::vector;
typedef uint64_t id_type;

CONTRACT amft : public contract {
    public:
        static constexpr symbol key_symbol = symbol(symbol_code("KRW"), 3);
        amft( name self, name first_receiver, datastream<const char*> ds )
		: contract( self, first_receiver, ds ), _stat( self, self.value ) {
            _stat_state = _stat.exists() ? _stat.get() : key_currency_stat{};
        }
        ~amft() {
            _stat.set( _stat_state, get_self() );
        }

        ACTION issue( name to, asset quantity, string memo );

        ACTION burn( name owner, asset quantity, string memo );

        ACTION burnfrom( name burner, name owner, asset quantity, string memo );

        ACTION transfer( name from, name to, asset quantity, string memo );
    
        ACTION approve( name owner, name spender, asset quantity );

        ACTION transferfrom( name spender, name from, name to, asset quantity, string memo );

        ACTION incallowance( name owner, asset quantity );

        ACTION decallowance( name owner, asset quantity );

        ACTION open( name owner, const symbol& symbol, name ram_payer );

        ACTION close( name owner, const symbol& symbol );

        ACTION createnft( name issuer, string sym ); // nft

        ACTION issuenft( name to, asset quantity, vector<std::pair<uint64_t, std::pair<name, asset>>> token_infos, string memo );

        ACTION burnnft( name owner, asset quantity, vector<uint64_t> token_ids, string memo );

        ACTION burnnftfrom( name burner, string sym, id_type token_id, string memo );

        ACTION send( name from, name to, string sym, id_type token_id, string memo );

        ACTION approvenft( name owner, name spender, string sym, id_type token_id );

        ACTION sendfrom( name spender, name to, string sym, id_type token_id, string memo );

        ACTION auctiontoken( name auctioneer, string sym, id_type token_id, asset min_price, uint32_t sec );

        ACTION bidtoken( name bidder, string sym, id_type token_id, asset bid );

        ACTION claimtoken( name requester, string sym, id_type token_id );

        // dummy action
        ACTION bidresult( const asset& bid_key_currency ) { };

        using bidresult_action = action_wrapper<"bidresult"_n, &amft::bidresult>;

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

        static asset get_key_supply( name token_contract_account ) {
            stat_singleton _stat( token_contract_account, token_contract_account.value );
            const auto& st = _stat.get();
            return st.supply;
        }

        static asset get_key_balance( name token_contract_account, name owner, symbol_code sym_code ) {
            accounts accountstable( token_contract_account, owner.value );
            const auto& ac = accountstable.get( sym_code.raw() );
            return ac.balance;
        }

    private:
        struct [[eosio::table("keystat")]] key_currency_stat {
            asset    supply = asset{0, symbol(symbol_code("KRW"), 3)};
        };

        TABLE account {
            asset balance;

            uint64_t primary_key() const { return balance.symbol.code().raw(); }
        };

        TABLE allowance {
            asset    balance;
            name     spender;

            uint64_t primary_key() const { return balance.symbol.code().raw(); }
        };

        TABLE currency_stats {
            asset supply;
            name  issuer;

            uint64_t primary_key() const { return supply.symbol.code().raw(); }
            uint64_t get_issuer() const { return issuer.value; }
        };

        TABLE token {
            id_type token_id;    // Unique 64 bit identifier,
            name    owner;  	 // token owner
            asset   value;       // token value
	        name    tokenName;	 // token name
            name    spender;     // token spender

            id_type     primary_key() const { return token_id; }
            uint64_t    get_owner() const { return owner.value; }
        };

        TABLE token_bid {
            id_type         token_id;
            name            high_bidder;
            int64_t         high_bid = 0;
            time_point_sec  deadline;

            id_type     primary_key() const { return token_id; }
        };

        typedef eosio::singleton< "keystat"_n, key_currency_stat > stat_singleton;

        typedef eosio::multi_index<"accounts"_n, account> accounts;

        typedef eosio::multi_index< "allowances"_n, allowance > allowances;

        typedef eosio::multi_index<"stat"_n, currency_stats,
                                    indexed_by< "byissuer"_n, const_mem_fun< currency_stats, uint64_t, &currency_stats::get_issuer> >
                                  > stats;

        typedef eosio::multi_index<"token"_n, token,
                                    indexed_by< "byowner"_n, const_mem_fun< token, uint64_t, &token::get_owner> >
                                  > tokens;

        typedef eosio::multi_index< "tokenbids"_n, token_bid> token_bids;

        stat_singleton     _stat;

        key_currency_stat  _stat_state;

        void sub_balance(name owner, asset value);
        void add_balance(name owner, asset value, name ram_payer);
};