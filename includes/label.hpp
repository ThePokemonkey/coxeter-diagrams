#ifndef LABEL_HPP
#define LABEL_HPP

#include <string>
#include <stdexcept>
#include <numeric>
#include <iostream>

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

    private:
    
    int num_ = 2; //of course the default label is 2
    int den_ = 1;
    bool is_infty_ = false; //if this is true we treat it as an infinity label
    bool is_retrograde_ = false; //easy value to tell if it's prograde or retrograde, important for infinity
    
    friend std::ostream& operator<<(std::ostream& os, const Label& toprint); //print a representation of this label
};



#endif //LABEL_HPP