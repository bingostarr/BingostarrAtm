cmake_minimum_required(VERSION 3.11.3)

# Set the C++ standard we will use
set(CMAKE_CXX_STANDARD 17)

# Add the path of the cmake files to the CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

project(bingostarr_atm)

# Add project executable
add_executable(bingostarr_atm src/BingostarrAtm.cpp src/AtmController.cpp src/AtmCard.cpp src/BankProxy.cpp)
target_include_directories(bingostarr_atm PRIVATE inc)
