#include "Tetrahedron.h"

#include <glm/gtc/random.hpp>
#include <iostream>

#include "GeomTriangle.h"

using namespace glm;

Tetrahedron::Tetrahedron(float side_len, std::shared_ptr<MaterialBase> mat) {
    this->side_len = side_len;

    // Material Object can be shared between multiple geometries
    material = mat;

    // XY Plane Triangle
    std::vector<vec3> xy_vertices = {vec3(0.0f, 0.0f, 0.0f),
                                     vec3(0.0f, side_len, 0.0f),
                                     vec3(side_len, 0.0f, 0.0f)};
    vec3 xy_normal = normalize(cross(xy_vertices[1] - xy_vertices[0], xy_vertices[2] - xy_vertices[0]));
    std::vector<vec3>
        xy_normals = {vec3(xy_normal),
                      vec3(xy_normal),
                      vec3(xy_normal)};

    std::unique_ptr<GeomTriangle>
        xy_triangle = std::make_unique<GeomTriangle>(xy_vertices, xy_normals);

    // XZ Plane Triangle
    std::vector<vec3> xz_vertices = {vec3(0.0f, 0.0f, 0.0f),
                                     vec3(side_len, 0.0f, 0.0f),
                                     vec3(0.0f, 0.0f, side_len)};
    vec3 xz_normal = normalize(cross(xz_vertices[1] - xz_vertices[0], xz_vertices[2] - xz_vertices[0]));
    std::vector<vec3> xz_normals = {vec3(xz_normal),
                                    vec3(xz_normal),
                                    vec3(xz_normal)};
    std::unique_ptr<GeomTriangle>
        xz_triangle = std::make_unique<GeomTriangle>(xz_vertices, xz_normals);

    // YZ Plane Triangle
    std::vector<vec3> yz_vertices = {vec3(0.0f, 0.0f, 0.0f),
                                     vec3(0.0f, 0.0f, side_len),
                                     vec3(0.0f, side_len, 0.0f)};
    vec3 yz_normal = normalize(cross(yz_vertices[1] - yz_vertices[0], yz_vertices[2] - yz_vertices[0]));
    std::vector<vec3> yz_normals = {vec3(yz_normal),
                                    vec3(yz_normal),
                                    vec3(yz_normal)};
    std::unique_ptr<GeomTriangle>
        yz_triangle = std::make_unique<GeomTriangle>(yz_vertices, yz_normals);

    // XYZ Plane Triangle
    std::vector<vec3> xyz_vertices = {vec3(side_len, 0.0f, 0.0f),
                                      vec3(0.0f, side_len, 0.0f),
                                      vec3(0.0f, 0.0f, side_len)};
    vec3 xyz_normal = normalize(cross(xyz_vertices[1] - xyz_vertices[0], xyz_vertices[2] - xyz_vertices[0]));
    std::vector<vec3> xyz_normals = {vec3(xyz_normal),
                                     vec3(xyz_normal),
                                     vec3(xyz_normal)};
    std::unique_ptr<GeomTriangle>
        xyz_triangle = std::make_unique<GeomTriangle>(xyz_vertices, xyz_normals);

    // Add to geometries
    geometries.push_back(std::move(xy_triangle));
    geometries.push_back(std::move(xz_triangle));
    geometries.push_back(std::move(yz_triangle));
    geometries.push_back(std::move(xyz_triangle));
}
