#include <iostream>
#include "../includes/diagram.hpp"

int main() {
    //some test stuff for now idk
    
    try {
        std::cout << Diagram("o3o4o") << std::endl;
        std::cout << Diagram("o3o4o5o6o") << std::endl;
        std::cout << Diagram("o o o o3o") << std::endl;
        std::cout << Diagram("o3o3o3*a") << std::endl;
        std::cout << Diagram("o3o3o3o *b3o") << std::endl;
        std::cout << Diagram("o5/2o3o7/6o *b5/3o") << std::endl;
        std::cout << Diagram("o5o3o5/2o3*a5/4*c  *b3*d") << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    
    
    return 0;
}