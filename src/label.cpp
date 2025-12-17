#include "..\includes\label.hpp"

Label::Label(int num) {
    if (num <= 0) {
        throw std::invalid_argument("Label constructor: Only positive numerator permitted!");
    } else if (num == 1) {
        throw std::invalid_argument("Label constructor: No 1 Numerator!");
    }
    
    num_ = num; //definitely not retrograde
}

Label::Label(int num, int den) {
    if (num <= 0 || den <= 0) {
        throw std::invalid_argument("Label constructor: Only positive inputs permitted!");
    } else if (num == 1) {
        throw std::invalid_argument("Label constructor: No 1 Numerator!");
    } else if (den >= num) {
        throw std::invalid_argument("Label constructor: Denominator too large!");
    }

    num_ = num;
    den_ = den;

    is_retrograde_ = (den_ > (num_/2)); //integer devision funny
}

Label::Label(bool retrograde) {
    is_infty_ = true;
    is_retrograde_ = retrograde;
}

double Label::GetValue() const {
    if (is_infty_) {
        if (is_retrograde_) {
            return -2;
        } else {
            return -1;
        }
    }

    return static_cast<double>(num_)/static_cast<double>(den_);
}

int Label::GetAxis() const {
    if (is_infty_) {
        return -1;
    }
    return num_;
}

int Label::GetOrder() const {
    if (is_infty_) {
        return -1;
    }
    return num_/std::gcd(num_,den_);
}

void Label::Retrograde() {
    if (is_infty_) {
        is_retrograde_ = !is_retrograde_;
        return;
    }
    den_ = num_-den_;
    is_retrograde_ = (den_ > (num_/2)); //recalc because for things like 2 or 4/2 it might not have swapped
}

Label Label::GetRetrograde() const {
    if (is_infty_) {
        return Label(!is_retrograde_);
    }
    return Label(num_,num_-den_);
}

bool Label::IsRetrograde() const {
    return is_retrograde_;
}

std::ostream& operator<<(std::ostream& os, const Label& toprint) {
    if (toprint.is_infty_) {
        os << '~'; //apparently making an infinity symbol work portably is a nightmare so i guess its a tilde fine
        if (toprint.is_retrograde_) {
            os << '\''; //retrograde infinity looks like ∞' according to klitzing
        }
        return os;
    }

    os << toprint.num_;

    if (toprint.den_ != 1) {
        os << '/' << toprint.den_;
    }
    return os;
}