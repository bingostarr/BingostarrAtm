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

const uint64_t ERR_NUMBER = 0;
const std::string ERR_STR = "";
const int NUM_LENGTH = 16;
const int PIN_LENGTH = 4;

static const uint64_t MAX_ACC_BAL = 10000000;
static const uint64_t MAX_ACC_TXNS = 10;

enum class BANKSTATE {
    BK_UNKNOWN,
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
} BankAccount_t;

typedef std::map<uint64_t, BankAccount_t> AccountsDb;

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
    BANKSTATE m_state = BANKSTATE::BK_UNKNOWN;
    AccountsDb m_accounts;
    ACCERROR validate(AtmCard* card,
                      uint64_t& c,
                      int64_t& balance);
    void initialize();
    void close();
    ACCERROR addAccount(AtmCard* card,
                        const uint64_t& amt = 0);
    ACCERROR validate(AtmCard* card,
                      int64_t& balance);
    ACCERROR rmvAccount(AtmCard* card);
    ACCERROR withdraw(AtmCard* card,
                      const uint64_t& amt,
                      int64_t& balance);
    ACCERROR deposit(AtmCard* card,
                     const uint64_t& amt,
                     int64_t& balance);
    std::string show();
    static bool isValidNumber(std::string s);
    static uint64_t numberStr2Int(std::string s);
    static std::string encrypt(std::string s);
    static std::string decrypt(std::string s);
    friend AtmController;
    friend AtmCard;
};

} /* base */
} /* bingostarr */
#endif /* INC_BANKPROXY_HPP_ */
