#ifndef DOUBLER_HPP
#define DOUBLER_HPP

#include "../includes/diagram.hpp"
#include "../includes/adder.hpp" //literally just for isinlist. i should make a util file at some point shouldnt i


Diagram doubleEdge(const Diagram& base, unsigned target, unsigned respective); //returns a new diagram obtained by rotating the target mirror so it has twice the angle with the respective mirror
//props to planet9ine for coming up with this angle doubling strategy its awesome

//given a list of starter diagrams, repeatedly tries all doublings and node flips until no new diagrams can be found
//returns a giant list of the results.
std::vector<Diagram> recursiveDoubler(const std::vector<Diagram>& initials);








#endif //DOUBLER_HPP