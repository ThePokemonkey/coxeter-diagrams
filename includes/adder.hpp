#ifndef ADDER_HPP
#define ADDER_HPP


#include <unordered_map>
#include <map>
#include <unordered_set>
#include <utility>
#include <vector>
#include "../includes/diagram.hpp"

//this file shall contain various functions that are helpful for adder stuff, and eventually the whole adder

//HELPER FUNCTIONS:

bool doubleEq(double lhs, double rhs); //equality with epsilon 1e-12 on doubles
double doubleRd(double num); //rounds to nearest 2epsilon

//double versions of the Label functions for isomorphism testing. TODO: someday it would be nice to just have isomorphism checking for general AdjMats...
const std::unordered_map<std::unordered_multiset<double>,std::vector<unsigned>> getVertexSignatures(const AdjMat<double>& edges); //gets a collection of vectors of nodes that share the same edges comin out
const std::unordered_map<std::unordered_multiset<double>,unsigned> countifySignatures(const std::unordered_map<std::unordered_multiset<double>,std::vector<unsigned>>& sigs); //turns a getVertexSignatures result into one where each vector is replaced by its size
bool subpermute(std::vector<std::vector<unsigned>>& toperm); //does std::next_permutation on the sub-vecs, rolling to next one if complete. returns false if rolling all the way over
bool exactEqual(const AdjMat<double>& edges1, const AdjMat<double>& edges2);
//returns all permutations of the nodes in toperm that match with match
std::vector<std::vector<unsigned>> matchingPerms(const AdjMat<double>& toperm, const AdjMat<double>& match);


//REAL USEFUL FUNCTIONS:

//isomorphism checking for edges
bool edgesIsomorphic(const AdjMat<double>& edges1, const AdjMat<double>& edges2); //checks whether two adjmats of doubles are isomorphic. used to match facets for adding

//find potential addition sites
std::vector<std::pair<unsigned,unsigned>> sharedFacets(Diagram& lhs, Diagram& rhs); //returns pairs of node indices {index into lhs, index into rhs} whose dual facets are identical











//more hashing tomfoolery, in the future i should really modularize this better
namespace std {
    template<typename T>
    struct hash<std::unordered_multiset<T>> {
        size_t operator()(std::unordered_multiset<T> const& s) const noexcept {
            std::vector<size_t> hashes;
            hashes.reserve(s.size());
            for (auto const& value : s) {
                hashes.push_back(std::hash<T>{}(value));
            }
            std::sort(hashes.begin(), hashes.end());
            size_t h = 0;
            for (auto hv : hashes) {
                h ^= hv + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
            }
            return h;
        }
    };
}

#endif //ADDER_HPP