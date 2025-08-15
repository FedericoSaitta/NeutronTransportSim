#pragma once

#include <iostream>

enum EnableOptimizations {
    NO_OPT=0,
    OPT=1,
};

enum ShapeType {
    CIRCLE=0,
    RECTANGLE=1,
    SLAB=2,
};

struct RenderInfo {
    ShapeType type;
    double width;  // x direction (radius if circle)
    double height; // y direction (radius if circle)
    double x, y;
};

struct SimReuslts {
    size_t absorbed;
    size_t reflected;
    size_t transmitted;
};


class TwoVec {
public:
    double x;
    double y;

    TwoVec() : x(0.0), y(0.0) {}
    TwoVec(const double a, const double b) : x(a), y(b) {}

    // Overloading + operator
    TwoVec operator+ (const TwoVec& other) const {
        return { this->x + other.x, this->y + other.y};
    }

    TwoVec operator* (const TwoVec& other) const {
        return { this->x * other.x, this->y * other.y};
    }

    TwoVec operator* (const double coeff) const {
        return { this->x * coeff, this->y * coeff};
    }

    double mag() const { return std::sqrt(x*x + y*y); }
    double mag2() const { return x*x + y*y; }

    void update(const double a, const double b) {
        this->x = a;
        this->y = b;
    }

    void update(const TwoVec& threevec) {
        this->x = threevec.x;
        this->y = threevec.y;
    }

    void print() const {
        std::cout << "TwoVec: " << x << " ," << y << '\n';
    }


};