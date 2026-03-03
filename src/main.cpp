#include <iostream>
#include "../includes/diagram.hpp"


int main() {
    //some test stuff for now idk
    
    try {
        Diagram cube("o4o3o3o3o3o3o3o3o");
        //std::cout << cube << std::endl;
        //cube.invertNode(2);
        //std::cout << cube << std::endl;

        std::cout << cube.size() << std::endl;
        std::cout << cube.getSpace() << std::endl;
        std::cout << cube.getEdges() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    
    
    return 0;
}