#ifndef DIAGRAM_HPP
#define DIAGRAM_HPP

#include "../includes/label.hpp"
#include "../includes/adjmat.hpp"

class Diagram : AdjMat<Label> {
    public:

    Diagram() = default; //makes a totally empty diagram
    Diagram(unsigned nodes); //makes a diagram with n nodes with 2 on every edge
    Diagram(unsigned nodes, const Label& value); //makes a graph with n nodes with a given initial label on every edge
    Diagram(const std::string&); //makes a cd from its ascii representation. only supports o nodes

    private:
    
    //eventually this will be replaced with a more reasonable function that prints an ASCII diagram
     friend std::ostream& operator<<(std::ostream& os, const Diagram& toprint) { //prints the diagram as a matrix

        os << "Diagram { " ;

        for (unsigned i = 0; i < toprint.size(); ++i) {
            os << std::endl;
            for (const Label& edge : toprint.getAdjacents(i)) {
                os << edge << ", ";
            }
        }
        
        os << std::endl << "}";

        return os;
     }

    
};


#endif //DIAGRAM_HPP