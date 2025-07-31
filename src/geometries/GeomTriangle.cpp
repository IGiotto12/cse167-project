#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include "GeomTriangle.h"

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "Intersection.h"
#include "Ray.h"

GeomTriangle::GeomTriangle(std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &normals) {
    this->vertices[0] = vertices[0];
    this->vertices[1] = vertices[1];
    this->vertices[2] = vertices[2];

    this->normals[0] = normals[0];
    this->normals[1] = normals[1];
    this->normals[2] = normals[2];
}

std::vector<Intersection> GeomTriangle::intersect(Ray &ray) {
    using namespace glm;
    /**
     * NOTE: Ray is already transformed to the Model coordinate space.
     */

    // vector to store the intersections
    std::vector<Intersection> intersections;

    /**
     * TODO: Implement the Ray intersection with the current geometry
     */

    /**
     * Once you find the intersection, add it to the `intersections` vector.
     *
     * Example:
     * Suppose the ray intersects the current geometry at a point `vec3 point`
     * at a distance `float t`, and the unit normal vector at the intersection
     * point is `vec3 normal`. You would then insert an intersection into the
     * vector as follows:
     *
     * intersections.push_back({t, point, normal, this, nullptr});
     *
     * Note:
     * - Here we pass the Model pointer as `nullptr` because it will be
     *   populated by the Model::intersect() function call.
     * - Only add intersections that are in front of the camera, i.e.,
     *   where t > 0.
     */

    /**
     * TODO: Update `intersections`
     */

    vec3 v0 = vertices[0];
    vec3 v1 = vertices[1];
    vec3 v2 = vertices[2];
    

    vec3 origin = ray.p0;
    vec3 direction = ray.dir;
    
    vec3 edge1 = v1 - v0;
    vec3 edge2 = v2 - v0;
    
    vec3 P = cross(direction, edge2);
    float determinant = dot(edge1, P);
    
    const float EPSILON = 0.0000001f;
    if (fabs(determinant) < EPSILON) {
        return intersections;
    }
    
    float inverse_determinant = 1.0f / determinant;
    vec3 T = origin - v0;
    
    float u = dot(T, P) * inverse_determinant;    
    if (u < 0.0f || u > 1.0f) {
        return intersections;
    }
    
    vec3 Q = cross(T, edge1);
    float v = dot(direction, Q) * inverse_determinant;    
    if (v < 0.0f || u + v > 1.0f) {
        return intersections;
    }
    
    float t = dot(edge2, Q) * inverse_determinant;
    if (t > 0.0f) {
        vec3 point = origin + t * direction;
        
        float w = 1.0f - u - v;
        vec3 normal = w * normals[0] + u * normals[1] + v * normals[2];
        normal = normalize(normal);
        
        intersections.push_back({t, point, normal, this, nullptr});
    }

    return intersections;
}