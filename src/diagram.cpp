#include "../includes/diagram.hpp"


std::ostream& operator<<(std::ostream& os, Space space) {
    switch(space) {
        case Space::Spherical:
            os << "Spherical";
            break;
        case Space::Euclidean:
            os << "Euclidean";
            break;
        case Space::Hyperbolic:
            os << "Hyperbolic";
            break;
    }
    return os;
}

Diagram::Diagram(unsigned nodes) : AdjMat<Label>(nodes) {}

Diagram::Diagram(unsigned nodes, const Label& value) : AdjMat<Label>(nodes, value) {}

//super ouper long function :(
Diagram::Diagram(const std::string& ascii) : AdjMat<Label>() {
    //state starts out with no nodes

    AdjMat<bool> connected; //keeps track of what nodes have already been linked (no double link allowed)

    int curnode = -1; //current node index we're on. -1 initially because there aren't any nodes yet
    bool inedge = false; //if we're trying to parse an edge label
    bool denning = false; //if in an edge, this means we're doing the denominator now
    bool virtualing = false; //means we're looking for a lowercase leter to refer to a virtual node
    bool infiniting = false; //means we are in special infinity edge parser state
    bool infinitretro = false; //means we have a retrograde infinity edge cooking up
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
        case '~': //infinity edge awesome case
            if (virtualing) {
                throw std::invalid_argument("Diagram Parsing: virtual node expected, got tilde!");
            }
            if (!inedge) {
                throw std::invalid_argument("Diagram Parsing: tilde outside of an edge!");
            }
            if (!curnum.empty()) {
                throw std::invalid_argument("Diagram Parsing: tilde within number!");
            }
            infiniting = true;
            continue;
        case '\'':
            if (!infiniting) {
                throw std::invalid_argument("Diagram Parsing: apostrophe not in infinity!");
            }
            if (virtualing) {
                throw std::invalid_argument("Diagram Parsing: virtual node expected, got tilde!");
            }
            infinitretro = true;
            continue;
        case 'o': //define node
            if (!virtualing) {
                //add a node
                addNode();
                connected.addNode();
                if (inedge) { //we need to actually draw an edge
                    if (!infiniting) {
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
                    } else {
                        //infinity edge funny
                        setEdge(curnode,size()-1,Label(infinitretro));
                        infiniting = false;
                        infinitretro = false;
                    }

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
                    if (infiniting) {
                        throw std::invalid_argument("Diagram Parsing: number in infinity!");
                    }

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

                        if (!infiniting) {
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
                        } else {
                            //infinity funny
                            setEdge(curnode,refnode,Label(infinitretro));
                            infiniting = false;
                            infinitretro = false;
                        }
                        
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

void Diagram::invertNode(unsigned node) {
    for (unsigned i = 0; i < size(); ++i) {
        if (i == node) {continue;} //no self loop
        setEdge(node,i,getEdge(node,i).GetRetrograde());
    }
}

Eigen::MatrixXd Diagram::getSchlafli() const {
    //create matrix
    Eigen::MatrixXd schlafli(size(),size());
    for (unsigned i = 0; i < size(); ++i) {
        for (unsigned j = i; j < size(); ++j) {
            if (i == j) {
                //self loop positions are 1
                schlafli(i,j) = 1;
            } else {
                //else, -cos(pi/label)
                Label thislabel = getEdge(i,j);
                if (thislabel.IsInfinity()) {
                    if (thislabel.IsRetrograde()) {
                        //retrograde infinity has a cos of -1. dont think about it
                        schlafli(i,j) = 1;
                        schlafli(j,i) = 1;
                    } else {
                        //normal infinity gives cos(0) = 1
                        schlafli(i,j) = -1;
                        schlafli(j,i) = -1;
                    }
                } else {
                    //normal freaking value
                    schlafli(i,j) = -std::cos(std::numbers::pi/(getEdge(i,j).GetValue()));
                    schlafli(j,i) = schlafli(i,j);
                }
                
            }
        }
    }
    //return it!!
    return schlafli;
}

Eigen::MatrixXd Diagram::getStott() const {
    return getSchlafli().inverse();
}

constexpr double epsilon = 1e-12;

Space Diagram::getSpace() const {
    //find schlafli
    Eigen::MatrixXd schlafli = getSchlafli();
    //determine
    double det = schlafli.determinant();
    if (abs(det) < epsilon) { //probably euclidean
        return Space::Euclidean;
    } else if (det > 0) {
        return Space::Spherical;
    } else {
        return Space::Hyperbolic;
    }
}

void Diagram::calcEdges() {
    //for those just joining the stream calc is slang for calculator
    //set up edges matrix
    edges.clear();
    edges.resize(size());
    //TODO: will EXPLODE if you put a euclidean diagram in here
    //get stott matrix
    Eigen::MatrixXd stott = getStott();
    //fill out squared edges
    for (unsigned i = 0; i < size(); ++i) {
        double sii = stott(i,i);
        for (unsigned j = 0; j < size(); ++j) {
            if (i == j) {continue;}
            double sjj = stott(j,j);
            double sij = stott(i,j);
            if (abs(sii) < epsilon || abs(sjj) < epsilon) { //ideal vertex involved here
                edges.setEdge(i,j,std::numeric_limits<double>::infinity());
            } else {
                double cosedge = sij / std::sqrt(abs(sii*sjj));
                if (sii > 0 && sjj > 0) { //spherical case, or ultraideal-ultraideal hyperbolic
                    cosedge = std::clamp(cosedge,-1.0,1.0); //just in case
                    double edgehere = acos(cosedge);
                    edges.setEdge(i,j,edgehere*edgehere);
                } else if (sii < 0 && sjj < 0) { //normal-normal hyperbolic
                    if (cosedge > -1) {cosedge = -1;} //just in case
                    double edgehere = acosh(-cosedge);
                    edges.setEdge(i,j,edgehere*edgehere);
                } else { //normal-ultraideal imaginary distance (unverified: i only know this because chatGPT says its true :unknown:)
                    double edgehere = asinh(abs(cosedge));
                    edges.setEdge(i,j,-edgehere*edgehere);
                }
            }
        }
    }
}

const AdjMat<double>& Diagram::getEdges() {
    if (edges.empty()) {
        calcEdges();
    }
    return edges;
}

void Diagram::swapNodes(unsigned node1, unsigned node2) {
    AdjMat<Label>::swapNodes(node1,node2);
    if (!edges.empty()) {
        edges.swapNodes(node1,node2);
    }
}

void Diagram::permuteNodes(std::vector<unsigned> perm) {
    AdjMat<Label>::permuteNodes(perm);
    if (!edges.empty()) {
        edges.permuteNodes(perm);
    }
}

bool Diagram::exactEqual(const Diagram& rhs) const {
    for (unsigned i = 0; i < size(); ++i) {
        for (unsigned j = i; j < size(); ++j) {
            if (i == j) {continue;}
            if (getEdge(i,j) != rhs.getEdge(i,j)) {
                return false;
            }
        }
    }
    return true;
}

const std::unordered_map<std::unordered_multiset<Label>,std::vector<unsigned>> Diagram::getVertexSignatures() const {
    std::unordered_map<std::unordered_multiset<Label>,std::vector<unsigned>> res;
    for (unsigned i = 0; i < size(); ++i) {
        //build the set of adjacent Labels of node i
        std::unordered_multiset<Label> sig;
        std::vector<Label> adjs = getAdjacents(i);
        for (unsigned j = 0; j < size(); ++j) {
            if (i == j) {continue;}
            sig.insert(adjs[j]);
        }

        //check if it's in the map
        if (res.contains(sig)) {
            //add i to the vector there
            res.at(sig).push_back(i);
        } else {
            //craft new vector
            std::pair<std::unordered_multiset<Label>,std::vector<unsigned>> pair = {sig,{i}};
            res.insert(pair);
        }
    }
    return res;
}

const std::unordered_map<std::unordered_multiset<Label>,unsigned> Diagram::countifySignatures(const std::unordered_map<std::unordered_multiset<Label>,std::vector<unsigned>>& sigs) const {
    std::unordered_map<std::unordered_multiset<Label>,unsigned> res;
    for (const auto& pair : sigs) {
        std::pair<std::unordered_multiset<Label>,unsigned> newpair{pair.first,pair.second.size()};
        res.insert(newpair);
    }
    return res;
}

bool Diagram::subpermute(std::vector<std::vector<unsigned>>& toperm) const {
    auto it = toperm.begin();
    while (it != toperm.end() && !std::next_permutation(it->begin(),it->end())) {
        ++it;
    }
    return it != toperm.end(); //this function is actually quite elegant i think
}

bool Diagram::isomorphic(const Diagram& rhs) const {

    //check 1: if they have different numbers of nodes, they're different

    if (size() != rhs.size()) {
        return false;
    }

    //check 2: if they have different "vertex signatures", collections of edges leaving nodes, they're different
    //this should be able to tell apart the vast majority of diagrams, especially in low dimensions

    //get vertex signatures
    const std::unordered_map<std::unordered_multiset<Label>,std::vector<unsigned>> sigs1 = getVertexSignatures();
    const std::unordered_map<std::unordered_multiset<Label>,std::vector<unsigned>> sigs2 = rhs.getVertexSignatures();
    //turn them into counts
    const std::unordered_map<std::unordered_multiset<Label>,unsigned> sigc1 = countifySignatures(sigs1);
    const std::unordered_map<std::unordered_multiset<Label>,unsigned> sigc2 = countifySignatures(sigs2);
    if (sigc1 != sigc2) {
        //ousted by signatures!!
        return false;
    }

    //check 3: just permute rhs (in ways permitted by signatures) until isomorphism occurs, or cry if it doesn't
    //this can defo be optimized but i'll do that later as i need to push limits

    std::vector<std::vector<unsigned>> sigsets1; //vec of the vec entries of sigs1
    std::vector<std::vector<unsigned>> sigsets2; //vec of the vec entires of sigs2. these two should be CORRESPONDING
    for (const auto& pair : sigs1) {
        sigsets1.push_back(pair.second);
        sigsets2.push_back(sigs2.at(pair.first)); //this should always work
    } 
    
    /*
    THE PLAN:
    -build a table for where to find the numbers in order in sigsets1. nodinds[1] = {1,3} where sigsets[1][3] = 1 ...
    -for all combinations of permutations of subvectors of sigsets2
     -use nodinds to build a permutation of the diagram where perm[1] = sigsets2perm[nodinds[1].first][nodinds[1].second], ...
     -make a copy of diagram rhs, permute it according to this permutation
     -check exact equality with this diagram, if true return true
    -if none of the permutation comboes work, these diagrams must be different, and so we can return false
    */

    //build nodinds
    std::map<unsigned,std::pair<unsigned,unsigned>> nodinds;
    for (unsigned i = 0; i < sigsets1.size(); ++i) {
        for (unsigned j = 0; j < sigsets1[i].size(); ++j) {
            nodinds.insert(std::make_pair(sigsets1[i][j],std::make_pair(i,j)));
        }
    }
    //we need to sort the subvectors of sigsets2 so std::next_permutation suffices to create every permutation
    for (unsigned i = 0; i < sigsets2.size(); ++i) {
        std::sort(sigsets2[i].begin(),sigsets2[i].end());
    }
    //main loop over all permutation combinations
    do {
        //build a permutation of rhs, from the current permutation of sigsets2
        std::vector<unsigned> perm;
        for (unsigned i = 0; i < size(); ++i) {
            perm.push_back(sigsets2[nodinds[i].first][nodinds[i].second]);
        }
        //clone rhs, permute it accordingly
        Diagram permrhs = rhs;
        permrhs.permuteNodes(perm);
        //check exact equality
        if (exactEqual(permrhs)) {
            return true;
        }
    } while (subpermute(sigsets2));
    
    //if no permutation created exact equality, then fail
    return false;
}