#ifndef ADJMAT_HPP
#define ADJMAT_HPP

#include <vector>
#include <stdexcept>
#include <iostream>

//this matrix stores edges from a node to itself. maybe it's fine? they'll be garbage though
//IDEA: make the matrix somehow triangular so i store no reduntant information, without reducing functionality (would this make things much much slower?)
//IDEA 2: currently there's no nice way to modify elements in place using their own functions. maybe some reference subclass would be nice

template <typename T>
class AdjMat { //adjacency matrix, for storing labelled complete graphs. no self-loops

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
    void clear(); //deletes all the nodes

    //vector<bool> specialization hellscape
    typename std::vector<T>::const_reference getEdge(unsigned node1, unsigned node2) const; //returns the edge between the two nodes given by index. errors if self-loop
    void setEdge(unsigned node1, unsigned node2, const T& value); //sets the edge between the two nodes given by index to the value. errors if self-loop

    const std::vector<T>& getAdjacents(unsigned node) const; //returns a const& vector of the edges between the given node and all other node indices. garbage data in the (n,n) position

    void swapNodes(unsigned node1, unsigned node2); //swaps the nodes at the given indices, in place
    //TODO: void permuteNodes(std::vector<unsigned> perm); //reorders the nodes according to perm: node perm[i] goes to position i.

    private:

     std::vector<std::vector<T>> matrix_;

     //prints the adjmat out mediocrely
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
void AdjMat<T>::clear() {
    matrix_.clear();
}

template <typename T>
typename std::vector<T>::const_reference AdjMat<T>::getEdge(unsigned node1, unsigned node2) const {
    if (node1 >= size() || node2 >= size()) {
        throw std::out_of_range("AdjMat Get Edge: tried to get an edge at a nonexistent node index!");
    } else if (node1 == node2) {
        throw std::invalid_argument("AdjMat Get Edge: tried to get a selfloop!");
    }

    return matrix_[node1][node2];
}

template <typename T>
void AdjMat<T>::setEdge(unsigned node1, unsigned node2, const T& value) {
    if (node1 >= size() || node2 >= size()) {
        throw std::out_of_range("AdjMat Set Edge: tried to set an edge at a nonexistent node index!");
    } else if (node1 == node2) {
        throw std::invalid_argument("AdjMat Set Edge: tried to set a selfloop!");
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
void AdjMat<T>::swapNodes(unsigned node1, unsigned node2) {
    if (node1 == node2) {return;}
    for (unsigned i = 1; i < size(); ++i) {
        if (i == node1 || i == node2) {continue;}
        std::swap(matrix_[node1][i],matrix_[node2][i]);
        std::swap(matrix_[i][node1],matrix_[i][node2]);
    }
}


#endif //ADJMAT_HPP