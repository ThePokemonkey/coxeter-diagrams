#ifndef TWIDDLER_HPP
#define TWIDDLER_HPP

#include "../includes/diagram.hpp"
#include "../includes/adder.hpp" //literally just for isinlist. i should make a util file at some point shouldnt i


Diagram twiddleEdge(const Diagram& base, unsigned target, unsigned respective, unsigned newden); //returns a new diagram obtained by rotating the target mirror so its with the respective mirror is num/newden

//given a list of starter diagrams, repeatedly tries all angle twiddlings until no new diagrams can be found
//returns a giant list of the results.
std::vector<Diagram> recursiveTwiddler(const std::vector<Diagram>& initials);








#endif //TWIDDLER_HPP