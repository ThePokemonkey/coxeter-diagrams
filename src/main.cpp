#include <iostream>
#include <chrono>
#include "../includes/adder.hpp"


int main() {
    //some test stuff for now idk
    
    try {
        auto begin = std::chrono::high_resolution_clock::now();

        Label test(4,2);
        Label test2;
        std::cout << test.isComplementary(test2) << std::endl;

        Diagram cube("o5o5o5o");

        std::vector<Diagram> adds = recursiveAdder({cube});
        for (unsigned i = 0; i < adds.size(); ++i) {
            std::cout << adds[i] << std::endl;
        }
        std::cout << "results: " << adds.size() << std::endl;
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin);
        std::cout << "took " << duration.count() << " milliseconds!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    
    
    return 0;
}