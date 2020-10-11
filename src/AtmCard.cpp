/*
 * AtmCard.cpp
 *
 *  Created on: 20201010
 *      Author: bingo
 */

#include "AtmCard.hpp"
#include "BankProxy.hpp"

namespace bingostarr {
namespace base {

AtmCard::AtmCard(const std::string& number,
                 const std::string& pin)
        : m_isValid(((number.length() == NUM_LENGTH) &&
                     (BankProxy::isValidNumber(number)) &&
                     (pin.length() == PIN_LENGTH)) &&
                     (BankProxy::isValidNumber(pin))),
          m_number(m_isValid ? BankProxy::encrypt(number) : ERR_STR),
          m_strongPin(m_isValid ? BankProxy::encrypt(pin) : ERR_STR) {
}

AtmCard::AtmCard(AtmCard* card)
        : m_isValid((card) ? card->isValid() : false),
          m_number(m_isValid ? card->getNumber() : ERR_STR),
          m_strongPin(m_isValid ? card->getStrongPin() : ERR_STR) {
}

const bool& AtmCard::isValid() const {
    return m_isValid;
}

const std::string& AtmCard::getNumber() const {
    return m_number;
}

const std::string& AtmCard::getStrongPin() const {
    return m_strongPin;
}

std::string AtmCard::show() {
    std::string outStr = "\nATM Card { ";
    if (m_isValid) {
        outStr += m_number + " : " + m_strongPin;
    } else {
        outStr += "INVALID";
    }
    outStr += " }\n";
    return outStr;
}
} /* base */
} /* bingostarr */
