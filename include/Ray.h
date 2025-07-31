/*
Class representing a Ray
*/

#ifndef __RAY_H__
#define __RAY_H__

#include <glm/glm.hpp>
#include <utility>
#include <vector>

#include "Intersection.h"

struct Ray {
    int pixel_x_coordinate, pixel_y_coordinate;  // Pixel cordinates to keep track of origin pixel for ray
    int n_bounces = 0;                           // Max bounces performed by this ray
    bool isWip = true;                           // To signal termination of a ray (set false to terminate)
    bool is_diffuse_bounce = false;              // This attribute is used to prevent double counting of light contribution
                                                 // for diffuse ray bounces
    glm::vec3 debug_color = glm::vec3(0.0f);     // Use this to debug

    // Geometry parameters
    glm::vec3 p0;   // basepoint of the ray
    glm::vec3 dir;  // unit direction vector

    // Lighting parameters
    glm::vec3 W_wip = glm::vec3(1.0f);  // wip: Work in progress
    glm::vec3 color = glm::vec3(0.0f);  // Pixel color at end of each bounce

    // [{t, point, normal, geometry, model}, ...]
    std::vector<Intersection> intersections;  // To keep track of all intersections of a ray in given pass
};

#endif