/*
 * BankProxy.hpp
 *
 *  Created on: 20201010
 *      Author: bingo
 */

#ifndef INC_BANKPROXY_HPP_
#define INC_BANKPROXY_HPP_

#include <map>
#include "AtmCard.hpp"

namespace bingostarr {
namespace base {

const std::string ERR_STR = "";
const int NUM_LENGTH = 16;
const int PIN_LENGTH = 4;

static const int64_t MAX_ACC_BAL = 100000000;
static const int MAX_ACC_TXNS = 10;
static const std::string ACCFILENAME = "accountsDb.txt";

enum class BANKSTATE {
    BK_OFFLINE,
    BK_BUSY,
    BK_ONLINE,
};

enum class ACCERROR {
    ACC_NOERROR,
    ACC_UNKNOWN,
    ACC_OFFLINE,
    ACC_LOW_BAL,
    ACC_MAX_TXNS,
    ACC_INVALID,
    ACC_OVERDRAFT,
    ACC_AUTHERROR,
    ACC_OTHER,
};

typedef struct BankAccount {
    std::string pin;
    bool valid;
    int64_t balance;
    int nTxns;
    BankAccount()
    : pin(ERR_STR), valid(false), balance(0), nTxns(0) {
    }
    BankAccount(const std::string& p,
                const int64_t& b)
    : pin(p), valid(true), balance(b), nTxns(MAX_ACC_TXNS) {
    }
    BankAccount(const std::string& p,
                const bool& v,
                const int64_t& b,
                const int& t)
    : pin(p), valid(v), balance(b), nTxns(t) {
    }
} BankAccount_t;

typedef std::map<std::string, BankAccount_t> AccountsDb;

class AtmController;

class BankProxy {
public:
    static BankProxy& getInstance() {
        static BankProxy inst;
        return inst;
    }

    ~BankProxy() = default;

private:
    BankProxy() = default;
    BankProxy(const BankProxy&) = delete;
    BankProxy& operator=(const BankProxy&) = delete;
    BANKSTATE m_state = BANKSTATE::BK_OFFLINE;
    AccountsDb m_accounts;
    ACCERROR validate(AtmCard* card,
                      std::string& c,
                      int64_t& balance);
    bool initialize();
    bool close();
    bool loadAccounts();
    bool saveAccounts();
    ACCERROR addAccount(AtmCard* card,
                        const int64_t& amt = 0);
    ACCERROR validate(AtmCard* card,
                      int64_t& balance);
    ACCERROR rmvAccount(AtmCard* card);
    ACCERROR withdraw(AtmCard* card,
                      const int64_t& amt,
                      int64_t& balance);
    ACCERROR deposit(AtmCard* card,
                     const int64_t& amt,
                     int64_t& balance);
    std::string show();
    std::string showAccounts();
    static bool isValidString(std::string s, bool isPin);
    static std::string encrypt(std::string s);
    static std::string decrypt(std::string s);
    friend AtmController;
    friend AtmCard;
};

} /* base */
} /* bingostarr */
#endif /* INC_BANKPROXY_HPP_ */
