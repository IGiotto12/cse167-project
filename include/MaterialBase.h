/*
Class Defining Material Properties for lightings
*/
#ifndef __MATERIA_BASE_H__
#define __MATERIA_BASE_H__

#include <glm/glm.hpp>

#include "Intersection.h"
#include "Ray.h"
#include "Utility.h"

class Scene;

class MaterialBase {
   public:
    glm::vec3 emission = glm::vec3(0.0f);    // defines RGB color of Light source
    glm::vec3 intensity = glm::vec3(10.0f);  // Intensity of source

    virtual Ray sample_ray_and_update_radiance(Ray &ray, Intersection &intersection) = 0;

    virtual float get_light_attenuation_factor(float distance) {
        // c0 = 1, c1 = 0, c2 = 4π
        return (1 + 4 * M_PI * distance * distance);
    }

    virtual void convert_to_light(glm::vec3 color, glm::vec3 intensity) {
        // update class members
        this->intensity = intensity;

        // convert sRGB color to linear space
        this->emission = RGB_to_Linear(color) * intensity;
    }

    virtual glm::vec3 color_of_last_bounce(Ray &ray, Intersection &intersection, Scene const &scene) = 0;
};

#endif