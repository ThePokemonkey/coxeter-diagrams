#include "../includes/twiddler.hpp"

Diagram twiddleEdge(const Diagram& base, unsigned target, unsigned respective, unsigned newden) {
    Diagram res = base;

    //double the edge
    const Label& targetedge = base.getEdge(target,respective);
    Label newlabel; //bunch of awesome logic to construct this new label

    if (targetedge.isInfinity()) { //if it's infinity ignore newden and just swap to the other retrogradity
        newlabel = Label(!targetedge.isRetrograde());
    } else {
        //normal label
        newlabel = Label(targetedge.getAxis(),newden);
    }
    newlabel.reduce(); //this is a double-edged sword. it throws away real information,
    //but you're usually going to get duplicates with the reduced and unreduced forms, so it helps reduce clutter
    //this method is already untrustworthy with respect to reducible labels, so this isn't a weird thing to do.
    //if it's already wrong, we can make it a little more wrong if it makes things cleaner

    res.setEdge(target,respective,newlabel); //set it

    //now, update all the other affected labels coming out of target according to Da Rulez
    //precompute these values which don't change when looping
    double targetcos = targetedge.getCos(); // a
    double newcos = newlabel.getCos(); // a'
    double targetsin = targetedge.getSin();
    double newsin = newlabel.getSin();
    //loop over every other mirror
    for (unsigned other = 0; other < base.size(); ++other) {
        if (other == target || other == respective) {
            continue; //selfloop or already dealt with
        }
        double origcos = base.getEdge(target,other).getCos(); // b
        double othercos = base.getEdge(other,respective).getCos(); // c

        //crazy cosine update rule
        double finalcos = ( (newsin/targetsin) * ( origcos + (targetcos*othercos) ) ) - (newcos*othercos);
        //std::cout << targetcos << " " << targetsin << " " << newcos << " " << newsin << " " << origcos << " " << othercos << " " << finalcos << std::endl;
        newlabel = Label(finalcos);
        //creating newlabel might have errored if it was illegal or a pseudogon or whatever. not our problem

        res.setEdge(target,other,newlabel);
    }

    return res; //should be good
}


std::vector<Diagram> recursiveTwiddler(const std::vector<Diagram>& initials) {
    std::vector<Diagram> results = initials; //this is the updating collection of result diagrams
    int checkedto = -1; //this is how far into the list we have checked for mutations
    unsigned news = static_cast<unsigned>(initials.size()); //keeps track of how many new diagrams were obtained this iteration
    unsigned its = 0; //how many iterations have occurred

    //i know it says "recursiveTwiddler" but it's actually iterative
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
                        continue; //no angle with itself :(
                    }

                    //now we have an angle chosen to screw with. possibilities for new denominator are from 1 to old denominator
                    const Label& targetlab = base.getEdge(targetnod,respectnod);
                    if (targetlab.isInfinity()) {continue;} //do NOT twiddle infinity, it WILL divide by zero
                    for (unsigned i = 1; i < targetlab.getAxis(); ++i) { //looping over new denominators
                        if (i == targetlab.getWinding()) {continue;} //will be identical to old diagram

                        //attempt the twiddle
                        Diagram newdiag;
                        try
                        {
                            newdiag = twiddleEdge(base,targetnod,respectnod,i);
                        }
                        catch(const std::exception&) //having catches be intended is kinda bad form i think but whatevs
                        {continue;} //oh no,, an error,,, anyway

                        //put this in ze list if it's new
                        if ((!isInList(results,newdiag) && (!isInList(newguys,newdiag)))) {
                            newguys.push_back(newdiag);
                        }
                    }
                }
            }

            /* FLIPPING SHOULD NO LONGER BE NECESSARY
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
            */
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