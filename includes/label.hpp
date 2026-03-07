#ifndef LABEL_HPP
#define LABEL_HPP

#include <string>
#include <stdexcept>
#include <numeric>
#include <iostream>
#include <unordered_set>
#include <algorithm>

class Label { //one label of a coxeter diagram
    public:

    Label() = default; //makes a 2 label
    Label(int num); //makes a label with just this numerator
    Label(int num, int den); //makes a label with numerator and denominator
    Label(bool retrograde); //creates an infinity label with the specified retrogradity
    double GetValue() const; //returns the float numerical value of that label. -1 if infinity, -2 if retrograde infinity
    int GetAxis() const; //returns the numerator of the label. -1 if infinity
    int GetOrder() const; //returns the numerator of the label if the fraction were to be reduced. aka rotational symmetry order here. -1 if infinity
    void Retrograde(); //turns this label retrograde in place
    Label GetRetrograde() const; //returns the retrograde of this label
    bool IsRetrograde() const; //returns if this label is retrograde
    bool IsInfinity() const; //returns if this label is infinity (or retrograde infinity)
    bool operator==(const Label& rhs) const; //tells label equality

    friend struct std::hash<Label>; //so i can put these in a set
    friend struct std::hash<std::unordered_multiset<Label>>; //so i can put THOSE in a set

    private:
    
    int num_ = 2; //of course the default label is 2
    int den_ = 1;
    bool is_infty_ = false; //if this is true we treat it as an infinity label
    bool is_retrograde_ = false; //easy value to tell if it's prograde or retrograde, important for infinity
    
    friend std::ostream& operator<<(std::ostream& os, const Label& toprint); //print a representation of this label
};

//making custom hash functions is scary and i don't know whats going on so i made chatgpt do it for now
//TODO: learn this stuff and make a smart human implementation
namespace std {
    template<>
    struct hash<Label> {
        size_t operator()(const Label& l) const {
            size_t h = std::hash<int>{}(l.num_);
            h = h * 31 + std::hash<int>{}(l.den_);
            h = h * 31 + std::hash<bool>{}(l.is_infty_);
            h = h * 31 + std::hash<bool>{}(l.is_retrograde_);
            return h;
        }
    };

    template<>
    struct hash<std::unordered_multiset<Label>> {
        size_t operator()(std::unordered_multiset<Label> const& s) const noexcept {
            // order-independent hashing: collect individual hashes, sort, then combine
            std::vector<size_t> hashes;
            hashes.reserve(s.size());
            for (auto const& lab : s) {
                hashes.push_back(std::hash<Label>{}(lab));
            }
            std::sort(hashes.begin(), hashes.end());
            size_t h = 0;
            for (auto hv : hashes) {
                // classical 64-bit mix
                h ^= hv + 0x9e3779b97f4a7c15ULL + (h<<6) + (h>>2);
            }
            return h;
        }
    };
}



#endif //LABEL_HPP