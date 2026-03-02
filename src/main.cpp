#include <iostream>
#include "../includes/diagram.hpp"
#include "../includes/Eigen/Dense"

int main() {
    //some test stuff for now idk
    
    try {
        Diagram cube("o3o4o");
        std::cout << cube << std::endl;
        cube.invertNode(2);
        std::cout << cube << std::endl;

        Eigen::Matrix<double,2,2> m;
        m(0,0) = 5;
        m(0,1) = 6;
        m(1,0) = 4;
        m(1,1) = 3;

        std::cout << m << std::endl;

    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    
    
    return 0;
}