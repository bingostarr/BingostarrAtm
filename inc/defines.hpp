/*
 * defines.hpp
 *
 *  Created on: 20201010
 *      Author: bingo
 */

#ifndef INC_DEFINES_HPP_
#define INC_DEFINES_HPP_

namespace bingostarr {
namespace base {

enum class ATMERROR {
    NO_ERROR,
    SYS_BUSY,
    AUTH_ERROR,
    BANK_TIMEOUT,
    CASH_ERROR,
    /*
     * ...
     */
    MAX_ERROR
};

enum class ATMSTATE {
    AVAILABLE,
    CARD_ACCEPTED,
    CARD_FAILED,
    BUSY,
    SERVICE,
    NO_CASH,
    ERROR
};

} /* base */
} /* bingostarr */
#endif /* INC_DEFINES_HPP_ */
