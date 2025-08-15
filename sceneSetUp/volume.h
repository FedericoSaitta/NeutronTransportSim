#pragma once

#include "../utils/types.h"

class Volume {
public:
    virtual ~Volume() = default;
    virtual bool contains(const TwoVec& p) const = 0;
};

// For now this is just a slab in the x direction
// in the future a plane method will be made which will be more general
class Slab final : public Volume{
public:
    explicit Slab(const double xMin, const double xMax) : xMin(xMin), xMax(xMax) {}

    bool contains(const TwoVec& p) const override {
        return (p.x <= xMax) && (p.x >= xMin);
    }

private:
    double xMin;
    double xMax;
};

class Sphere final : public Volume{
public:
    explicit Sphere(const double radius) : radius(radius) {}

    bool contains(const TwoVec &p) const override { return (p.mag() <= radius); }

private:
    double radius;
};

class Cuboid final : public Volume{
public:
    Cuboid(const TwoVec& minCorner, const TwoVec& maxCorner)
    : minCorner(minCorner), maxCorner(maxCorner)  {}

    bool contains(const TwoVec& neutron) const override {
        return (neutron.x >= minCorner.x && neutron.x <= maxCorner.x) &&
               (neutron.y >= minCorner.y && neutron.y <= maxCorner.y);
    }

private:
    TwoVec minCorner;
    TwoVec maxCorner;
};