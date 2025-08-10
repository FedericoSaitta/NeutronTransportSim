#pragma once

#include <iostream>

enum EnableOptimizations {
    NO_OPT=0,
    OPT=1,
};

class ThreeVec {
public:
    double x;
    double y;
    double z;

    ThreeVec() : x(0.0), y(0.0), z(0.0) {}
    ThreeVec(const double a, const double b, const double c) : x(a), y(b), z(c) {}

    // Overloading + operator
    ThreeVec operator+ (const ThreeVec& other) const {
        return { this->x + other.x, this->y + other.y, this->z + other.z};
    }

    ThreeVec operator* (const ThreeVec& other) const {
        return { this->x * other.x, this->y * other.y, this->z * other.z};
    }

    ThreeVec operator* (const double coeff) const {
        return { this->x * coeff, this->y * coeff, this->z * coeff};
    }

    double mag() const { return std::sqrt(x*x + y*y  + z*z); }
    double mag2() const { return x*x + y*y  + z*z; }

    void update(double a, double b, double c) {
        this->x = a;
        this->y = b;
        this->z = c;
    }

    void update(ThreeVec threevec) {
        this->x = threevec.x;
        this->y = threevec.y;
        this->z = threevec.z;
    }

    void print() const {
        std::cout << "ThreeVec: " << x << " ," << y << " ," << z << '\n';
    }


};