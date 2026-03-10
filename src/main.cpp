#include <iostream>
#include <chrono>
#include "../includes/adder.hpp"


int main() {
    //some test stuff for now idk
    
    try {
        auto begin = std::chrono::high_resolution_clock::now();

        Diagram cube("o3o3o *b3o");
        Diagram cube2("o3o3o4o");

        std::vector<std::vector<unsigned>> sfacets = sharedFacets(cube,cube2);
        for (unsigned i = 0; i < sfacets.size(); ++i) {
            std::cout << sfacets[i][0] << ", " << sfacets[i][1] << std::endl;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin);
        std::cout << "took " << duration.count() << " milliseconds!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    
    
    return 0;
}