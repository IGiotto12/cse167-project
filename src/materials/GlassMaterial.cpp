#define GLM_ENABLE_EXPERIMENTAL
#include "GlassMaterial.h"

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

using namespace glm;

Ray GlassMaterial::sample_ray_and_update_radiance(Ray &ray, Intersection &intersection) {
    /**
     * Calculate the next ray after intersection with the model.
     * This will be used for recursive ray tracing.
     */

    vec3 normal = intersection.normal;
    vec3 point = intersection.point;
    
    // Determine if we're entering or exiting the medium
    bool entering = dot(ray.dir, normal) < 0;
    
    // If exiting, flip the normal
    if (!entering) {
        normal = -normal;
    }
    
    // Indices of refraction
    float n1 = entering ? 1.0f : ior;  // From (air or glass)
    float n2 = entering ? ior : 1.0f;  // To (glass or air)
    
    // Calculate cosine of incident angle
    float cos_theta_i = -dot(ray.dir, normal);
    
    // Calculate Fresnel reflectance
    float fresnel = fresnel_schlick(cos_theta_i, n1, n2);
    
    // Decide whether to reflect or refract based on Fresnel and transparency
    float random = linearRand(0.0f, 1.0f);
    
    if (random > transparency || is_total_internal_reflection(ray.dir, normal, n1, n2)) {
        vec3 reflection_dir = normalize(ray.dir - 2.0f * dot(ray.dir, normal) * normal);
        
        ray.p0 = point + 0.001f * normal;  // Offset to avoid self-intersection
        ray.dir = reflection_dir;
        ray.is_diffuse_bounce = false;
        ray.n_bounces++;
        
        // Update radiance
        ray.W_wip = ray.W_wip * color;
        
        return ray;
    } else {
        // Refraction
        vec3 refraction_dir = refract_ray(ray.dir, normal, n1, n2);
        
        // Update ray
        ray.p0 = point - 0.001f * normal;  // Offset in opposite direction of normal
        ray.dir = refraction_dir;
        ray.is_diffuse_bounce = false;
        ray.n_bounces++;
        
        // Update radiance
        ray.W_wip = ray.W_wip * color;
        
        return ray;
    }
}

glm::vec3 GlassMaterial::get_direct_lighting(Intersection &intersection, Scene const &scene) {
    // Glass doesn't have direct lighting contribution, but primarily transmits and reflects light
    return glm::vec3(0.0f);
}

glm::vec3 GlassMaterial::color_of_last_bounce(Ray &ray, Intersection &intersection, Scene const &scene) {
    // Return the sky color tinted by the glass color for simplicity
    return ray.W_wip * color * scene.get_sky_color(ray);
}

bool GlassMaterial::is_total_internal_reflection(const glm::vec3 &incident, const glm::vec3 &normal, float n1, float n2) {

    float cos_theta_i = -dot(incident, normal);  
    float sin2_theta_t = (n1 / n2) * (n1 / n2) * (1.0f - cos_theta_i * cos_theta_i);
    
    // Total internal reflection occurs when sin2_theta_t > 1.0
    return sin2_theta_t > 1.0f;
}

glm::vec3 GlassMaterial::refract_ray(const glm::vec3 &incident, const glm::vec3 &normal, float n1, float n2) {
    float cos_theta_i = -dot(incident, normal);
    
    // Calculate refraction direction using Snell's law
    float eta = n1 / n2;
    float k = 1.0f - eta * eta * (1.0f - cos_theta_i * cos_theta_i);
    
    if (k < 0.0f) {
        return normalize(incident - 2.0f * dot(incident, normal) * normal);
    }
    
    return normalize(eta * incident + (eta * cos_theta_i - sqrt(k)) * normal);
}

float GlassMaterial::fresnel_schlick(float cos_theta, float n1, float n2) {
    // Calculate Fresnel reflectance using Schlick's approximation
    float r0 = (n1 - n2) / (n1 + n2);
    r0 = r0 * r0;
    
    return r0 + (1.0f - r0) * pow(1.0f - cos_theta, 5.0f);
} 