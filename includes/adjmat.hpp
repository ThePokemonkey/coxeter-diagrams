#ifndef ADJMAT_HPP
#define ADJMAT_HPP

#include <vector>
#include <stdexcept>
#include <iostream>
#include <algorithm>

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

    //these are virtual because Diagrams need to swap other data other than just their labels, so i need to do a little bit of polymorphism
    virtual void swapNodes(unsigned node1, unsigned node2); //swaps the nodes at the given indices, in place
    virtual void permuteNodes(std::vector<unsigned> perm); //reorders the nodes according to perm: node perm[i] goes to position i. invalidates references
    
    //calling these functions in derived classes is like mega weird you shouldnt do it
    AdjMat<T> subgraph(unsigned remnode) const; //gives the subgraph obtained by deleting this node
    AdjMat<T> subgraph(const std::vector<unsigned>& nodes) const; //gives the subgraph containing only these nodes. will produce garbage if you feed it duplicate nodes
    
    protected:

    std::vector<std::vector<T>> subMat(unsigned remnode) const; //gives the subgraph obtained by deleting this node, in pure matrix
    std::vector<std::vector<T>> subMat(const std::vector<unsigned>& nodes) const; //gives the subgraph containing only these nodes, in pure matrix. will produce garbage if you feed it duplicate nodes
    void setMat(const std::vector<std::vector<T>>& matrix); //sets matrix_ to the input matrix. to only be used when better methods are not available

    private:

     std::vector<std::vector<T>> matrix_; //holds the edges

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
    for (unsigned i = 0; i < size(); ++i) {
        if (i == node1 || i == node2) {continue;}
        // vector<bool> says i am not allowed to std::swap() these... why...... why........
        T temp = matrix_[node1][i];
        matrix_[node1][i] = matrix_[node2][i];
        matrix_[node2][i] = temp;
        
        temp = matrix_[i][node1];
        matrix_[i][node1] = matrix_[i][node2];
        matrix_[i][node2] = temp;
    }
}

template <typename T>
void AdjMat<T>::permuteNodes(std::vector<unsigned> perm) {
    if (perm.size() != size()) {
        throw std::invalid_argument("AdjMat Permute Nodes: this permutation is not the proper size!");
    }
    if (std::unique(perm.begin(),perm.end()) != perm.end()) {
        throw std::invalid_argument("AdjMat Permute Nodes: this permutation contains duplicates!");
    }
    std::vector<std::vector<T>> newmat;
    newmat.resize(size(),std::vector<T>(size())); //not doing this in place, that's a headache
    for (unsigned i = 0; i < size(); ++i) {
        for (unsigned j = 0; j < size(); ++j) {
            unsigned nodi = perm[i];
            unsigned nodj = perm[j];
            if (nodi >= size() || nodj >= size()) {
                throw std::invalid_argument("AdjMat Permute Nodes: this permutation contains nonexistent nodes");
            }
            newmat[i][j] = matrix_[nodi][nodj];
        }
    }
    matrix_ = newmat;
}

template <typename T>
std::vector<std::vector<T>> AdjMat<T>::subMat(unsigned remnode) const {
    if (empty()) {
        throw std::out_of_range("AdjMat Submatrix: this diagram is empty!");
    }
    if (remnode >= size()) {
        throw std::out_of_range("AdjMat Submatrix: there aren't this many nodes here!");
    }
    std::vector<std::vector<T>> resmat;
    unsigned newsize = size()-1;
    resmat.resize(newsize,std::vector<T>(newsize));
    for (unsigned i = 0; i < newsize; ++i) {
        unsigned targeti = i;
        if (i >= remnode) {
            ++targeti;
        }
        for (unsigned j = i; j < newsize; ++j) {
            unsigned targetj = j;
            if (j >= remnode) {
                ++targetj;
            }
            resmat[i][j] = matrix_[targeti][targetj];
            resmat[j][i] = matrix_[targeti][targetj];
        }
    }
    return resmat;
}

template <typename T>
std::vector<std::vector<T>> AdjMat<T>::subMat(const std::vector<unsigned>& nodes) const {
    //this function will NOT be checking if the nodes vector has duplicates. try not to feed it utter garbage
    if (empty()) {
        throw std::out_of_range("AdjMat Submatrix: this diagram is empty!");
    }
    if (nodes.size() > size()) {
        throw std::invalid_argument("AdjMat Submatrix: too many nodes provided!");
    }
    std::vector<std::vector<T>> resmat;
    resmat.resize(nodes.size(),std::vector<T>(nodes.size()));
    for (unsigned i = 0; i < nodes.size(); ++i) {
        unsigned targeti = nodes[i];
        for (unsigned j = i; j < nodes.size(); ++j) {
            unsigned targetj = nodes[j];
            if (targetj >= size()) {
                throw std::out_of_range("AdjMat Submatrix: there aren't this many nodes here!");
            }
            if (i == j) {continue;} //selfloops are garbage, but we still needed to check for out of range here
            resmat[i][j] = matrix_[targeti][targetj];
            resmat[j][i] = matrix_[targeti][targetj];
        }

    }
    return resmat;
}

template <typename T>
void AdjMat<T>::setMat(const std::vector<std::vector<T>>& matrix) {
    matrix_ = matrix;
}

template <typename T>
AdjMat<T> AdjMat<T>::subgraph(unsigned remnode) const {
    AdjMat<T> res;
    res.setMat(subMat(remnode));
    return res;
}

template <typename T>
AdjMat<T> AdjMat<T>::subgraph(const std::vector<unsigned>& nodes) const {
    AdjMat<T> res;
    res.setMat(subMat(nodes));
    return res;
}


#endif //ADJMAT_HPP