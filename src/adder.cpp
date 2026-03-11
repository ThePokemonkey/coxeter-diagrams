#include "../includes/adder.hpp"



constexpr double epsilon = 1e-12;

bool doubleEq(double lhs, double rhs) {
    return abs(lhs-rhs) < epsilon;
}

double doubleRd(double num) {
    return std::round((num/epsilon)/2)*epsilon*2;
}

const std::unordered_map<std::unordered_multiset<double>,std::vector<unsigned>> getVertexSignatures(const AdjMat<double>& edges) {
    std::unordered_map<std::unordered_multiset<double>,std::vector<unsigned>> res;
    for (unsigned i = 0; i < edges.size(); ++i) {
        //build the set of adjacent doubles of node i
        std::unordered_multiset<double> sig;
        std::vector<double> adjs = edges.getAdjacents(i);
        for (unsigned j = 0; j < edges.size(); ++j) {
            if (i == j) {continue;}
            sig.insert(doubleRd(adjs[j]));
        }

        //check if it's in the map (god is telling me that .contains() doesn't exist?? huh???)
        if (res.find(sig) != res.end()) {
            //add i to the vector there
            res.at(sig).push_back(i);
        } else {
            //craft new vector
            std::pair<std::unordered_multiset<double>,std::vector<unsigned>> pair = {sig,{i}};
            res.insert(pair);
        }
    }
    return res;
}

const std::unordered_map<std::unordered_multiset<double>,unsigned> countifySignatures(const std::unordered_map<std::unordered_multiset<double>,std::vector<unsigned>>& sigs) {
    std::unordered_map<std::unordered_multiset<double>,unsigned> res;
    for (const auto& pair : sigs) {
        std::pair<std::unordered_multiset<double>,unsigned> newpair{pair.first,pair.second.size()};
        res.insert(newpair);
    }
    return res;
}

bool subpermute(std::vector<std::vector<unsigned>>& toperm) {
    auto it = toperm.begin();
    while (it != toperm.end() && !std::next_permutation(it->begin(),it->end())) {
        ++it;
    }
    return it != toperm.end(); //this function is actually quite elegant i think
}

bool exactEqual(const AdjMat<double>& edges1, const AdjMat<double>& edges2) {

    if (edges1.size() != edges2.size()) {
        return false; //duh
    }

    for (unsigned i = 0; i < edges1.size(); ++i) {
        for (unsigned j = i; j < edges1.size(); ++j) {
            if (i == j) {continue;}
            if (!doubleEq(edges1.getEdge(i,j),edges2.getEdge(i,j))) {
                return false;
            }
        }
    }

    return true;
}

std::vector<std::vector<unsigned>> edgesIsomorphisms(const AdjMat<double>& edges1, const AdjMat<double>& edges2) {
    //nodes check
    if (edges1.size() != edges2.size()) {
        return {}; //duh
    }

    //sum all the entries and compare
    double sum1 = 0;
    double sum2 = 0;
    for (unsigned i = 0; i < edges1.size(); ++i) {
        for (unsigned j = i; j < edges1.size(); ++j) {
            if (i == j) {continue;}
            sum1 += edges1.getEdge(i,j);
            sum2 += edges2.getEdge(i,j);
        }
    }
    if (!doubleEq(sum1,sum2)) {
        return {};
    }

    //get vertex signatures
    const std::unordered_map<std::unordered_multiset<double>,std::vector<unsigned>> sigs1 = getVertexSignatures(edges1);
    const std::unordered_map<std::unordered_multiset<double>,std::vector<unsigned>> sigs2 = getVertexSignatures(edges2);
    //turn them into counts
    const std::unordered_map<std::unordered_multiset<double>,unsigned> sigc1 = countifySignatures(sigs1);
    const std::unordered_map<std::unordered_multiset<double>,unsigned> sigc2 = countifySignatures(sigs2);
    if (sigc1 != sigc2) {
        //ousted by signatures!!
        return {};
    }

    std::vector<std::vector<unsigned>> sigsets1; //vec of the vec entries of sigs1
    std::vector<std::vector<unsigned>> sigsets2; //vec of the vec entires of sigs2. these two should be CORRESPONDING
    for (const auto& pair : sigs1) {
        sigsets1.push_back(pair.second);
        sigsets2.push_back(sigs2.at(pair.first)); //this should always work
    }

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
    std::vector<std::vector<unsigned>> res;
    do {
        //build a permutation of rhs, from the current permutation of sigsets2
        std::vector<unsigned> perm;
        for (unsigned i = 0; i < edges1.size(); ++i) {
            perm.push_back(sigsets2[nodinds[i].first][nodinds[i].second]);
        }
        //clone rhs, permute it accordingly
        AdjMat<double> permrhs = edges2;
        permrhs.permuteNodes(perm);
        //check exact equality
        if (exactEqual(edges1,permrhs)) {
            res.push_back(perm);
        }
    } while (subpermute(sigsets2));
    
    //if return all perms that keep equality
    return res;
}

