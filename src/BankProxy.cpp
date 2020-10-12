/*
 * BankProxy.cpp
 *
 *  Created on: 20201010
 *      Author: bingo
 */

#include "BankProxy.hpp"
#include <fstream>
#include <sstream>

namespace bingostarr {
namespace base {

bool BankProxy::isValidString(std::string s, bool isPin) {
    // tbd: add bank specific validation of strings here
    if (isPin) {
        return (s.length() == PIN_LENGTH);
    } else {
        return (s.length() == NUM_LENGTH);
    }
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

bool BankProxy::initialize() {
    m_state = BANKSTATE::BK_BUSY;
    bool result = loadAccounts();
    if (result) {
        m_state = BANKSTATE::BK_ONLINE;
        return true;
    } else {
        m_state = BANKSTATE::BK_OFFLINE;
        m_accounts.clear();
        return false;
    }
}

bool BankProxy::close() {
    m_state = BANKSTATE::BK_OFFLINE;
    bool result  = saveAccounts();
    m_accounts.clear();
    return result;
}

bool BankProxy::loadAccounts() {
    m_accounts.clear();
    // tbd: load encrypted data from a secure banking db here.
    std::ifstream afile(ACCFILENAME.c_str());
    if (afile.fail()) {
        return false;
    }
    std::string line;
    while (std::getline(afile, line)) {
        std::istringstream linestream(line);
        std::string c;
        std::getline(linestream, c, '\t');
        if (!isValidString(c, false)) continue;
        std::string p;
        std::getline(linestream, p, '\t');
        if (!isValidString(p, true)) continue;
        std::string v;
        std::getline(linestream, v, '\t');
        bool val = ("VALID" == v);
        int64_t b = 0;
        int t = 0;
        linestream >> b;
        linestream >> t;
        m_accounts[c] = BankAccount_t(p, val, b, t);
    }
    return true;
}

bool BankProxy::saveAccounts() {
    // tbd: save encrypted data to a secure banking db here.
    std::ofstream afile(ACCFILENAME.c_str());
    if (afile.fail()) {
        return false;
    }
    if (afile.is_open()) {
        afile << "AccountNumber\tPIN\tValid\tBalance\tTxns\n";
        for (auto a : m_accounts) {
            std::string v = (a.second.valid) ? "VALID\t" : "INVALID\t";
            afile << a.first << "\t" <<
                     a.second.pin << "\t" <<
                     v << a.second.balance << "\t" <<
                     a.second.nTxns << "\n";
        }
        afile.close();
        return true;
    }
    return false;
}

ACCERROR BankProxy::validate(AtmCard* card,
                             std::string& c,
                             int64_t& balance) {
    if (BANKSTATE::BK_ONLINE != m_state) return ACCERROR::ACC_OFFLINE;
    m_state = BANKSTATE::BK_BUSY;
    if ((nullptr == card) || (!card->isValid())){
        m_state = BANKSTATE::BK_ONLINE;
        return ACCERROR::ACC_UNKNOWN;
    }
    auto itr = m_accounts.find(card->getNumber());
    if (itr == m_accounts.end()) {
        m_state = BANKSTATE::BK_ONLINE;
        return ACCERROR::ACC_UNKNOWN;
    }
    if (itr->second.pin != card->getStrongPin()) {
        m_state = BANKSTATE::BK_ONLINE;
        return ACCERROR::ACC_AUTHERROR;
    }
    m_state = BANKSTATE::BK_ONLINE;
    c = card->getNumber();
    balance = itr->second.balance;
    return ACCERROR::ACC_NOERROR;
}

ACCERROR BankProxy::addAccount(AtmCard* card,
                               const int64_t& amt)
{
    if (BANKSTATE::BK_ONLINE != m_state) return ACCERROR::ACC_OFFLINE;
    m_state = BANKSTATE::BK_BUSY;
    if ((nullptr == card) || (!card->isValid())){
        m_state = BANKSTATE::BK_ONLINE;
        return ACCERROR::ACC_UNKNOWN;
    }
    if (m_accounts.find(card->getNumber()) != m_accounts.end()) {
        m_state = BANKSTATE::BK_ONLINE;
        return ACCERROR::ACC_OTHER;
    }
    m_accounts[card->getNumber()] = BankAccount_t(card->getStrongPin(), amt);
    m_state = BANKSTATE::BK_ONLINE;
    return ACCERROR::ACC_NOERROR;
}

ACCERROR BankProxy::rmvAccount(AtmCard* card)
{
    std::string c = ERR_STR;
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
    std::string c = ERR_STR;
    ACCERROR err = validate(card, c, balance);
    return err;
}

ACCERROR BankProxy::withdraw(AtmCard* card,
                             const int64_t& amt,
                             int64_t& balance)
{
    if (amt < 0) {
        return ACCERROR::ACC_OTHER;
    }
    std::string c = ERR_STR;
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
                            const int64_t& amt,
                            int64_t& balance)
{
    if (amt < 0) {
        return ACCERROR::ACC_OTHER;
    }
    std::string c = ERR_STR;
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
    int64_t x = itr->second.balance + amt;
    if (x > MAX_ACC_BAL) {
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
    std::string outStr = "\n";
    switch (m_state) {
        case BANKSTATE::BK_BUSY:
            outStr += "BANK: BUSY\n";
            break;
        case BANKSTATE::BK_ONLINE:
            outStr += "BANK: ONLINE\n";
            break;
        default:
            outStr += "BANK: OFFLINE\n";
            break;
        }
    return outStr;
}

std::string BankProxy::showAccounts() {
    std::string outStr = "Account Number     \tPIN\tValid\tBalance   \tTxns\n";
    for (auto a : m_accounts) {
        outStr += a.first + "\t";
        outStr += a.second.pin + "\t";
        outStr += a.second.valid ? "VALID\t" : "INVALID\t";
        outStr += std::to_string(a.second.balance) + "   \t";
        outStr += std::to_string(a.second.nTxns) + "\n";
    }
    return outStr;
}

} /* base */
} /* bingostarr */
