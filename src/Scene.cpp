#include <iostream>
#include <stack>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/random.hpp>

#include "ModelBase.h"
#include "Ray.h"
#include "Scene.h"

Scene::Scene(std::unique_ptr<Node> root_node) {
    /**
     * `rootNode` is a root node of transformation tree defining how geometries as
     * coverted to the desired scene. We need to process these tree to convert
     * Intial geometries into world view coordinates.
     */

    // Stacks for DFS
    // Each element in stack represents a node and cumulative trasformation matrix defined
    // by all nodes above current node in the tree
    std::stack<std::pair<Node *, glm::mat4>>
        node_stack;

    // start with root node
    node_stack.push(std::make_pair(root_node.get(), glm::mat4(1.0f)));

    while (!node_stack.empty()) {
        // get top node
        Node *curr_node = node_stack.top().first;
        glm::mat4 curr_mat = node_stack.top().second;
        node_stack.pop();

        // check if current node is leaf node
        if (curr_node->model.get() != nullptr) {
            // add to light sources if emmission is non zero
            if (curr_node->model->is_light_source()) {
                light_sources.push_back(curr_node->model.get());
            }

            // update transformation matrix of model
            curr_node->model->transformation_matrix = glm::mat4(curr_mat);
            curr_node->model->inverse_transform_matrix = glm::inverse(glm::mat4(curr_mat));
            models.push_back(std::move(curr_node->model));

            continue;
        }

        // iterate child nodes and update stack
        for (unsigned int idx = 0; idx < curr_node->childnodes.size(); idx++) {
            // calculate cummulative transformation matrix
            glm::mat4 cumm_mat = curr_mat * curr_node->childtransforms[idx];

            // update stack
            node_stack.push(std::make_pair(curr_node->childnodes[idx].get(), cumm_mat));
        }
    }
}

Ray Scene::intersect(Ray &ray) const {
    using namespace glm;
    /**
     * For each model in the scene, we will check if the ray intersects
     * the model. If it does, we will update the ray with the intersection
     * details.
     */

    ray.intersections.clear();

    for (unsigned int idx = 0; idx < models.size(); idx++) {
        // ray is updated with just intersection details
        models[idx]->intersect(ray);
    }

    // Select the closest intersection
    // This section assumes that ray intesections behind the camera are
    // already filtered out by the `model->intersect()` function
    Intersection intersection;
    intersection.t = std::numeric_limits<float>::max();
    for (unsigned int idx = 0; idx < ray.intersections.size(); idx++) {
        if (ray.intersections[idx].t < intersection.t)
            intersection = ray.intersections[idx];
    }

    // update color based on intersection
    if (ray.intersections.size() > 0) {
        // if normal shading is on
        if (this->shading_mode == ShadingMode::NORMAL) {
            // just return normal as color
            ray.color = RGB_to_Linear(0.4f * intersection.normal + 0.6f);
            ray.isWip = false;
            return ray;
        }

        if (intersection.model->is_light_source()) {
            if (ray.is_diffuse_bounce)
                // ignore to avoid double counting
                ray.color = vec3(0.0f);
            else
                ray.color = ray.W_wip * intersection.model->material->emission;

            ray.isWip = false;  // no further processing
            return ray;
        }

        // update color for nth last bounce
        ray.color = intersection.model->material->color_of_last_bounce(ray, intersection, *this);

        // Russian Roulette Implementation
        // Parameter lambda - probability of terminating the path
        const float lambda = 0.5f;
        float random = linearRand(0.0f, 1.0f);
        
        // Terminate the path
        if (random < lambda) {
            ray.color = ray.color / (1.0f - lambda);
            ray.isWip = false;
            return ray;
        }
        ray.W_wip = ray.W_wip / (1.0f - lambda);

        // This function will give out next ray and
        // update wip color params as well
        ray = intersection.model->material->sample_ray_and_update_radiance(ray, intersection);

        return ray;
    }

    // if no intersection, update color with sky color`
    ray.W_wip = ray.W_wip * this->get_sky_color(ray);  // sky color
    ray.isWip = false;                                 // no further processing
    ray.color = ray.W_wip;
    return ray;
}

glm::vec3 Scene::get_sky_color(Ray &ray) const {
    /**
     * This function maps a ray pointing towards sky in given direction
     * to a vertical gradient between two colors
     */

    using namespace glm;

    // colors for gradient
    vec3 start_color = RGB_to_Linear(vec3(0.227f, 0.392f, 1.0f));
    vec3 end_color = RGB_to_Linear(vec3(0.9f));

    // linear interpolate based on y coordinate
    float alpha = 0.5f * (ray.dir.y + 1.0f);
    return (1 - alpha) * start_color + alpha * end_color;
}