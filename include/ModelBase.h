#ifndef __GEOMETRY_BASE_H__
#define __GEOMETRY_BASE_H__
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "GeometryBase.h"
#include "MaterialBase.h"
#include "Ray.h"

using namespace glm;

class ModelBase {
   public:
    // List of geometries composing the given model
    std::vector<std::unique_ptr<GeometryBase>> geometries;
    std::shared_ptr<MaterialBase> material;
    glm::mat4 transformation_matrix;
    glm::mat4 inverse_transform_matrix;

    ModelBase() = default;

    virtual ~ModelBase() = default;

    virtual bool intersect(Ray &ray) {
        bool isIntersect = false;

        // Cache ray parameters
        vec3 p0(ray.p0);
        vec3 dir(ray.dir);

        // Transform Ray to model space
        ray.p0 = vec3(inverse_transform_matrix * vec4(ray.p0, 1.0f));
        ray.dir = normalize(vec3(inverse_transform_matrix * vec4(ray.dir, 0.0f)));

        for (int idx = 0; idx < geometries.size(); idx++) {
            GeometryBase *prim = geometries[idx].get();  // get the raw pointer

            // intersections in model space
            std::vector<Intersection> intersections = prim->intersect(ray);

            // Transform intersection points to world space
            for (int i = 0; i < intersections.size(); i++) {
                intersections[i].model = this;
                intersections[i].point = vec3(transformation_matrix * vec4(intersections[i].point, 1.0f));
                intersections[i].normal = normalize(vec3(transpose(inverse_transform_matrix) * vec4(intersections[i].normal, 0.0f)));
                intersections[i].t = length(intersections[i].point - p0);
            }

            // Update ray intersections
            ray.intersections.insert(ray.intersections.end(), intersections.begin(), intersections.end());

            // Update isIntersect
            isIntersect = isIntersect || !intersections.empty();
        }

        // Restore ray parameters
        ray.p0 = p0;
        ray.dir = dir;

        return isIntersect;
    }

    virtual glm::vec3 get_surface_point() = 0;

    virtual bool is_light_source() {
        return glm::length(material->emission) > 0;
    }
};

#endif