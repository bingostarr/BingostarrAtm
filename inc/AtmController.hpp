/*
 * AtmController.hpp
 *
 *  Created on: 20201010
 *      Author: bingo
 */

#ifndef INC_ATMCONTROLLER_HPP_
#define INC_ATMCONTROLLER_HPP_

#include <string>
#include <map>
#include "BankProxy.hpp"
#include "AtmCard.hpp"

namespace bingostarr {
namespace base {

enum class ATMSTATE {
    SLEEP,
    READY,
    CARDED,
    BUSY,
    OFFLINE,
    SERVICE,
    ERROR
};

class AtmController {
public:
    enum class ATMERROR {
        NO_ERROR,
        CARD_ERROR,
        AUTH_ERROR,
        BANK_ERROR,
        CASH_ERROR,
        SYSTEM_ERROR,
        MAX_ERROR
    };
    static AtmController& getInstance() {
        static AtmController inst;
        return inst;
    }
    ~AtmController();
    void initialize();
    void close();
    ATMERROR insertCard(AtmCard* card);
    ATMERROR removeCard(AtmCard* card);
    ATMERROR quickBalanceOnly(AtmCard* card,
                              int64_t& balance);
    ATMERROR getBalance(AtmCard* card,
                        int64_t& balance);
    ATMERROR withdraw(AtmCard* card,
                      const uint64_t& amount,
                      int64_t& balance);
    ATMERROR deposit(AtmCard* card,
                     const uint64_t& amount,
                     int64_t& balance);
    std::string show();
private:
    AtmController();
    AtmController(const AtmController&) = delete;
    AtmController& operator=(const AtmController&) = delete;
    ATMSTATE m_state;
    AtmCard* m_card;
    bool checkCard(AtmCard* card);
    void killCard();
    ATMERROR insertCardInt(AtmCard* card,
                           int64_t& balance);
    bool validate(int64_t& balance);
};

} /* base */
} /* bingostarr */
#endif /* INC_ATMCONTROLLER_HPP_ */
