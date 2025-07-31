/**
 * Class to represent geometry shapes like Triangle and Sphere
 */

#ifndef __PRIM_GEOMETRY_BASE_H__
#define __PRIM_GEOMETRY_BASE_H__

#include <vector>

#include "Intersection.h"

class Ray;

class GeometryBase {
   public:
    virtual ~GeometryBase() = default;
    virtual std::vector<Intersection> intersect(Ray &ray) = 0;
};

#endif
