#include <iostream>
#include "../includes/label.hpp"
#include "../includes/adjmat.hpp"

int main() {
    //some test labels for now idk
    AdjMat<int> coolmat(2);
    coolmat.setEdge(0,1,1);
    std::cout << coolmat[0][1] << std::endl;
    std::cout << coolmat[1][1] << std::endl;
    std::cout << coolmat << std::endl;
    return 0;
}