/*
Scene contains the information about all geometries.
*/

#ifndef __SCENE_H__
#define __SCENE_H__

#include <glm/glm.hpp>
#include <vector>

#include "MaterialBase.h"
#include "ModelBase.h"
#include "ShadingMode.h"

class Ray;
struct Intersection;

struct Node {
    std::vector<std::unique_ptr<Node>> childnodes;
    std::vector<glm::mat4> childtransforms;
    std::unique_ptr<ModelBase> model;  // Only leaf node would have this set
};

class Scene {
   public:
    // List of all Models
    std::vector<std::unique_ptr<ModelBase>> models;

    // To switch between different shading modes
    ShadingMode shading_mode;

    // List of light sources
    std::vector<ModelBase *> light_sources;

    Scene(std::unique_ptr<Node> rootNode);

    Ray intersect(Ray &ray) const;

    glm::vec3 get_sky_color(Ray &ray) const;
};

#endif