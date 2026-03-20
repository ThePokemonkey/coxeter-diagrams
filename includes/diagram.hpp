#ifndef DIAGRAM_HPP
#define DIAGRAM_HPP

#include <cmath>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <utility>
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
    Diagram getInverted(unsigned node) const; //returns a new diagram obtained by doing invertNode to this one

    Space getSpace() const; //uses schlafli matrix to find the space of the diagram (up to floating point precision) (will be confused for hypercompacts)
    const AdjMat<double>& getEdges(); //displays to you the edge lengths of this diagram

    virtual void swapNodes(unsigned node1, unsigned node2) override; //swaps the nodes at the given indices, in place
    virtual void permuteNodes(std::vector<unsigned> perm) override; //reorders the nodes according to perm: node perm[i] goes to position i. invalidates references
    
    bool exactEqual(const Diagram& rhs) const; //foolishly checks direct equality on each individual label (does not solve isomorphism)
    bool isomorphic(const Diagram& rhs) const; //checks if the diagrams are isomorphic (difficult)
    //TODO: isomorphic uses my own ad-hoc algorithm that might suck. if it becomes a bottleneck (probable) get a library

    Diagram subDiagram(unsigned remnode) const; //gives the subgdiagram obtained by deleting this node
    Diagram subDiagram(const std::vector<unsigned>& nodes) const; //gives the subdiagram containing only these nodes. will produce garbage if you feed it duplicate nodes

    private:
    
    //this is the list of SQUARED edge lengths of the fundamental domain
    // edges.getEdge(i,j) is the squared edge length between the vertices opposite the facets that are dual-represented by nodes i and j
    AdjMat<double> edges;

    Eigen::MatrixXd getSchlafli() const; //creates and returns the schlafli matrix corresponding to this diagram
    Eigen::MatrixXd getStott() const; //creates and returns the stott matrix corresponding to this diagram, the inverse of the schlafli matrix
    void calcEdges(); //recalculates the squared edge lengths using matrix shenanigans

    const std::unordered_map<std::unordered_multiset<Label>,std::vector<unsigned>> getVertexSignatures() const; //gets a collection of vectors of nodes that share the same edges comin out
    const std::unordered_map<std::unordered_multiset<Label>,unsigned> countifySignatures(const std::unordered_map<std::unordered_multiset<Label>,std::vector<unsigned>>& sigs) const; //turns a getVertexSignatures result into one where each vector is replaced by its size
    //dude do you guys see these types ^^^ what the hell am i even doing

    bool subpermute(std::vector<std::vector<unsigned>>& toperm) const; //does std::next_permutation on the sub-vecs, rolling to next one if complete. returns false if rolling all the way over

    //this function returns (numerator-(denominator+1))/(numerator-2) which is 1 if unwound, 0 if maximally wound, and gives an inbetween score of windyness for inbetween values
    static double normalizedWindingScore(const Label& lab);

    //prints out the diagram in ASCII formatting
    //this function is horrifyingly large and complicated
    friend std::ostream& operator<<(std::ostream& os, const Diagram& toprint) {
        int curnode = -1; //whatever node we are on right now. -1 if we have no target
        AdjMat<bool> traversed(toprint.size(),false); //tracks which edges we've rendered so far
        std::vector<int> usednodes(toprint.size(),-1); //tracks which nodes have been rendered so far. -1 if unused, otherwise indicates the order of rendering as 0,1,2,3...
        unsigned nextnode = 0; //is the index of the next node to render. ++'s after using a node for the first time
        bool inchain = true; //when we begin a new chain we need to add a space. except for the first chain, which is why we start at true

        //main rendering loop
        do {
            

            if (curnode == -1) { //no target, begin new chain
                //if we have no target, scan through nodes to try to determine a good target:
                //a target is good if it's unused, and ties are broken based on number of non-traversed non-2 edges adjacent to it. ties from there are broken by larger axis sum (non-traversed non-2), then lower winding score sum (non-traversed non-2), then smaller node index
                //exceptions: 0 adjacents is seen as maximally bad even though it's smallest, and a winding score of 0 is seen as maximally bad even though its smallest
                //normalized winding score is a little metric i made that gives "less wound" polygons a higher score in a sensible manner. p/q has a score of (p-q-1)/(p-2). (2 is 1, ~ is 1, ~' is 0)
                //REALLY the best node is the one that will have the longest uninterrupted chain coming off it it, but that's annoying to check and this should be mostly good enough
                int newtarget = -1; //will track our current best target
                bool targetused = false; //will track if our new target is used
                unsigned adjs = 0; //will track the number of adjacent non-traversed non-2 edges
                int axissum = 0; //will track the (non-traversed non-2) sum of axes adjacent to this node
                double windsum = 0; //will track the (non-traversed non-2) sum of winding scores adjacent to this node
                //check every node
                for (unsigned checknode = 0; checknode < toprint.size(); ++checknode) {
                    bool checkused = usednodes[checknode] != -1; //is this node used?
                    if (newtarget != -1 && (!targetused && checkused)) {continue;} //if we have a target, and it's unused, and this is, move on
                    unsigned checkadjs = 0; //how many non-traversed non-2 edges does this node have?
                    double checkwsum = 0; //what's the non-traversed non-2 winding score sum of this node?
                    int checksum = 0; //what's the non-traversed non-2 axis sum of this node?
                    //loop over all adjs
                    const std::vector<Label>& adjacents = toprint.getAdjacents(checknode);
                    for (unsigned adj = 0; adj < adjacents.size(); ++adj) {
                        if (adj == checknode) {continue;} //no selfloop
                        if (traversed.getEdge(checknode,adj)) {continue;} //do not count traversed edges
                        const Label& adjlab = adjacents[adj];
                        if (adjlab == Label()) {continue;} //no 2 labels
                        //update statistics according to this non-traversed non-2 label
                        ++checkadjs;
                        checkwsum += normalizedWindingScore(adjlab); //winding score
                        checksum += adjlab.getAxis();
                    }

                    //now all the sums are calculated
                    if (checkadjs == 0 && checkused) {continue;} //no adjs and already used means no work to do, dont accept
                    //conditional logic for if this is a better choice
                    if (newtarget != -1 || !(targetused && !checkused)) {  //autoaccept if no target yet or if this is unused and target is used
                        if ((checkadjs > adjs && adjs > 0) || (checkadjs < adjs && checkadjs == 0)) {continue;} //we want less adjs, unless 0
                        if (checkadjs == adjs) { //if same adjs, tiebreak on axis
                            if (checksum < axissum) {continue;} //we want more axis
                            if (checksum == axissum) { //if same axis, tiebreak on winding
                                if (checkwsum >= windsum || (checkwsum < windsum && checkwsum == 0)) {continue;} //we want less winding score on start, unless it's zero which is bad
                                //if fully tied just reject, prioritizing lower indices
                            }
                        }
                    }
                    //update target to this newly found best
                    newtarget = checknode;
                    targetused = checkused;
                    adjs = checkadjs;
                    axissum = checksum;
                    windsum = checkwsum;
                }
                //if newtarget is -1 here it means there is no work left to do and we should break out
                if (newtarget == -1) {
                    break;
                }

                //okay after doing all that looping newtarget should be the best node to go to
                curnode = newtarget;
            } else { //if we know what node to be drawing
                //okay gang we have a node picked out

                //first let's render a space if this is the beginning of the chain
                if (!inchain) {
                    os << ' ';
                    inchain = true;
                }

                //next we should render the node itself, either o or a virtual node
                int isused = usednodes[curnode];
                if (isused != -1) {
                    //render a virtual node
                    if (isused >= 26) { //only 26 letters exist to be used as virtual nodes, turn into ash
                        throw std::runtime_error("Diagram <<: tried to make a virtual node past z!");
                    }
                    os << '*' << static_cast<char>('a'+isused);
                } else {
                    //first time rendering this node, just o, mark it used
                    os << 'o';
                    usednodes[curnode] = nextnode;
                    ++nextnode;
                }

                //third: find which edge to follow in chain (which node to visit next). must be non-traversed non-2
                //edges will be prioritized if the node at the end of them is unused, ties broken by more allowed adjacencies at the end node, then by higher axis, higher winding score, lower index
                int newtarget = -1; //will track our current best target next node
                Label targetedge; //tracks the label of the current best target edge
                bool targetused = false; //will track if our new target is used
                unsigned targetadjs = 0; //will track the number of adjacent non-traversed non-2 edges to the next node
                double wind = 0; //will track the winding score of new target edge
                int axis = 0; //will track the axis of new target edge
                //loop over all adjacent nodes to go to and check conditions to find the best one
                const std::vector<Label>& curadjs = toprint.getAdjacents(curnode);
                for (unsigned adj = 0; adj < curadjs.size(); ++adj) {
                    if (adj == curnode) {continue;} //no selfloops
                    if (traversed.getEdge(curnode,adj)) {continue;} //no traversed edges
                    const Label& adjlab = curadjs[adj];
                    if (adjlab == Label()) {continue;} //do not count 2's

                    bool adjused = usednodes[adj] != -1; //will track if this next node has been used
                    if (newtarget != -1 && adjused && !targetused) {continue;} //if we have an unused connection itll always be better than a used one
                    unsigned adjadjcount = 0; //will track allowed adjacencies of this next node
                    double adjwind = 0; //will track the winding score of this next node
                    int adjaxis = 0; //will track the axis of this next node

                    //now we need to collect some statistics on this potential next target. first lets count adjs
                    const std::vector<Label>& adjadjs = toprint.getAdjacents(adj);
                    for (unsigned adjadj = 0; adjadj < adjadjs.size(); ++adjadj) {
                        if (adjadj == adj || adjadj == curnode) {continue;} //no selfloops or backtracking
                        const Label& adjadjlab = toprint.getEdge(adj,adjadj);
                        if (adjadjlab != Label() && !traversed.getEdge(adj,adjadj)) {
                            ++adjadjcount;
                        }
                    }
                    //now the winding score and axis
                    adjaxis = adjlab.getAxis();
                    adjwind = normalizedWindingScore(adjlab);
                    //check all of our conditions
                    if (newtarget != -1 || !(targetused && !adjused)) { //autoaccept if no target yet or if this is unused and target is used
                        if (adjadjcount < targetadjs) {continue;} //we want more adjs
                        if (adjadjcount == targetadjs) { //if same adjs tiebreak on axis
                            if (adjaxis < axis) {continue;} //we want higher axis
                            if (adjaxis == axis) { //if same axis tiebreak on winding
                                if (adjwind <= wind) {continue;} //we want more winding score
                                //if fully tied reject, first come first serve, lesser index wins
                            }
                        }
                    }
                    //update target to this newly found best
                    newtarget = adj;
                    targetedge = adjlab;
                    targetused = adjused;
                    targetadjs = adjadjcount;
                    wind = adjwind;
                    axis = adjaxis;
                }

                //okay NOW newtarget should be the next node we go to, or -1 if there's nowhere useful to go
                if (newtarget != -1) {
                    //render the edge to this next node
                    os << targetedge;
                    //mark this edge as traversed
                    traversed.setEdge(curnode,newtarget,true);
                } else {
                    //oh nowhere to go okay. let us know we're about to start a new chain
                    inchain = false;
                }
                curnode = newtarget; //either way this is correct
            }


        } while (true); //we break from this loop manually
        
        //okay we finished traversing lets go we should have printed everything
        return os;

    }


};


#endif //DIAGRAM_HPP