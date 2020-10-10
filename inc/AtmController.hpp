/*
 * AtmController.hpp
 *
 *  Created on: 20201010
 *      Author: bingo
 */

#ifndef INC_ATMCONTROLLER_HPP_
#define INC_ATMCONTROLLER_HPP_

#include <string>

#include "defines.hpp"

namespace bingostarr {
namespace base {

class AtmController {
public:
    static AtmController& getInstance() {
        static AtmController inst;
        return inst;
    }
    ~AtmController();
    ATMERROR insertCard(const uint64_t& cardNumber,
                        const std::string& cardPin);
    ATMERROR removeCard();
    ATMERROR quickBalanceOnly(const uint64_t& cardNumber,
                              const std::string& cardPin,
                              int64_t& balance);
    int64_t getBalance();
    ATMERROR withdraw(const uint64_t& amount,
                      int64_t& balance);
    ATMERROR deposit(const uint64_t& amount,
                     int64_t& balance);
private:
    AtmController();
    AtmController(const AtmController&) = delete;
    AtmController& operator=(const AtmController&) = delete;
    ATMSTATE m_atmState;
};

} /* base */
} /* bingostarr */
#endif /* INC_ATMCONTROLLER_HPP_ */
