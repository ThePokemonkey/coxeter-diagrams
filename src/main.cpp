#include <iostream>
#include <chrono>
#include "../includes/adder.hpp"


int main() {
    //some test stuff for now idk
    
    try {
        auto begin = std::chrono::high_resolution_clock::now();

        Diagram cube("o3o4o");
        Diagram cube2("o4o4o3/2*a");

        std::vector<Diagram> adds = makeAdditions(cube,cube2);
        for (unsigned i = 0; i < adds.size(); ++i) {
            std::cout << adds[i] << std::endl;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin);
        std::cout << "took " << duration.count() << " milliseconds!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    
    
    return 0;
}