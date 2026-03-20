#include "../includes/doubler.hpp"

Diagram doubleEdge(const Diagram& base, unsigned target, unsigned respective) {
    Diagram res = base;

    //double the edge
    const Label& targetedge = base.getEdge(target,respective);
    Label newlabel; //bunch of awesome logic to construct this new label
    if (targetedge.isInfinity()) {
        if (targetedge.isRetrograde()) {
            throw std::invalid_argument("doubleEdge: Cannot double a retrograde infinity!");
        }
        newlabel = Label(false); //double infinity is just infinity
    } else {
        //normal label
        const int num = targetedge.getAxis();
        const int den = 2*targetedge.getWinding();
        if (den < num) {
            newlabel = Label(num,den);
        } else {
            throw std::invalid_argument("doubleEdge: Angle too shallow to double!");
        }
    }
    newlabel.reduce(); //this is a double-edged sword. it throws away real information,
    //but you're usually going to get duplicates with the reduced and unreduced forms, so it helps reduce clutter
    //this method is already untrustworthy with respect to reducible labels, so this isn't a weird thing to do.
    //if it's already wrong, we can make it a little more wrong if it makes things cleaner

    res.setEdge(target,respective,newlabel); //set it

    //now, update all the other affected labels coming out of target according to p9's magic rule
    double targetchord = targetedge.getChord(); // a
    for (unsigned other = 0; other < base.size(); ++other) {
        if (other == target || other == respective) {
            continue; //selfloop or already dealt with
        }
        double origchord = base.getEdge(target,other).getChord(); // b
        double otherchord = base.getEdge(other,respective).getChord(); // c
        newlabel = Label((targetchord*origchord)+otherchord); //p9's magic rule (awesome variable reuse too)
        //creating newlabel might have errored if it was illegal or a pseudogon or whatever. not our problem

        res.setEdge(target,other,newlabel);
    }

    return res; //should be good
}


std::vector<Diagram> recursiveDoubler(const std::vector<Diagram>& initials) {
    std::vector<Diagram> results = initials; //this is the updating collection of result diagrams
    int checkedto = -1; //this is how far into the list we have checked for mutations
    unsigned news = static_cast<unsigned>(initials.size()); //keeps track of how many new diagrams were obtained this iteration
    unsigned its = 0; //how many iterations have occurred

    //i know it says "recursiveDoubler" but it's actually iterative
    while (news > 0) {
        ++its;
        std::cout << its << ": starting with " << results.size() << std::endl;;
        //store the ones found this iteration
        std::vector<Diagram> newguys;
        //iterate over every mutable diagram
        for (unsigned baseind = checkedto+1; baseind < results.size(); ++baseind) { //save a lot of time by skipping over checkedto
            const Diagram& base = results[baseind];
            //iterate over every ordered pair of nodes in this base
            for (unsigned targetnod = 0; targetnod < base.size(); ++targetnod) {
                for (unsigned respectnod = 0; respectnod < base.size(); ++respectnod) { //angle doubling is not commutative, so we can't start at targetnod here
                    if (targetnod == respectnod) {
                        continue; //no no
                    }

                    //try to double this angle
                    Diagram newdiag;
                    try
                    {
                        newdiag = doubleEdge(base,targetnod,respectnod);
                    }
                    catch(const std::exception&) //having catches be intended is kinda bad form i think but whatevs
                    {continue;} //oh no,, an error,,, anyway

                    //put this in ze list if it's new
                    if ((!isInList(results,newdiag) && (!isInList(newguys,newdiag)))) {
                        newguys.push_back(newdiag);
                    }
                }
            }

            //try flipping every node in this diagram
            //in the future it would be more optimized to do every flipping pattern at once
            //then we could know that flipping a diagram we got by flipping would never be useful, and avoid a lot of work
            //not doing this rn though
            for (unsigned flipnod = 0; flipnod < base.size(); ++flipnod) {
                Diagram newdiag = base.getInverted(flipnod); //flippied
                //put this in ze list if it's new
                if ((!isInList(results,newdiag) && (!isInList(newguys,newdiag)))) {
                    newguys.push_back(newdiag);
                }
            }
        }

        //every mutation has been made. update the bookkeeping
        checkedto += news; //we've checked all the previously-new diagrams now
        news = static_cast<unsigned>(newguys.size()); //new news
        //put the newguys into results
        for (unsigned i = 0; i < newguys.size(); ++i) {
            results.push_back(newguys[i]);
        }
    }

    return results; //yipee!!!!
}