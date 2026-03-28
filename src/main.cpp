#include <iostream>
#include <chrono>
#include "../includes/twiddler.hpp"

int main() {
    try {
        auto begin = std::chrono::high_resolution_clock::now();

        Diagram test("o5o3o3o");

        std::vector<Diagram> results = recursiveTwiddler({test});
        for (unsigned i = 0; i < results.size(); ++i) {
            std::cout << results[i] << std::endl;
        }
        std::cout << "got " << results.size() << " diagrams!" << std::endl;
        

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin);
        std::cout << "took " << duration.count() << " milliseconds!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    return 0;
}