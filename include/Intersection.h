#ifndef __INTERSECTION_H__
#define __INTERSECTION_H__

#include <glm/glm.hpp>

class GeometryBase;
class ModelBase;

struct Intersection {
    float t;                 // Distance from the ray origin to the intersection point
    glm::vec3 point;         // The intersection point
    glm::vec3 normal;        // The normal vector of the geometry at the intersection point
    GeometryBase *geometry;  // Pointer to the Geometry object where the intersection occurred
    ModelBase *model;        // Pointer to the Model object to which the intersected geometry belongs
};

#endif