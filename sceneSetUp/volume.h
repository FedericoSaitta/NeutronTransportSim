#pragma once

#include "../utils/types.h"

class Volume {
public:
    virtual ~Volume() = default;

    virtual bool contains(const ThreeVec &p) const = 0;
};

// For now this is just a slab in the x direction
// in the future a plane method will be made which will be more general
class Slab : public Volume{
public:
    Slab(const double xMin, const double xMax) : xMin(xMin), xMax(xMax) {}

    bool contains(const ThreeVec &p) const override {
        return (p.x <= xMax) && (p.x >= xMin);
    }

private:
    double xMin;
    double xMax;
};

class Sphere : public Volume{
public:
    Sphere(const double radius) : radius(radius) {}

    bool contains(const ThreeVec &p) const override { return (p.mag() <= radius); }

private:
    double radius;
};

class Cuboid : public Volume{
public:
    Cuboid(const ThreeVec& minCorner, const ThreeVec& maxCorner)
    : minCorner(minCorner), maxCorner(maxCorner)  {}

    bool contains(const ThreeVec& neutron) const override {
        return (neutron.x >= minCorner.x && neutron.x <= maxCorner.x) &&
               (neutron.y >= minCorner.y && neutron.y <= maxCorner.y) &&
               (neutron.z >= minCorner.z && neutron.z <= maxCorner.z);
    }

private:
    ThreeVec minCorner;
    ThreeVec maxCorner;
};