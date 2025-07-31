#ifndef __GLASS_MATERIAL_H__
#define __GLASS_MATERIAL_H__

#include <glm/glm.hpp>

#include "Intersection.h"
#include "MaterialBase.h"
#include "Ray.h"
#include "Scene.h"

class GlassMaterial : public MaterialBase {
   public:
    glm::vec3 color;       // Color tint of the glass
    float ior;             // Index of refraction
    float transparency;    // How transparent the glass is (0-1)

    GlassMaterial(glm::vec3 color, float ior, float transparency) 
        : color(color), ior(ior), transparency(transparency) {
        // convert to linear space from sRGB
        this->color = RGB_to_Linear(color);
    }

    Ray sample_ray_and_update_radiance(Ray &ray, Intersection &intersection) override;

    glm::vec3 get_direct_lighting(Intersection &intersection, Scene const &scene);

    glm::vec3 color_of_last_bounce(Ray &ray, Intersection &intersection, Scene const &scene) override;

   private:
    // Helper function to determine if total internal reflection occurs
    bool is_total_internal_reflection(const glm::vec3 &incident, const glm::vec3 &normal, float n1, float n2);
    
    // Helper function to calculate refraction direction using Snell's law
    glm::vec3 refract_ray(const glm::vec3 &incident, const glm::vec3 &normal, float n1, float n2);
    
    // Helper function to calculate Fresnel reflectance (Schlick's approximation)
    float fresnel_schlick(float cosTheta, float n1, float n2);
};

#endif 