#ifndef ADJMAT_HPP
#define ADJMAT_HPP

#include <vector>
#include <stdexcept>
#include <iostream>

//TODO: this matrix stores edges from a node to itself. it really shouldn't do that. maybe it's fine?
//IDEA: make the matrix somehow triangular so i store no reduntant information, without reducing functionality (would this make things much much slower?)


template <typename T>
class AdjMat { //adjacency matrix, for storing labelled complete graphs. includes self-loops (watch out)

    public:

    AdjMat() = default; //makes a totally empty graph
    AdjMat(unsigned nodes); //makes a graph with n nodes with default constructed value on every edge
    AdjMat(unsigned nodes, const T& value); //makes a graph with n nodes with a given initial value on every edge

    unsigned size() const; //returns the number of nodes
    bool empty() const; //returns true if the graph has 0 nodes

    void addNode(); //adds a node with default constructed links to every other node
    void addNode(const T& value); //adds a node with a given initial value as links to every other node
    void removeNode(unsigned index); //removes the node of given index, shifting later indices down

    void resize(unsigned nodes); //adds or removes nodes until there is n nodes in the graph. default constructed value on all new edges
    void resize(unsigned nodes, const T& value); //adds or removes nodes until there is n nodes in the graph. given value on all new edges

    const T& getEdge(unsigned node1, unsigned node2) const; //returns the edge (as const reference!) between the two nodes given by index
    T& getEdge(unsigned node1, unsigned node2); //returns the edge (as mut reference!) between the two nodes given by index
    void setEdge(unsigned node1, unsigned node2, const T& value); //sets the edge between the two nodes given by index to the value

    const std::vector<T>& getAdjacents(unsigned node) const; //returns a const& vector of the edges between the given node and all other node indices
    const std::vector<T>& operator[](unsigned node) const; //returns a const& of the edges between the given node and all other nodes. allows for [x][y] syntax to access an edge. NOT MUTABLE

    private:

     std::vector<std::vector<T>> matrix_;

     template <typename T> //this is in the class for some reason
     friend std::ostream& operator<<(std::ostream& os, const AdjMat<T>& toprint) {

        os << "AdjMat { " ;

        for (const std::vector<T>& row : toprint.matrix_) {
            os << std::endl;
            for (const T& item : row) {
                os << item << ", ";
            }
        }
        
        os << std::endl << "}";

        return os;
     }

};

//define functions where they are declared for templates, so John Link doesn't get confused

template <typename T>
AdjMat<T>::AdjMat(unsigned nodes) {
    matrix_.resize(nodes,std::vector<T>(nodes));
}

template <typename T>
AdjMat<T>::AdjMat(unsigned nodes, const T& value) {
    matrix_.resize(nodes,std::vector<T>(nodes,value));
}

template <typename T>
unsigned AdjMat<T>::size() const {
    return matrix_.size();
}

template <typename T>
bool AdjMat<T>::empty() const {
    return matrix_.empty();
}

template <typename T>
void AdjMat<T>::addNode() {
    for (std::vector<T>& row : matrix_) {
        row.push_back(T{});
    }
    matrix_.push_back(std::vector<T>(size()+1,T{}));
}

template <typename T>
void AdjMat<T>::addNode(const T& value) {
    for (std::vector<T>& row : matrix_) {
        row.push_back(value);
    }
    matrix_.push_back(std::vector<T>(size()+1,value));
}

template <typename T>
void AdjMat<T>::removeNode(unsigned index) {
    if (index >= size()) {
        throw std::out_of_range("AdjMat Remove Node: tried to remove a node at a nonexistent index!");
    }

    matrix_.erase(matrix_.begin()+index);
    for (std::vector<T>& row : matrix_) {
        row.erase(row.begin()+index);
    }
}

template <typename T>
void AdjMat<T>::resize(unsigned nodes) {
    for (std::vector<T>& row : matrix_) {
        row.resize(nodes,T{});
    }
    matrix_.resize(nodes,std::vector<T>(nodes,T{}));
}

template <typename T>
void AdjMat<T>::resize(unsigned nodes, const T& value) {
    for (std::vector<T>& row : matrix_) {
        row.resize(nodes,value);
    }
    matrix_.resize(nodes,std::vector<T>(nodes,value));
}

template <typename T>
const T& AdjMat<T>::getEdge(unsigned node1, unsigned node2) const {
    if (node1 >= size() || node2 >= size()) {
        throw std::out_of_range("AdjMat Get Edge: tried to get an edge at a nonexistent node index!");
    }

    return matrix_[node1][node2];
}

template <typename T>
T& AdjMat<T>::getEdge(unsigned node1, unsigned node2) {
    if (node1 >= size() || node2 >= size()) {
        throw std::out_of_range("AdjMat Get Edge: tried to get an edge at a nonexistent node index!");
    }

    return matrix_[node1][node2];
}

template <typename T>
void AdjMat<T>::setEdge(unsigned node1, unsigned node2, const T& value) {
    if (node1 >= size() || node2 >= size()) {
        throw std::out_of_range("AdjMat Set Edge: tried to set an edge at a nonexistent node index!");
    }

    matrix_[node1][node2] = value;
    matrix_[node2][node1] = value;
}

template <typename T>
const std::vector<T>& AdjMat<T>::getAdjacents(unsigned node) const {
    if (node >= size()) {
        throw std::out_of_range("AdjMat Get Adjacents: tried to get adjacents of a nonexistent node index!");
    }

    return matrix_[node];
}

template <typename T>
const std::vector<T>& AdjMat<T>::operator[](unsigned node) const {
    return getAdjacents(node);
}




#endif //ADJMAT_HPP