#ifndef DIAGRAM_HPP
#define DIAGRAM_HPP

#include <cmath>
#include <numbers>
#include "../includes/label.hpp"
#include "../includes/adjmat.hpp"
#include "../includes/Eigen/Dense"

//used to give the space a diagram lies in
enum class Space {
    Spherical,
    Euclidean,
    Hyperbolic,
};
//used to print spaces
std::ostream& operator<<(std::ostream& os, Space space);

class Diagram : public AdjMat<Label> {
    public:

    Diagram() = default; //makes a totally empty diagram
    Diagram(unsigned nodes); //makes a diagram with n nodes with 2 on every edge
    Diagram(unsigned nodes, const Label& value); //makes a graph with n nodes with a given initial label on every edge
    Diagram(const std::string&); //makes a cd from its ascii representation. only supports o nodes

    void invertNode(unsigned node); //retrogrades all the labels surrounding this node

    Space getSpace() const; //uses schlafli matrix to find the space of the diagram (up to floating point precision) (will be confused for hypercompacts)

    const AdjMat<double>& getEdges(); //displays to you the edge lengths of this diagram

    virtual void swapNodes(unsigned node1, unsigned node2) override; //swaps the nodes at the given indices, in place
    virtual void permuteNodes(std::vector<unsigned> perm) override; //reorders the nodes according to perm: node perm[i] goes to position i. invalidates references

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

    Eigen::MatrixXd getSchlafli() const; //creates and returns the schlafli matrix corresponding to this diagram
    Eigen::MatrixXd getStott() const; //creates and returns the stott matrix corresponding to this diagram, the inverse of the schlafli matrix
    
    //this is the list of SQUARED edge lengths of the fundamental domain
    // edges.getEdge(i,j) is the squared edge length between the vertices opposite the facets that are dual-represented by nodes i and j
    AdjMat<double> edges;

    void calcEdges(); //recalculates the squared edge lengths using matrix shenanigans

};


#endif //DIAGRAM_HPP