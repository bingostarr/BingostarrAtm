# Bingo Starr ATM
# A Simple ATM Controller in C++

This repo contains C++ code for Bingo Starr ATM: A Simple ATM Controller.

#### Goal
Write code for a simple ATM. It doesn't need any UI (either graphical or console), but a controller should be implemented and tested.

#### Requirements

At least the following flow should be implemented:

Insert Card => PIN number => Select Account => See Balance/Deposit/Withdraw

For simplification, there are only 1 dollar bills in this world, no cents. Thus account balance can be represented in integer.

Your code doesn't need to integrate with a real bank system, but keep in mind that we may want to integrate it with a real bank system in the future.

It doesn't have to integrate with a real cash bin in the ATM, but keep in mind that we'd want to integrate with that in the future. And even if we integrate it with them, we'd like to test our code.

Implementing bank integration and ATM hardware like cash bin and card reader is not a scope of this task, but testing the controller part (not including bank system, cash bin etc) is within the scope.

A bank API wouldn't give the ATM the PIN number, but it can tell you if the PIN number is correct or not.

Based on your work, another engineer should be able to implement the user interface. You don't need to implement any REST API, RPC, network communication etc, but just functions/classes/methods, etc.

You can simplify some complex real world problems if you think it's not worth illustrating in the project.

#### Code Directory Structure
* */inc* - C++ header files (**.hpp*)
* */src* - C++ source files (**.cpp*)
* */data* - account database file (**.txt*)

## Dependencies for Running Locally
* cmake >= 3.11
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
install v3.0 or greater.

## Build and Execute Instructions

1. Clone this repo.
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Copy data into build folder: `cp ../data/accountsDb.txt .`
5. Run it: `./bingostarr_atm`.
6. Follow the on-screen prompts to take various following actions:
  * _x_  - exit
  * _i_  - insert card (please refer to [this file](/data/AccountDb.txt) to pick an existing account)
  * _r_  - remove card
  * _w_  - withdraw
  * _d_  - deposit
  * _b_  - balance

