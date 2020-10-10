cmake_minimum_required(VERSION 3.11.3)

# Set the C++ standard we will use
set(CMAKE_CXX_STANDARD 17)

# Add the path of the cmake files to the CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

project(bingostarr_atm)

# Set library output path to /lib
set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/lib")

# Add project executable
add_executable(bingostarr_atm src/main.cpp)

target_link_libraries(bingostarr_atm PUBLIC pthread)
target_include_directories(bingostarr_atm PRIVATE inc)
