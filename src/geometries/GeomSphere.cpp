#include "GeomSphere.h"

#include <iostream>
#include <utility>

#include "Intersection.h"
#include "Ray.h"

std::vector<Intersection> GeomSphere::intersect(Ray &ray) {
    /**
     * NOTE: Ray is already transformed to the Model coordinate space.
     */
    using namespace glm;

    // vector to store the intersections
    std::vector<Intersection> intersections;

    /**
     * TODO: Implement the Ray intersection with the current geometry
     */

    /**
     * Once you find the intersection, add it to the `intersections` vector.
     *
     * Example:
     * Suppose the ray intersects the current geometry at a point `vec3 point`
     * at a distance `float t`, and the unit normal vector at the intersection
     * point is `vec3 normal`. You would then insert an intersection into the
     * vector as follows:
     *
     * intersections.push_back({t, point, normal, this, nullptr});
     *
     * Note:
     * - Here we pass the Model pointer as `nullptr` because it will be
     *   populated by the Model::intersect() function call.
     * - Only add intersections that are in front of the camera, i.e.,
     *   where t > 0.
     */
    vec3 center(0.0f, 0.0f, 0.0f);
    float radius = 1.0f;
    

    vec3 origin = ray.p0;
    vec3 direction = ray.dir;
    
    vec3 origin_center = origin - center;
    float a = dot(direction, direction); 
    float b = 2.0f * dot(origin_center, direction);
    float c = dot(origin_center, origin_center) - radius * radius;
    

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return intersections;
    }
    

    float t1 = (-b - sqrt(discriminant)) / (2 * a);
    float t2 = (-b + sqrt(discriminant)) / (2 * a);

    /**
     * TODO: Update `intersections`
     */

    if (t1 > 0) {        
        vec3 point = origin + t1 * direction;
        vec3 normal = normalize(point - center);
        intersections.push_back({t1, point, normal, this, nullptr});
    }
    
    if (t2 > 0) {
        vec3 point = origin + t2 * direction;
        vec3 normal = normalize(point - center);
        intersections.push_back({t2, point, normal, this, nullptr});
    }

    return intersections;
};