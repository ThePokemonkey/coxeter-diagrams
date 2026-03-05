#include <iostream>
#include "../includes/diagram.hpp"


int main() {
    //some test stuff for now idk
    
    try {
        Diagram cube("o3o5o");
        
        //std::cout << cube << std::endl;
        //cube.invertNode(2);
        //std::cout << cube << std::endl;

        std::cout << cube << std::endl;
        std::cout << cube.size() << std::endl;
        cube.permuteNodes({2,0,1});
        std::cout << cube << std::endl;
        std::cout << cube.getSpace() << std::endl;
        std::cout << cube.getEdges() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    
    
    return 0;
}