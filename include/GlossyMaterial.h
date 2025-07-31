#ifndef __GLOSSY_MATERIAL_H__
#define __GLOSSY_MATERIAL_H__

#include <glm/glm.hpp>

#include "Intersection.h"
#include "MaterialBase.h"
#include "Ray.h"
#include "Scene.h"

class GlossyMaterial : public MaterialBase {
   public:
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

    GlossyMaterial(glm::vec3 diffuse, glm::vec3 specular, float shininess) : diffuse(diffuse), specular(specular), shininess(shininess) {
        // convert to linear space from sRGB
        this->diffuse = RGB_to_Linear(diffuse);
        this->specular = RGB_to_Linear(specular);
    }

    Ray sample_ray_and_update_radiance(Ray &ray, Intersection &intersection) override;

    glm::vec3 get_direct_lighting(Intersection &intersection, Scene const &scene);

    glm::vec3 color_of_last_bounce(Ray &ray, Intersection &intersection, Scene const &scene) override;
};

#endif