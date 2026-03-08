#include <iostream>
#include <chrono>
#include "../includes/diagram.hpp"


int main() {
    //some test stuff for now idk
    
    try {
        auto begin = std::chrono::high_resolution_clock::now();

        Diagram cube("o3o4o5o");

        std::cout << cube << std::endl;
        std::cout << cube.subDiagram(1) << std::endl;
        std::cout << cube.subDiagram({1,2}) << std::endl;
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin);
        std::cout << "took " << duration.count() << " milliseconds!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    
    
    return 0;
}