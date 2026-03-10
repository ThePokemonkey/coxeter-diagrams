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

bool edgesIsomorphic(const AdjMat<double>& edges1, const AdjMat<double>& edges2) {
    //nodes check
    if (edges1.size() != edges2.size()) {
        return false; //duh
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
        return false;
    }

    //get vertex signatures
    const std::unordered_map<std::unordered_multiset<double>,std::vector<unsigned>> sigs1 = getVertexSignatures(edges1);
    const std::unordered_map<std::unordered_multiset<double>,std::vector<unsigned>> sigs2 = getVertexSignatures(edges2);
    //turn them into counts
    const std::unordered_map<std::unordered_multiset<double>,unsigned> sigc1 = countifySignatures(sigs1);
    const std::unordered_map<std::unordered_multiset<double>,unsigned> sigc2 = countifySignatures(sigs2);
    if (sigc1 != sigc2) {
        //ousted by signatures!!
        return false;
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
            return true;
        }
    } while (subpermute(sigsets2));
    
    //if no permutation created exact equality, then fail
    return false;
}

std::vector<std::pair<unsigned,unsigned>> sharedFacets(Diagram& lhs, Diagram& rhs) {
    //if diagrams different size obv nothing
    if (lhs.size() != rhs.size()) {
        return {};
    }
    //craft result
    std::vector<std::pair<unsigned,unsigned>> res;
    //scan all pairs of facets (getedges subgraphs) and check for equality
    for (unsigned nodeL = 0; nodeL < lhs.size(); ++nodeL) {
        AdjMat<double> lsub = lhs.getEdges().subgraph(nodeL);
        for (unsigned nodeR = 0; nodeR < rhs.size(); ++nodeR) {
            AdjMat<double> rsub = rhs.getEdges().subgraph(nodeR);
            if (edgesIsomorphic(lsub,rsub)) {
                res.push_back(std::make_pair(nodeL,nodeR));
            }
        }
    }
    return res;
}

std::vector<std::vector<unsigned>> matchingPerms(const AdjMat<double>& toperm, const AdjMat<double>& match) {
    //TODO: write this function
    return {};
}