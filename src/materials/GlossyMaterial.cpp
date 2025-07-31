#define GLM_ENABLE_EXPERIMENTAL
#include "GlossyMaterial.h"

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

using namespace glm;

Ray GlossyMaterial::sample_ray_and_update_radiance(Ray &ray, Intersection &intersection) {
    /**
     * Calculate the next ray after intersection with the model.
     * This will be used for recursive ray tracing.
     */

    // Decide if diffuse or specular reflection
    float random = linearRand(0.0f, 1.0f);
    vec3 normal = intersection.normal;
    vec3 point = intersection.point;

    // Diffuse reflection
    if (random > shininess) {
        // Step 1: Sample ray direction
        /**
         * TODO: Task 6.1
         * Implement cosine-weighted hemisphere sampling
         */
        // cosin sample next ray
        float s = linearRand(0.0f, 1.0f);
        float t = linearRand(0.0f, 1.0f);

        // TODO: Update u, v based on Equation (8) in handout
        float u = sqrt(s);
        float v = 2.0f * M_PI * t;

        vec3 hemisphere_sample = vec3(
            u * cos(v),
            u * sin(v),
            sqrt(1.0f - s)
        );

        // The direction we sampled above is in local co-ordinate frame
        // we need to align it with the surface normal
        vec3 new_dir = align_with_normal(hemisphere_sample, normal);

        // Step 2: Calculate radiance
        /**
         * TODO: Task 6.1
         * Note:
         * - C_diffuse = `this->diffuse`
         */
        vec3 W_diffuse = diffuse;  // TODO: Calculate the radiance for current bounce


        ray.W_wip = ray.W_wip * W_diffuse;

        ray.W_wip = ray.W_wip * diffuse;
        

        ray.p0 = point + 0.001f * normal; 
        ray.dir = new_dir;
        ray.is_diffuse_bounce = true;
        ray.n_bounces++;
        
        return ray;
    }
    
    // Specular reflection
    
    // Step 1: Calculate reflection direction
    /**
     * TODO: Task 6.2
     * Calculate the perfect mirror reflection direction
     */
    vec3 reflection_dir = normalize(ray.dir - 2.0f * dot(ray.dir, normal) * normal);

    // Step 2: Calculate radiance
    /**
     * TODO: Task 6.2
     * Note:
     * - C_specular = `this->specular`
     */
    vec3 W_specular = specular;

    // update radiance
    ray.W_wip = ray.W_wip * W_specular;
    ray.p0 = point + 0.001f * normal;  // offset point slightly to avoid self intersection
    ray.dir = reflection_dir;
    ray.is_diffuse_bounce = false;
    ray.n_bounces++;

    return ray;
}

glm::vec3 GlossyMaterial::get_direct_lighting(Intersection &intersection, Scene const &scene) {
    using namespace glm;

    /**
     * Note:
     * - Light sources from scene can be accessed by `scene.light_sources`
     * - Models from scene can be accessed by `scene.models`
     */

    // Iterate over all light sources
    vec3 cummulative_direct_light = vec3(0.0f);
    for (unsigned int idx = 0; idx < scene.light_sources.size(); idx++) {
        // intersection could be with one of the light source to
        // so skip self intersection
        if (scene.light_sources[idx] == intersection.model)
            continue;

        // get light source position
        vec3 light_pos = scene.light_sources[idx]->get_surface_point();

        // check if point is in shadow
        /**
         * TODO: Task 4.1
         * Shoot a shadow ray towards light source
         * Note:
         * - Offset ray starting position by small amount to avoid self shadowing
         * - Use `light_pos` and `intersection.point` to get direction for shadow ray
         * - Surface normal at point of intersection is stored in `intersection.normal`
         */
        Ray shadow_ray;
        // shadow_ray.p0 = vec3(0.0f);   // TODO: Update ray start position here
        // shadow_ray.dir = vec3(0.0f);  // TODO: Update ray direction here
        const float EPSILON = 0.0001f;
        shadow_ray.p0 = intersection.point + EPSILON * intersection.normal;
        
        vec3 light_direction = normalize(light_pos - intersection.point);
        shadow_ray.dir = light_direction;

        // check if shadow ray intersects any model
        for (unsigned int idx = 0; idx < scene.models.size(); idx++)
            scene.models[idx]->intersect(shadow_ray);

        // get closest intersection
        Intersection closest_intersection;
        closest_intersection.t = std::numeric_limits<float>::max();
        for (unsigned int idx = 0; idx < shadow_ray.intersections.size(); idx++) {
            if (shadow_ray.intersections[idx].t < closest_intersection.t)
                closest_intersection = shadow_ray.intersections[idx];
        }

        // check if light source is visible
        if (closest_intersection.model == scene.light_sources[idx]) {
            // light source emission value
            vec3 light_emission = scene.light_sources[idx]->material->emission;

            /**
             * TODO: Task 4.1
             * Calculate direct light contribution using lamberts cosine law.
             * Refer Equation (3) in final project handout for more details
             *
             * NOTE:
             * - Surface normal at point of intersection is stored in `intersection.normal`
             * - This `if` condition block takes care of `visibility_of_light` part in the equation
             *   So here you just need to calculate contribution of light like we did in HW3 for diffuse part
             */
            float cos_theta = max(dot(intersection.normal, light_direction), 0.0f);
            // vec3 direct_light = vec3(0.0f);  // TODO: Update direct light constribution of light source
            vec3 direct_light = diffuse * light_emission * cos_theta;

            // attenuation factor for light source based on distance
            float attenuation_factor = scene.light_sources[idx]->material->get_light_attenuation_factor(closest_intersection.t);

            cummulative_direct_light += direct_light / attenuation_factor;
        }
    }

    return cummulative_direct_light;
}

vec3 GlossyMaterial::color_of_last_bounce(Ray &ray, Intersection &intersection, Scene const &scene) {
    using namespace glm;
    /**
     * Color after last bounce will be `W_wip * last_bounce_color`
     * We shade last bounce for this Glossy material using direct diffuse lighting
     */

    vec3 direct_diff_light = this->get_direct_lighting(intersection, scene);

    return ray.W_wip * diffuse * (1 - shininess) * direct_diff_light;
}