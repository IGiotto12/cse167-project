/*
Sphere Primitive Geometry
*/

#ifndef __PRIM_SPHERE_H__
#define __PRIM_SPHERE_H__

#include <glm/glm.hpp>

#include "GeometryBase.h"
#include "Ray.h"

class MaterialBase;

class Ray;

class GeomSphere : public GeometryBase {
   public:
    float radius;
    glm::vec3 center;

    GeomSphere(float radius, glm::vec3 center) : radius(radius), center(center) {};
    std::vector<Intersection> intersect(Ray &ray) override;
};

#endif