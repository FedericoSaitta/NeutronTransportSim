#pragma once

#include "../utils/types.h"

class Volume {
public:
    virtual ~Volume() = default;
    virtual bool contains(const TwoVec& p) const = 0;
    virtual ShapeType shapeType() const = 0;
    virtual RenderInfo renderInfo() const = 0;
};

// For now this is just a slab in the x direction
// in the future a plane method will be made which will be more general
class Slab final : public Volume{
public:
    explicit Slab(const double xMin, const double xMax) : xMin(xMin), xMax(xMax) {}

    bool contains(const TwoVec& p) const override {
        return (p.x <= xMax) && (p.x >= xMin);
    }

    ShapeType shapeType() const override { return SLAB; }

    // For now just huge number in the y direction, not inf though
    RenderInfo renderInfo() const override {
        return {SLAB, xMax - xMin, 999999.9, (xMax + xMin) / 2, 0.0};
    }

private:
    double xMin;
    double xMax;
};

class Sphere final : public Volume{
public:
    explicit Sphere(const double radius) : radius(radius) {}

    bool contains(const TwoVec &p) const override { return (p.mag() <= radius); }
    ShapeType shapeType() const override { return CIRCLE; }

    RenderInfo renderInfo() const override {
        return {CIRCLE, radius, radius, 0.0, 0.0};
    }

private:
    double radius;
};

class Rectanle final : public Volume{
public:
    Rectanle(const TwoVec& minCorner, const TwoVec& maxCorner)
    : minCorner(minCorner), maxCorner(maxCorner)  {}

    bool contains(const TwoVec& neutron) const override {
        return (neutron.x >= minCorner.x && neutron.x <= maxCorner.x) &&
               (neutron.y >= minCorner.y && neutron.y <= maxCorner.y);
    }

    ShapeType shapeType() const override { return ShapeType::RECTANGLE; }
    RenderInfo renderInfo() const override {
        return {RECTANGLE, maxCorner.x - minCorner.x , maxCorner.y - minCorner.y,
                (maxCorner.x + minCorner.x) / 2, (maxCorner.y + minCorner.y) / 2};
    }

private:
    TwoVec minCorner;
    TwoVec maxCorner;
};