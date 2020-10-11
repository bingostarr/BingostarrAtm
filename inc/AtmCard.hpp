/*
 * AtmCard.hpp
 *
 *  Created on: 20201010
 *      Author: bingo
 */

#ifndef INC_ATMCARD_HPP_
#define INC_ATMCARD_HPP_

#include <string>

namespace bingostarr {
namespace base {

class AtmCard {
public:
    AtmCard(const std::string& number,
            const std::string& pin);
    AtmCard(AtmCard* card);
    ~AtmCard() = default;

    const bool& isValid() const;
    const std::string& getNumber() const;
    const std::string& getStrongPin() const;
    std::string show();

private:
    AtmCard(const AtmCard&) = delete;
    AtmCard& operator=(const AtmCard&) = delete;
    bool m_isValid;
    std::string m_number;
    std::string m_strongPin;
};

} /* base */
} /* bingostarr */
#endif /* INC_ATMCARD_HPP_ */
