#include "../includes/diagram.hpp"


Diagram::Diagram(unsigned nodes) : AdjMat<Label>(nodes) {}

Diagram::Diagram(unsigned nodes, const Label& value) : AdjMat<Label>(nodes, value) {}