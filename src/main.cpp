#include <iostream>
#include <chrono>

int main(int argc, char **argv) {
    std::cout << "Hello World!" << std::endl;
    auto start = std::chrono::steady_clock::now();

    auto end = std::chrono::steady_clock::now();
    std::cout << "elapsed time: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << "s" << std::endl;
    std::cout << "Bye World!" << std::endl;
    return 0;
}
