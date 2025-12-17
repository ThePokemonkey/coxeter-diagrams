#include <iostream>
#include "../includes/label.hpp"

int main() {
    //some test labels for now idk
    Label def;
    Label inf(false);
    Label rinf(true);
    Label sq(4);
    Label rsq(4,3);
    Label dydy(4,2);
    std::cout << def.GetOrder() << std::endl;
    std::cout << inf.GetOrder()  << std::endl;
    std::cout << rinf.GetOrder()  << std::endl;
    std::cout << sq.GetOrder() << std::endl;
    std::cout << rsq.GetOrder()  << std::endl;
    std::cout << dydy.GetOrder()  << std::endl;
    return 0;
}