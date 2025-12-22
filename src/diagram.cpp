#include "..\includes\diagram.hpp"

Diagram::Diagram(unsigned int rank) {
    //set our rank
    rank_ = rank;
    //fill out the diagram with 2's
    Init();
}

Diagram::Diagram(std::string ascii) {
    //probably quite a pain
}

void Diagram::Init() {
    //wipe the matrix
    matrix_ = {};
    //fill out the diagram with 2's
    for (unsigned int i = 0; i < rank_; ++i) {
        std::vector<Label> row;
        for (unsigned int j = 0; j < rank_; ++j) {
            Label label;
            row.push_back(label);
        }
        matrix_.push_back(row);
    }
}

void Diagram::SetLabel(unsigned int v1, unsigned int v2, Label label) {
    if (v1 > rank_ || v2 > rank_) {
        throw std::out_of_range("Diagram SetLabel: this diagram doesn't have that many nodes!");
    }

    //i love storing all the information twice
    matrix_[v1][v2] = label;
    matrix_[v2][v1] = label;
}