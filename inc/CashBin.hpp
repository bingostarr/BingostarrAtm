/*
 * CashBin.hpp
 *
 *  Created on: 20201010
 *      Author: bingo
 */

#ifndef INC_CASHBIN_HPP_
#define INC_CASHBIN_HPP_

#include <string>

namespace bingostarr {
namespace base {

static const int64_t MIN_CASH_ATM = 0;
static const int64_t START_CASH_ATM = 50000;
static const int64_t MAX_CASH_ATM = 100000;
static const int64_t MAX_CASH_TXN = 1000;

class CashBin {
public:
    static CashBin& getInstance() {
        static CashBin inst;
        return inst;
    }
    ~CashBin() = default;

    const int64_t& getBalance() const {
        return m_balance;
    }

    bool initialize() {
        m_balance = START_CASH_ATM;
        return true;
    }

    bool close() {
        m_balance = MIN_CASH_ATM;
        return true;
    }

    const bool available(const int64_t& amount) {
        if ((m_balance - amount) < MIN_CASH_ATM) return false;
        return true;
    }

    const bool depositAllowed(const int64_t& amount) {
        if ((m_balance + amount) > MAX_CASH_ATM) return false;
        return true;
    }

    const bool withdraw(const int64_t& amount) {
        if ((m_balance - amount) < MIN_CASH_ATM) return false;
        m_balance -= amount;
        return true;
    }

    const bool deposit(const int64_t& amount) {
        if ((m_balance + amount) > MAX_CASH_ATM) return false;
        m_balance += amount;
        return true;
    }

    std::string show() {
        return "Cash Bin Balance: " + std::to_string(m_balance) + "\n";
    }
private:
    CashBin() = default;
    CashBin(const CashBin&) = delete;
    CashBin& operator=(const CashBin&) = delete;
    int64_t m_balance = MIN_CASH_ATM;
};

} /* base */
} /* bingostarr */
#endif /* INC_CASHBIN_HPP_ */
