#include "../includes/diagram.hpp"


Diagram::Diagram(unsigned nodes) : AdjMat<Label>(nodes) {}

Diagram::Diagram(unsigned nodes, const Label& value) : AdjMat<Label>(nodes, value) {}

//super ouper long function :(
Diagram::Diagram(const std::string& ascii) : AdjMat<Label>() {
    //state starts out with no nodes
    //TODO: it would maybe be faster (less reallocation) if we counted o's beforehand and created all nodes beforehand?

    //SUPER TODO: does not yet support infinity nodes (with ~)

    AdjMat<bool> connected; //keeps track of what nodes have already been linked (no double link allowed)

    int curnode = -1; //current node index we're on. -1 initially because there aren't any nodes yet
    bool inedge = false; //if we're trying to parse an edge label
    bool denning = false; //if in an edge, this means we're doing the denominator now
    bool virtualing = false; //means we're looking for a lowercase leter to refer to a virtual node
    std::string curnum; //current numerator of edge
    std::string curden; // current denominator of edge

    for (unsigned i = 0; i < ascii.size(); ++i) {
        const char& ch = ascii[i];

        switch (ch)
        {
        case ' ': //spacer outside of edges. if encountered it means we're not actually making an edge
            inedge = false;
            denning = false;
            if (!curnum.empty()) {
                throw std::invalid_argument("Diagram Parsing: edge ended too early! (via space)");
            } else if (virtualing) {
                throw std::invalid_argument("Diagram Parsing: virtual node expected, got space!");
            }
            continue;
        //TODO: hyphen support? seems like it may be a pain
        case '/': //fraction time
            if (inedge) {
                if (denning) {
                    //cant denominator if already denominator
                    throw std::invalid_argument("Diagram Parsing: too many slashes in edge!");
                } else if (curnum.empty()) {
                    //cant denominator if our numerator is empty
                    throw std::invalid_argument("Diagram Parsing: empty edge numerator!");
                } else {
                    denning = true;
                }
            } else {
                //slashes should only be in edge
                throw std::invalid_argument("Diagram Parsing: slash not in edge!");
            }
            continue;
        case '*': //virtual node
            if (virtualing) {
                throw std::invalid_argument("Diagram Parsing: virtual node expected, got asterisk!");
            }
            virtualing = true; //virtual node completion procedures are in the letters case
            continue;
        case 'o': //define node
            if (!virtualing) {
                //add a node
                addNode();
                connected.addNode();
                if (inedge) { //we need to actually draw an edge
                    //check if number is reasonable, craft numerator and denominator
                    if (curnum.empty() || (denning && curden.empty())) {
                        throw std::invalid_argument("Diagram Parsing: edge ended too early! (via node)");
                    }
                    const int num = std::stoi(curnum);
                    int den = 1;
                    if (denning) {
                        den = std::stoi(curden);
                    }

                    //set the edge to the appropriate label
                    setEdge(curnode,size()-1,Label(num,den));
                    //set connected to true at this connection
                    connected.setEdge(curnode,size()-1,true);

                    //turn variables back to normal
                    denning = false; //but we are still inedge until a space cancels it
                    curnum.clear();
                    curden.clear();
                } else { //first node in an edge streak
                    inedge = true; //doesn't fully commit to an edge until it encounters a number, though
                }
                //properly set curnode to the new node we just made
                curnode = size()-1;
                continue;
            }
            //if we are virtualing, we need to fallthrough to default case, to support *o virtual node
            [[fallthrough]];
        default:
            //elseif chain for large classes of characters

            if (ch >= '0' && ch <= '9') { //digit
                if (inedge) {
                    //add on to whatever part of the edge we're doing
                    if (denning) {
                        curden += ch;
                    } else {
                        curnum += ch;
                    }
                } else {
                    //numbers should only be in edge
                    throw std::invalid_argument("Diagram Parsing: number not in edge!");
                }
                continue;
            } else if (ch >= 'a' && ch <= 'z') { //virtual nodes
                if (virtualing) {
                    //determine which node we're talking about here
                    const unsigned refnode = ch - 'a';
                    if (refnode >= size()) {
                        throw std::invalid_argument("Diagram Parsing: virtual node referring to nonexistent node!");
                    }
                    //actually connect
                    if (inedge) {
                        //do NOT make a self-connection
                        if (refnode == curnode) {
                            throw std::invalid_argument("Diagram Parsing: tried to connect node to itself!");
                        }
                        //do NOT make a connection where one has already been made
                        if (connected.getEdge(curnode,refnode)) {
                            throw std::invalid_argument("Diagram Parsing: tried to connect an edge multiple times!");
                        }

                        //check if number is reasonable, craft numerator and denominator
                        if (curnum.empty() || (denning && curden.empty())) {
                            throw std::invalid_argument("Diagram Parsing: edge ended too early! (via virtual node)");
                        }
                        const int num = std::stoi(curnum);
                        int den = 1;
                        if (denning) {
                            den = std::stoi(curden);
                        }

                        //set the edge to the appropriate label
                        setEdge(curnode,refnode,Label(num,den));
                        //set connected to true at this connection
                        connected.setEdge(curnode,refnode,true);

                        //turn variables back to normal
                        denning = false; //but we are still inedge until a space cancels it
                        curnum.clear();
                        curden.clear();
                    } else { //first node in chain (rare situation but useful)
                        inedge = true;
                    }
                    curnode = refnode; //this happens whether or not we're in an edge
                    virtualing = false;
                } else {
                    throw std::invalid_argument("Diagram Parsing: random "+std::string(1,ch)+" outside of a virtual node!");
                }
                continue;
            } else { //not caught by any other situash
                throw std::invalid_argument("Diagram Parsing: unexpected character "+std::string(1,ch)+"!");
            }

            break;
        }


    }

}