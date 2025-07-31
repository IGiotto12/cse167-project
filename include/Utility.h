#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

static glm::vec3 RGB_to_Linear(glm::vec3 color) {
    // Gamma correction distorts color
    // this transformation accounts for that.
    return glm::pow(color, glm::vec3(2.2f));
}

static float degree_to_rad(float degrees) {
    return (float)(degrees * M_PI / 180.0);
}

static glm::vec3 align_with_normal(const glm::vec3& v, const glm::vec3& normal) {
    using namespace glm;
    // Build an orthonormal basis(tangent, bitangent, normal)
    vec3 up = (abs(normal.y) < 0.999f) ? vec3(0, 1, 0) : vec3(1, 0, 0);
    vec3 tangent = normalize(cross(up, normal));
    vec3 bitangent = cross(normal, tangent);

    return normalize(v.x * tangent + v.y * normal + v.z * bitangent);
}

#endif