/*
 * BingostarrAtm.cpp
 *
 *  Created on: 20201010
 *      Author: bingo
 */

#include <iostream>
#include <chrono>
#include "AtmCard.hpp"
#include "AtmController.hpp"

namespace bb = bingostarr::base;

int main(int argc, char **argv) {
    std::cout << "Hello World!" << std::endl;
    auto start = std::chrono::steady_clock::now();
    std::string c = "4147987054324598";
    std::string p = "4134";
    bb::AtmCard* a = new bb::AtmCard(c,p);
    bb::AtmController& atm = bb::AtmController::getInstance();
    std::cout << atm.show() << std::endl;
    atm.initialize();
    std::cout << atm.show() << std::endl;
    bb::AtmController::ATMERROR err = atm.insertCard(a);
    int64_t b = 0;
    if (bb::AtmController::ATMERROR::NO_ERROR == err) {
        std::cout << atm.show() << std::endl;
        atm.withdraw(a, 2356000, b);
        std::cout << atm.show() << std::endl;
        atm.deposit(a, 2356, b);
    }
    std::cout << atm.show() << std::endl;
    atm.removeCard(a);
    std::cout << atm.show() << std::endl;
    atm.close();
    delete a;
    auto end = std::chrono::steady_clock::now();
    std::cout << "elapsed time: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << "s" << std::endl;
    std::cout << "Bye World!" << std::endl;
    return 0;
}
