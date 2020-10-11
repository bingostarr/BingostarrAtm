/*
 * BankProxy.cpp
 *
 *  Created on: 20201010
 *      Author: bingo
 */

#include "BankProxy.hpp"
#include <sstream>

namespace bingostarr {
namespace base {

bool BankProxy::isValidNumber(std::string s) {
    std::string::const_iterator itr = s.begin();
    while (itr != s.end() && std::isdigit(*itr)) ++itr;
    return (!s.empty() && itr == s.end());
}

uint64_t BankProxy::numberStr2Int(std::string s) {
    uint64_t n = ERR_NUMBER;
    std::istringstream iss(s);
    if (!iss.fail()) iss >> n;
    return n;
}

std::string BankProxy::encrypt(std::string s) {
    std::string es = s;
    // tbd: implement an encryption here.
    return es;
}

std::string BankProxy::decrypt(std::string s) {
    std::string ds = s;
    // tbd: implement an encryption here.
    return ds;
}

void BankProxy::initialize() {
    m_state = BANKSTATE::BK_ONLINE;
    m_accounts.clear();
    // tbd: load from a secure banking db here.
    uint64_t c = 4147987054324598;
    int n = 10;
    int p = 4134;
    int64_t b = 644512;
    for (int i = 0; i < n; ++i) {
        m_accounts[c + i] = BankAccount_t(encrypt(std::to_string(p)), b);
    }
}

void BankProxy::close() {
    m_state = BANKSTATE::BK_OFFLINE;
    // tbd: save to a secure banking db here.
    m_accounts.clear();
}

ACCERROR BankProxy::validate(AtmCard* card,
                             uint64_t& c,
                             int64_t& balance) {
    if (BANKSTATE::BK_ONLINE != m_state) return ACCERROR::ACC_OFFLINE;
    m_state = BANKSTATE::BK_BUSY;
    if ((nullptr == card) || (!card->isValid())){
        m_state = BANKSTATE::BK_ONLINE;
        return ACCERROR::ACC_UNKNOWN;
    }
    c = numberStr2Int(decrypt(card->getNumber()));
    if (ERR_NUMBER == c) {
        m_state = BANKSTATE::BK_ONLINE;
        return ACCERROR::ACC_UNKNOWN;
    }
    auto itr = m_accounts.find(c);
    if (itr == m_accounts.end()) {
        m_state = BANKSTATE::BK_ONLINE;
        return ACCERROR::ACC_UNKNOWN;
    }
    if (itr->second.pin != card->getStrongPin()) {
        m_state = BANKSTATE::BK_ONLINE;
        return ACCERROR::ACC_AUTHERROR;
    }
    m_state = BANKSTATE::BK_ONLINE;
    balance = itr->second.balance;
    return ACCERROR::ACC_NOERROR;
}

ACCERROR BankProxy::addAccount(AtmCard* card,
                               const uint64_t& amt)
{
    if (BANKSTATE::BK_ONLINE != m_state) return ACCERROR::ACC_OFFLINE;
    m_state = BANKSTATE::BK_BUSY;
    if ((nullptr == card) || (!card->isValid())){
        m_state = BANKSTATE::BK_ONLINE;
        return ACCERROR::ACC_UNKNOWN;
    }
    uint64_t c = numberStr2Int(decrypt(card->getNumber()));
    if (ERR_NUMBER == c) {
        m_state = BANKSTATE::BK_ONLINE;
        return ACCERROR::ACC_UNKNOWN;
    }
    if (m_accounts.find(c) != m_accounts.end()) {
        m_state = BANKSTATE::BK_ONLINE;
        return ACCERROR::ACC_OTHER;
    }
    m_accounts[c] = BankAccount_t(card->getStrongPin(), amt);
    m_state = BANKSTATE::BK_ONLINE;
    return ACCERROR::ACC_NOERROR;
}

ACCERROR BankProxy::rmvAccount(AtmCard* card)
{
    uint64_t c = ERR_NUMBER;
    int64_t b = 0;
    ACCERROR err = validate(card, c, b);
    m_state = BANKSTATE::BK_BUSY;
    if (ACCERROR::ACC_NOERROR == err) {
        m_accounts.erase(c);
    }
    m_state = BANKSTATE::BK_ONLINE;
    return err;
}

ACCERROR BankProxy::validate(AtmCard* card,
                             int64_t& balance) {
    uint64_t c = ERR_NUMBER;
    ACCERROR err = validate(card, c, balance);
    return err;
}

ACCERROR BankProxy::withdraw(AtmCard* card,
                             const uint64_t& amt,
                             int64_t& balance)
{
    uint64_t c = ERR_NUMBER;
    ACCERROR err = validate(card, c, balance);
    m_state = BANKSTATE::BK_BUSY;
    if (ACCERROR::ACC_NOERROR != err) {
        m_state = BANKSTATE::BK_ONLINE;
        return err;
    }
    auto itr = m_accounts.find(c);
    if (itr == m_accounts.end()) {
        m_state = BANKSTATE::BK_ONLINE;
        return ACCERROR::ACC_UNKNOWN;
    }
    if (itr->second.nTxns <= 0) {
        m_state = BANKSTATE::BK_ONLINE;
        return ACCERROR::ACC_MAX_TXNS;
    }
    if (!itr->second.valid) {
        m_state = BANKSTATE::BK_ONLINE;
        return ACCERROR::ACC_INVALID;
    }
    if (itr->second.balance < 0) {
        m_state = BANKSTATE::BK_ONLINE;
        return ACCERROR::ACC_OVERDRAFT;
    }
    if (itr->second.balance - amt < 0) {
        m_state = BANKSTATE::BK_ONLINE;
        return ACCERROR::ACC_LOW_BAL;
    }
    itr->second.balance -= amt;
    itr->second.nTxns--;
    balance = itr->second.balance;
    m_state = BANKSTATE::BK_ONLINE;
    return ACCERROR::ACC_NOERROR;
}

ACCERROR BankProxy::deposit(AtmCard* card,
                            const uint64_t& amt,
                            int64_t& balance)
{
    uint64_t c = ERR_NUMBER;
    ACCERROR err = validate(card, c, balance);
    m_state = BANKSTATE::BK_BUSY;
    if (ACCERROR::ACC_NOERROR != err) {
        m_state = BANKSTATE::BK_ONLINE;
        return err;
    }
    auto itr = m_accounts.find(c);
    if (itr == m_accounts.end()) {
        m_state = BANKSTATE::BK_ONLINE;
        return ACCERROR::ACC_UNKNOWN;
    }
    if (itr->second.nTxns <= 0) {
        m_state = BANKSTATE::BK_ONLINE;
        return ACCERROR::ACC_MAX_TXNS;
    }
    if (!itr->second.valid) {
        m_state = BANKSTATE::BK_ONLINE;
        return ACCERROR::ACC_INVALID;
    }
    if (itr->second.balance + amt > MAX_ACC_BAL) {
        m_state = BANKSTATE::BK_ONLINE;
        return ACCERROR::ACC_OTHER;
    }
    itr->second.balance += amt;
    itr->second.nTxns--;
    balance = itr->second.balance;
    m_state = BANKSTATE::BK_ONLINE;
    return ACCERROR::ACC_NOERROR;
}

std::string BankProxy::show() {
    std::string outStr = "\nAccount Number     \tPIN\tValid\tBalance\tTxns\n";
    for (auto a : m_accounts) {
        outStr += std::to_string(a.first) + "\t";
        outStr += a.second.pin + "\t";
        outStr += a.first ? "VALID\t" : "INVALID\t";
        outStr += std::to_string(a.second.balance) + "\t";
        outStr += std::to_string(a.second.nTxns) + "\n";
    }
    return outStr;
}

} /* base */
} /* bingostarr */
