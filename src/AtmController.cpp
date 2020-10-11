/*
 * AtmController.cpp
 *
 *  Created on: 20201010
 *      Author: bingo
 */

#include "AtmController.hpp"
#include "CashBin.hpp"

namespace bingostarr {
namespace base {

static std::map<ACCERROR, AtmController::ATMERROR> ErrorMap =
                      { { ACCERROR::ACC_NOERROR, AtmController::ATMERROR::NO_ERROR },
                        { ACCERROR::ACC_UNKNOWN, AtmController::ATMERROR::BANK_ERROR },
                        { ACCERROR::ACC_OFFLINE, AtmController::ATMERROR::BANK_ERROR },
                        { ACCERROR::ACC_LOW_BAL, AtmController::ATMERROR::CASH_ERROR },
                        { ACCERROR::ACC_MAX_TXNS, AtmController::ATMERROR::CASH_ERROR },
                        { ACCERROR::ACC_INVALID, AtmController::ATMERROR::AUTH_ERROR },
                        { ACCERROR::ACC_OVERDRAFT, AtmController::ATMERROR::CASH_ERROR },
                        { ACCERROR::ACC_AUTHERROR, AtmController::ATMERROR::AUTH_ERROR },
                        { ACCERROR::ACC_OTHER, AtmController::ATMERROR::BANK_ERROR } };

AtmController::AtmController()
        : m_state(ATMSTATE::SLEEP), m_card(nullptr) {

}

AtmController::~AtmController() {
    m_state = ATMSTATE::SLEEP;
    killCard();
}

void AtmController::killCard() {
    if (m_card) {
        delete m_card;
        m_card = nullptr;
    }
}

void AtmController::initialize() {
    m_state = ATMSTATE::BUSY;
    BankProxy::getInstance().initialize();
    CashBin::getInstance().initialize();
    killCard();
    m_state = ATMSTATE::READY;
}

void AtmController::close() {
    if (ATMSTATE::CARDED == m_state) {
        removeCard(m_card);
    }
    m_state = ATMSTATE::BUSY;
    killCard();
    CashBin::getInstance().close();
    BankProxy::getInstance().close();
    m_state = ATMSTATE::SLEEP;
}

AtmController::ATMERROR AtmController::insertCard(AtmCard *card) {
    int64_t b = 0;
    return insertCardInt(card, b);
}

AtmController::ATMERROR AtmController::removeCard(AtmCard* card) {
    switch (m_state) {
    case ATMSTATE::CARDED: {
        m_state = ATMSTATE::BUSY;
        if (!checkCard(card)) {
            m_state = ATMSTATE::CARDED;
            return ATMERROR::CARD_ERROR;
        }
        killCard();
        m_state = ATMSTATE::READY;
        return ATMERROR::NO_ERROR;
    }
    case ATMSTATE::OFFLINE:
        return ATMERROR::BANK_ERROR;
    case ATMSTATE::READY:
        return ATMERROR::NO_ERROR;
    default:
        return ATMERROR::SYSTEM_ERROR;
    }
}

bool AtmController::checkCard(AtmCard* card) {
    if (!card || !m_card) {
        return false;
    }
    if (!card->isValid() || !m_card->isValid()) {
        return false;
    }
    if ((card->getNumber() != m_card->getNumber()) ||
        (card->getStrongPin() != m_card->getStrongPin())) {
        return false;
    }
    return true;
}

AtmController::ATMERROR AtmController::quickBalanceOnly(AtmCard* card,
                                                        int64_t& balance) {
    int64_t b = 0;
    ATMERROR err = insertCardInt(card, b);
    if (ATMERROR::NO_ERROR == err) {
        balance = b;
    }
    err = removeCard(card);
    return err;
}

AtmController::ATMERROR AtmController::getBalance(AtmCard* card,
                                                  int64_t& balance) {
    switch (m_state) {
    case ATMSTATE::CARDED: {
        m_state = ATMSTATE::BUSY;
        if (!checkCard(card)) {
            m_state = ATMSTATE::CARDED;
            return ATMERROR::CARD_ERROR;
        }
        int64_t b = 0;
        if (!validate(b)) {
            m_state = ATMSTATE::CARDED;
            return ATMERROR::AUTH_ERROR;
        }
        balance = b;
        m_state = ATMSTATE::READY;
        return ATMERROR::NO_ERROR;
    }
    case ATMSTATE::OFFLINE:
        return ATMERROR::BANK_ERROR;
    case ATMSTATE::READY:
        return ATMERROR::CARD_ERROR;
    default:
        return ATMERROR::SYSTEM_ERROR;
    }
}

AtmController::ATMERROR AtmController::withdraw(AtmCard* card,
                                                const uint64_t& amount,
                                                int64_t& balance) {
    switch (m_state) {
    case ATMSTATE::CARDED: {
        m_state = ATMSTATE::BUSY;
        if (!checkCard(card)) {
            m_state = ATMSTATE::CARDED;
            return ATMERROR::CARD_ERROR;
        }
        if (!CashBin::getInstance().withdraw(amount)) {
            m_state = ATMSTATE::CARDED;
            return ATMERROR::CASH_ERROR;
        }
        ACCERROR acErr =
                BankProxy::getInstance().withdraw(m_card,
                                                  amount,
                                                  balance);
        m_state = ATMSTATE::CARDED;
        ATMERROR err = ErrorMap[acErr];
        return err;
    }
    case ATMSTATE::OFFLINE:
        return ATMERROR::BANK_ERROR;
    case ATMSTATE::READY:
        return ATMERROR::CARD_ERROR;
    default:
        return ATMERROR::SYSTEM_ERROR;
    }
}

AtmController::ATMERROR AtmController::deposit(AtmCard* card,
                                               const uint64_t& amount,
                                               int64_t& balance) {
    switch (m_state) {
    case ATMSTATE::CARDED: {
        m_state = ATMSTATE::BUSY;
        if (!checkCard(card)) {
            m_state = ATMSTATE::CARDED;
            return ATMERROR::CARD_ERROR;
        }
        if (!CashBin::getInstance().deposit(amount)) {
            m_state = ATMSTATE::CARDED;
            return ATMERROR::CASH_ERROR;
        }
        ACCERROR acErr =
                BankProxy::getInstance().deposit(m_card,
                                                 amount,
                                                 balance);
        m_state = ATMSTATE::CARDED;
        ATMERROR err = ErrorMap[acErr];
        return err;
    }
    case ATMSTATE::OFFLINE:
        return ATMERROR::BANK_ERROR;
    case ATMSTATE::READY:
        return ATMERROR::CARD_ERROR;
    default:
        return ATMERROR::SYSTEM_ERROR;
    }
}

AtmController::ATMERROR AtmController::insertCardInt(AtmCard *card,
                                                     int64_t &balance) {
    switch (m_state) {
    case ATMSTATE::READY: {
        killCard();
        if (!card || (!card->isValid())) {
            return ATMERROR::CARD_ERROR;
        }
        m_state = ATMSTATE::BUSY;
        m_card = new AtmCard(card);
        int64_t b = 0;
        if (!validate(b)) {
            killCard();
            m_state = ATMSTATE::READY;
            return ATMERROR::AUTH_ERROR;
        }
        balance = b;
        m_state = ATMSTATE::CARDED;
        return ATMERROR::NO_ERROR;
    }
    case ATMSTATE::CARDED:
        return ATMERROR::CARD_ERROR;
    case ATMSTATE::OFFLINE:
        return ATMERROR::BANK_ERROR;
    default:
        return ATMERROR::SYSTEM_ERROR;
    }
}

bool AtmController::validate(int64_t& balance) {
    return (ACCERROR::ACC_NOERROR == BankProxy::getInstance().validate(m_card, balance));
}

std::string AtmController::show() {
    std::string outStr = "====" + BankProxy::getInstance().show() + CashBin::getInstance().show();
    switch (m_state) {
    case ATMSTATE::SLEEP:
        outStr += "ATM: SLEEP\n";
        break;
    case ATMSTATE::READY:
        outStr += "ATM: READY\n";
        break;
    case ATMSTATE::CARDED: {
        outStr += "ATM: CARDED\n";
        if (m_card) {
            outStr += m_card->show();
            int64_t b = 0;
            if (validate(b)) {
                outStr += "Balance: $" + std::to_string(b);
            } else {
                outStr += "!!card declined!!\n";
            }
        } else {
            outStr += "!!null card!!\n";
        }
        break;
    }
    case ATMSTATE::BUSY:
        outStr += "ATM: BUSY\n";
        break;
    case ATMSTATE::OFFLINE:
        outStr += "ATM: OFFLINE\n";
        break;
    case ATMSTATE::SERVICE:
        outStr += "ATM: SERVICE\n";
        break;
    default:
        outStr += "ATM: ERROR\n";
        break;
    }
    return outStr;
}
} /* base */
} /* bingostarr */
