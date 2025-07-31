/*
Triangle Primitive Geometry
*/

#ifndef __PRIM_TRIANGLE_H__
#define __PRIM_TRIANGLE_H__

#include <glm/glm.hpp>

#include "GeometryBase.h"
#include "Ray.h"

class MaterialBase;

class Ray;

class GeomTriangle : public GeometryBase {
   public:
    glm::vec3 vertices[3];
    glm::vec3 normals[3];

    GeomTriangle(std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &normals);
    std::vector<Intersection> intersect(Ray &ray) override;
};

#endif