std::vector<unsigned> fullPermFromSubPerm(unsigned remnode, const std::vector<unsigned>& perm) {
    std::vector<unsigned> res;
    for (unsigned i = 0; i < perm.size()+1; ++i) {
        if (i < remnode) {
            unsigned target = perm[i];
            if (target >= remnode) {
                ++target;
            }
            res.push_back(target);
        } else if (i > remnode) {
            unsigned target = perm[i-1];
            if (target >= remnode) {
                ++target;
            }
            res.push_back(target);
        } else {
            res.push_back(remnode);
        }
    }
    return res;
}


std::vector<Diagram> makeAdditions(Diagram& lhs, Diagram& rhs) {
    //if diagrams different size obv nothing
    if (lhs.size() != rhs.size()) {
        return {};
    }
    //craft result
    std::vector<Diagram> res;
    //scan all pairs of facets (getedges subgraphs) and check for isomorphism
    for (unsigned nodeL = 0; nodeL < lhs.size(); ++nodeL) {
        AdjMat<double> lsub = lhs.getEdges().subgraph(nodeL);
        for (unsigned nodeR = 0; nodeR < rhs.size(); ++nodeR) {
            AdjMat<double> rsub = rhs.getEdges().subgraph(nodeR);
            
            std::vector<std::vector<unsigned>> isos = edgesIsomorphisms(lsub,rsub);
            std::vector<Label> ladjs = lhs.getAdjacents(nodeL); //for later
            //loop over every possible way to attach rhs to lhs
            for (unsigned isoi = 0; isoi < isos.size(); ++isoi) {
                //build a version of rhs that's permuted to match lhs according to this iso
                Diagram rhsp = rhs;
                rhsp.permuteNodes(fullPermFromSubPerm(nodeR,isos[isoi]));
                std::vector<Label> radjs = rhsp.getAdjacents(nodeR); 

                //now, check corresponding adjacent labels on these to see if exactly one isn't complementary
                //if all of them are complimentary we'll get a lune rather than a simplex
                unsigned noncompcount = 0;
                unsigned noncomp = 0; //the one... angi... that isn't complementary
                unsigned noncompL = 0; //this will hold the one node on lhs that isn't complementary, which is important info for later
                unsigned noncompR = 0; //this will hold the one node on rhs that isn't complementary, which is important info for later
                for (unsigned angi = 0; angi < lhs.size()-1; ++angi) {
                    //avoid selfloop situations properly
                    //these will correspond because we permuted rhs
                    unsigned lhsangi = angi;
                    if (lhsangi >= nodeL) {
                        ++lhsangi;
                    }
                    unsigned rhsangi = angi;
                    if (rhsangi >= nodeR) {
                        ++rhsangi;
                    }

                    //then check if the angles arent complimentary
                    if (!ladjs[lhsangi].isComplementary(radjs[rhsangi])) {
                        ++noncompcount;
                        if (noncompcount > 1) {
                            break;
                        }
                        noncomp = angi;
                        noncompL = lhsangi;
                        noncompR = rhsangi;
                    }
                }
                if (noncompcount != 1) { 
                    continue; //this is not going to add to a simplex
                }
                Label angL = lhs.getEdge(nodeL,noncompL);
                Label angR = rhsp.getEdge(nodeR,noncompR);
                //check status on the merged angles (the ones between the blended facets and the noncomp facets)
                if ((angL.getAxis() != angR.getAxis()) || angL.getWinding() + angR.getWinding() >= angL.getAxis() || angL.getWinding() + angR.getWinding() == 0 ) { // == 0 for the += inf case
                    continue; //the merged angles are incompatible
                }

                //FINAL CHECK: angles between facets that are all noncomp and not blended must all be identical
                bool fail = false;
                for (unsigned nodeabs1 = 0; nodeabs1 < lhs.size()-1; ++nodeabs1) {
                    if (nodeabs1 == noncomp) {continue;} //do not consider noncomp
                    unsigned lhsnode1 = nodeabs1;
                    if (lhsnode1 >= nodeL) {
                        ++lhsnode1;
                    }
                    unsigned rhsnode1 = nodeabs1;
                    if (rhsnode1 >= nodeR) {
                        ++rhsnode1;
                    }
                    //lhsnode1 and rhsnode1 now correspond to corresponding node indices in lhs and rhsp (which are neither noncomp nor nodeX)
                    //need to check every pair, do anotha one
                    for (unsigned nodeabs2 = 0; nodeabs2 < lhs.size()-1; ++nodeabs2) {
                        if (nodeabs2 == noncomp || nodeabs1 == nodeabs2) {continue;} //do not consider noncomp or selfloop
                        unsigned lhsnode2 = nodeabs2;
                        if (lhsnode2 >= nodeL) {
                            ++lhsnode2;
                        }
                        unsigned rhsnode2 = nodeabs2;
                        if (rhsnode2 >= nodeR) {
                            ++rhsnode2;
                        }
                        //lhsnode2 and rhsnode2 now also correspond to corresponding allowed node indices in lhs and rhsp
                        
                        Label lhscheck = lhs.getEdge(lhsnode1,lhsnode2);
                        Label rhscheck = rhsp.getEdge(rhsnode1,rhsnode2);
                        if (lhscheck != rhscheck) { //make sure its all the same
                            fail = true;
                            break;
                        }

                    }
                    if (fail) {
                        break;
                    }
                }
                if (fail) {
                    //invalid addition, move on
                    continue;
                }

                //this is a valid addition! let's do it!
                Diagram addition(2);
                //consider node 0 to be the one from the lhs noncomp and node 1 to be from the rhsp noncomp
                for (unsigned nodeabs1 = 0; nodeabs1 < lhs.size()-1; ++nodeabs1) {
                    if (nodeabs1 == noncomp) {continue;} //do not handle noncomp here
                    unsigned lhsnode1 = nodeabs1;
                    if (lhsnode1 >= nodeL) {
                        ++lhsnode1;
                    }
                    unsigned rhsnode1 = nodeabs1;
                    if (rhsnode1 >= nodeR) {
                        ++rhsnode1;
                    }

                    //this should be one of the merged nodes
                    //create it and set its relations to the noncomps
                    unsigned newnode = addition.size();
                    addition.addNode();
                    addition.setEdge(newnode,0,lhs.getEdge(lhsnode1, noncompL));
                    addition.setEdge(newnode,1,rhsp.getEdge(rhsnode1, noncompR));

                    //set relations to all the previous merged nodes in another for loop
                    for (unsigned nodeabs2 = 0; nodeabs2 < nodeabs1; ++nodeabs2) {
                        if (nodeabs2 == noncomp || nodeabs1 == nodeabs2) {continue;} //do not consider noncomp or selfloop
                        unsigned lhsnode2 = nodeabs2;
                        if (lhsnode2 >= nodeL) {
                            ++lhsnode2;
                        }
                        unsigned rhsnode2 = nodeabs2;
                        if (rhsnode2 >= nodeR) {
                            ++rhsnode2;
                        }
                        unsigned oldnode = newnode-(nodeabs1-nodeabs2);
                        addition.setEdge(newnode,oldnode,lhs.getEdge(lhsnode1,lhsnode2)); //could also use rhsp
                    }
                }
                //now we've done every angle except between the two noncomps
                //this is num/(dem1+dem2)
                addition.setEdge(0,1,Label(angL.getAxis(),angL.getWinding()+angR.getWinding()));

                //this SHOULD be a completed addition!!! add it to the results and move on
                res.push_back(addition);
            }
        }
    }
    return res;
}