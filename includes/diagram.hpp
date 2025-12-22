#ifndef DIAGRAM_HPP
#define DIAGRAM_HPP

#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "..\includes\label.hpp"

class Diagram { //a coxeter diagram without "annotations": just stores the edge labels, no ringings or edge lengths or other bonus info
    public:
    Diagram(unsigned int rank); //creates an all 2's diagram of the given rank
    Diagram(std::string ascii); //parses an ascii diagram into a Diagram


    private:

    unsigned int rank_; //how many nodes are in the diagram, the sqrt of the amount of Labels in matrix_
    std::vector<std::vector<Label>> matrix_; //the meat and potatoes of the diagram: an adjacency matrix of Labels between the nodes
    
    void Init(); //turns the diagram into one of all 2's (maintains rank)
    void SetLabel(unsigned int v1, unsigned int v2, Label label); //sets the edge between these two vertices to be this label
};

#endif