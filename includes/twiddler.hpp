#ifndef TWIDDLER_HPP
#define TWIDDLER_HPP

#include "../includes/diagram.hpp"
#include <unordered_map>
#include "../includes/adder.hpp" //literally just for isinlist. i should make a util file at some point shouldnt i

//returns a new diagram obtained by rotating the target mirror so its with the respective mirror is num/newden
//set noreduce to true to not intentionally reduce the rotated angle (usually undesirable)
Diagram twiddleEdge(const Diagram& base, unsigned target, unsigned respective, unsigned newden); 

//given a list of starter diagrams, repeatedly tries all angle twiddlings until no new diagrams can be found
//returns a giant list of the results.
std::vector<Diagram> recursiveTwiddler(const std::vector<Diagram>& initials);

//uses twiddletech to split a diagram into its addends at the angle specified between the two nodes (order irrelevant)
//returns as many diagrams as the denominator of the label. will explode if you pass in ~'
std::vector<Diagram> splitDiagram(const Diagram& tosplit, unsigned node1, unsigned node2);








#endif //TWIDDLER_HPP