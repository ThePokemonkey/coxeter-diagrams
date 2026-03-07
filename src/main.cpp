#include <iostream>
#include <chrono>
#include "../includes/diagram.hpp"


int main() {
    //some test stuff for now idk
    
    try {
        auto begin = std::chrono::high_resolution_clock::now();

        Diagram cube("o3o3o3o3o o3o3o3*c");
        Diagram oct("o3o3o3o3o o4o3o3*b");

        for (unsigned i = 0; i < 100; ++i) {
            cube.isomorphic(oct);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin);
        std::cout << "took " << duration.count() << " milliseconds!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    
    
    return 0;
}