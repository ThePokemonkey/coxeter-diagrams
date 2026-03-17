#include <iostream>
#include <chrono>
#include "../includes/adder.hpp"


int main() {
    //some test stuff for now idk
    
    try {
        auto begin = std::chrono::high_resolution_clock::now();

        Label test(4,3);
        Label test2;
        std::cout << Label(test.getChord()) << std::endl;
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin);
        std::cout << "took " << duration.count() << " milliseconds!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    
    
    return 0;
}