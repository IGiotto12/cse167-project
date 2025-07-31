#include "Square.h"

#include <glm/gtc/random.hpp>
#include <iostream>
#include <vector>

using namespace glm;

Square::Square(vec3 center, float side_len, vec3 normal, std::shared_ptr<MaterialBase> mat) {
    this->material = mat;
    this->side_len = side_len;

    normal = glm::normalize(normal);

    // Choose a reference vector that is not parallel to normal
    glm::vec3 reference = (glm::abs(normal.x) > 0.9f) ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);

    // Compute tangent and bitangent
    glm::vec3 tangent = glm::normalize(glm::cross(normal, reference));
    glm::vec3 bitangent = glm::normalize(glm::cross(normal, tangent));

    float halfSide = side_len / 2.0f;

    this->tangent = tangent;
    this->bitangent = bitangent;

    // Calculate the four corners
    glm::vec3 v0 = center - halfSide * tangent - halfSide * bitangent;
    glm::vec3 v1 = center + halfSide * tangent - halfSide * bitangent;
    glm::vec3 v2 = center + halfSide * tangent + halfSide * bitangent;
    glm::vec3 v3 = center - halfSide * tangent + halfSide * bitangent;

    // Create two triangles
    std::vector<vec3> vertices_1 = {v0, v1, v2};
    std::vector<vec3> vertices_2 = {v0, v2, v3};
    std::vector<vec3> normals = {normal, normal, normal};
    geometries.push_back(std::make_unique<GeomTriangle>(vertices_1, normals));
    geometries.push_back(std::make_unique<GeomTriangle>(vertices_2, normals));
}

vec3 Square::get_surface_point() {
    using namespace glm;
    /**
     * TODO: Task 4.2
     * Implement uniform random point sampling on the square
     */

    float u = linearRand(-0.5f, 0.5f);
    float v = linearRand(-0.5f, 0.5f);

    /**
     * TODO: use tangent and bitangent to sample a uniformly random point on square
     */
    // vec3 samplePoint = center;
    vec3 samplePoint = center + v * side_len * bitangent + u * side_len * tangent ;


    // transform to world space
    return vec3(transformation_matrix * vec4(samplePoint, 1.0f));
}
