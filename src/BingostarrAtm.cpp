/*
 * BingostarrAtm.cpp
 *
 *  Created on: 20201010
 *      Author: bingo
 */

#include <iostream>
#include "AtmCard.hpp"
#include "AtmController.hpp"

namespace bb = bingostarr::base;

int main(int argc, char **argv) {
    std::cout << "Hello World!" << std::endl;
    bb::AtmController& atm = bb::AtmController::getInstance();
    std::cout << atm.show() << std::endl;
    atm.initialize();
    std::cout << atm.show() << std::endl;
    std::cout << "Actions: x - exit\n"
                 "         i - insert card\n"
                 "         r - remove card\n"
                 "         w - withdraw\n"
                 "         d - deposit\n"
                 "         b - balance\n";
    bb::AtmCard* a = nullptr;
    bool running = true;
    char act = 'x';
    while (running) {
        std::cout << "Choose action: ";
        std::cin >> act;
        switch (act) {
        case 'x':
            if (a) {
                std::cout << "Card present. Remove before exiting." << std::endl;
            } else {
                std::cout << "Exiting..." << std::endl;
                running = false;
            }
            break;
        case 'i':
            if (a) {
                std::cout << "Card present. Remove before inserting a new card." << std::endl;
            } else {
                std::string c;
                std::string p;
                std::cout << "Enter card number (16 digits) and pin (4 digits), n p: ";
                std::cin >> c;
                std::cin >> p;
                a = new bb::AtmCard(c,p);
                if (bb::AtmController::ATMERROR::NO_ERROR != atm.insertCard(a)) {
                    std::cout << "Card rejected!" << std::endl;
                    delete a;
                    a = nullptr;
                }
            }
            break;
        case 'r':
            if (!a) {
                std::cout << "No card present." << std::endl;
            } else {
                if (bb::AtmController::ATMERROR::NO_ERROR != atm.removeCard(a)) {
                    std::cout << "Card removal error!" << std::endl;
                }
                delete a;
                a = nullptr;
            }
            break;
        case 'w':
            if (!a) {
                std::cout << "No card present." << std::endl;
            } else {
                int64_t amt = 0;
                int64_t bal = 0;
                std::cout << "Enter withdrawal amount: ";
                std::cin >> amt;
                if (bb::AtmController::ATMERROR::NO_ERROR != atm.withdraw(a, amt, bal)) {
                    std::cout << "transaction error!" << std::endl;
                } else {
                    std::cout << amt << " withdrawn, balance: " << bal << "!" << std::endl;
                }
            }
            break;
        case 'd':
            if (!a) {
                std::cout << "No card present." << std::endl;
            } else {
                int64_t amt = 0;
                int64_t bal = 0;
                std::cout << "Enter deposit amount: ";
                std::cin >> amt;
                if (bb::AtmController::ATMERROR::NO_ERROR != atm.deposit(a, amt, bal)) {
                    std::cout << "transaction error!" << std::endl;
                } else {
                    std::cout << amt << " deposited, balance: " << bal << "!" << std::endl;
                }
            }
            break;
        case 'b':
            if (!a) {
                std::cout << "No card present." << std::endl;
            } else {
                int64_t bal = 0;
                if (bb::AtmController::ATMERROR::NO_ERROR != atm.getBalance(a, bal)) {
                    std::cout << "transaction error!" << std::endl;
                } else {
                    std::cout << "balance: " << bal << "!" << std::endl;
                }
            }
            break;
        default:
            break;
        }
        std::cout << atm.show() << std::endl;
    }
    if (a) {
        atm.removeCard(a);
        delete a;
        a = nullptr;
    }
    atm.close();
    std::cout << atm.show() << std::endl;
    std::cout << "Bye World!" << std::endl;
    return 0;
}
