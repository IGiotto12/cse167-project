#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "GlassMaterial.h"
#include "GlossyMaterial.h"
#include "Obj.h"
#include "RayTracer.h"
#include "Sphere.h"
#include "Square.h"

using namespace glm;


/**
 * NOTE: Robot Scene
 */
Scene* robot() {    
    // Materials - Walls
    std::shared_ptr<GlossyMaterial> white_wall_material = std::make_shared<GlossyMaterial>(vec3(0.99f), vec3(0.0f), 0.0);
    std::shared_ptr<GlossyMaterial> pink_wall_material = std::make_shared<GlossyMaterial>(vec3(0.95f, 0.5f, 0.7f), vec3(0.0f), 0.0);
    std::shared_ptr<GlossyMaterial> blue_wall_material = std::make_shared<GlossyMaterial>(vec3(0.5f, 0.6f, 0.95f), vec3(0.0f), 0.0);
    
    // Mirror-like material
    std::shared_ptr<GlossyMaterial> mirror_material = std::make_shared<GlossyMaterial>(
        vec3(0.1f, 0.1f, 0.1f),
        vec3(0.95f, 0.95f, 0.95f),
        0.95f
    );
    
    // Robot head
    std::shared_ptr<GlassMaterial> head_material = std::make_shared<GlassMaterial>(
        vec3(0.6f, 0.6f, 0.7f),
        1.0f,
        0.5f
    );
    
    // Eyeball material
    std::shared_ptr<GlossyMaterial> eyeball_material = std::make_shared<GlossyMaterial>(
        vec3(1.0f, 1.0f, 1.0f), 
        vec3(0.5f, 0.5f, 0.5f),
        0.3f
    );
    
    // Glass material for the platform
    std::shared_ptr<GlassMaterial> glass_material = std::make_shared<GlassMaterial>(
        vec3(0.9f, 0.95f, 1.0f),
        1.5f,
        0.90f
    );
    
    // Light materials
    std::shared_ptr<GlossyMaterial> ceiling_light_material = std::make_shared<GlossyMaterial>(vec3(0.0f), vec3(0.0f), 0.5);
    ceiling_light_material->convert_to_light(vec3(1.0f), vec3(60.0f));  // Increased intensity for larger room with fewer lights
    
    std::shared_ptr<GlossyMaterial> floor_light_material = std::make_shared<GlossyMaterial>(vec3(0.0f), vec3(0.0f), 0.5);
    floor_light_material->convert_to_light(vec3(0.9f, 0.8f, 0.7f), vec3(45.0f));

    std::unique_ptr<Node> root_node = std::make_unique<Node>();

    // Floor
    std::unique_ptr<Node> bottom_square = std::make_unique<Node>();
    bottom_square->model = std::make_unique<Square>(vec3(0.0f, 0.0f, 0.0f), 8.0f, vec3(0.0f, 1.0f, 0.0f), white_wall_material);
    root_node->childnodes.push_back(std::move(bottom_square));
    root_node->childtransforms.push_back(translate(vec3(0.0f, -3.0f, 0.0f)));

    // Right wall
    std::unique_ptr<Node> right_square = std::make_unique<Node>();
    right_square->model = std::make_unique<Square>(vec3(0.0f, 0.0f, 0.0f), 8.0f, vec3(-1.0f, 0.0f, 0.0f), blue_wall_material);
    root_node->childnodes.push_back(std::move(right_square));
    root_node->childtransforms.push_back(translate(vec3(4.0f, 0.0f, 0.0f)));

    // Left wall
    std::unique_ptr<Node> left_square = std::make_unique<Node>();
    left_square->model = std::make_unique<Square>(vec3(0.0f, 0.0f, 0.0f), 8.0f, vec3(1.0f, 0.0f, 0.0f), pink_wall_material);
    root_node->childnodes.push_back(std::move(left_square));
    root_node->childtransforms.push_back(translate(vec3(-4.0f, 0.0f, 0.0f)));

    // Ceiling
    std::unique_ptr<Node> up_square = std::make_unique<Node>();
    up_square->model = std::make_unique<Square>(vec3(0.0f, 0.0f, 0.0f), 8.0f, vec3(0.0f, -1.0f, 0.0f), white_wall_material);
    root_node->childnodes.push_back(std::move(up_square));
    root_node->childtransforms.push_back(translate(vec3(0.0f, 4.0f, 0.0f)));

    // Back wall
    std::unique_ptr<Node> back_square = std::make_unique<Node>();
    back_square->model = std::make_unique<Square>(vec3(0.0f, 0.0f, 0.0f), 8.0f, vec3(0.0f, 0.0f, 1.0f), white_wall_material);
    root_node->childnodes.push_back(std::move(back_square));
    root_node->childtransforms.push_back(translate(vec3(0.0f, 0.0f, -4.0f)));

    // Ceiling light 
    std::unique_ptr<Node> ceiling_light = std::make_unique<Node>();
    ceiling_light->model = std::make_unique<Square>(vec3(0.0f, 0.0f, 0.0f), 1.2f, vec3(0.0f, -1.0f, 0.0f), ceiling_light_material);
    root_node->childnodes.push_back(std::move(ceiling_light));
    root_node->childtransforms.push_back(translate(vec3(0.0f, 3.95f, 0.0f)));
    
    // Front left floor light
    std::unique_ptr<Node> floor_light1 = std::make_unique<Node>();
    floor_light1->model = std::make_unique<Square>(vec3(0.0f, 0.0f, 0.0f), 0.5f, vec3(0.0f, 1.0f, 0.0f), floor_light_material);
    root_node->childnodes.push_back(std::move(floor_light1));
    root_node->childtransforms.push_back(translate(vec3(-2.0f, -2.95f, 2.0f)));
    
    // Front right corner
    std::unique_ptr<Node> floor_light2 = std::make_unique<Node>();
    floor_light2->model = std::make_unique<Square>(vec3(0.0f, 0.0f, 0.0f), 0.5f, vec3(0.0f, 1.0f, 0.0f), floor_light_material);
    root_node->childnodes.push_back(std::move(floor_light2));
    root_node->childtransforms.push_back(translate(vec3(2.0f, -2.95f, 2.0f)));
    
    // Back left corner
    std::unique_ptr<Node> floor_light3 = std::make_unique<Node>();
    floor_light3->model = std::make_unique<Square>(vec3(0.0f, 0.0f, 0.0f), 0.5f, vec3(0.0f, 1.0f, 0.0f), floor_light_material);
    root_node->childnodes.push_back(std::move(floor_light3));
    root_node->childtransforms.push_back(translate(vec3(-2.0f, -2.95f, -2.0f)));
    
    // Back right corner
    std::unique_ptr<Node> floor_light4 = std::make_unique<Node>();
    floor_light4->model = std::make_unique<Square>(vec3(0.0f, 0.0f, 0.0f), 0.5f, vec3(0.0f, 1.0f, 0.0f), floor_light_material);
    root_node->childnodes.push_back(std::move(floor_light4));
    root_node->childtransforms.push_back(translate(vec3(2.0f, -2.95f, -2.0f)));
    
    // Glass platform under the robot
    std::unique_ptr<Node> glass_platform = std::make_unique<Node>();
    glass_platform->model = std::make_unique<Square>(vec3(0.0f, 0.0f, 0.0f), 2.5f, vec3(0.0f, 1.0f, 0.0f), glass_material);
    root_node->childnodes.push_back(std::move(glass_platform));
    root_node->childtransforms.push_back(translate(vec3(0.0f, -2.95f, 0.0f)));
    
    // Robot body
    std::unique_ptr<Node> robot_body = std::make_unique<Node>();
    robot_body->model = std::make_unique<Sphere>(0.8f, vec3(0.0f, 0.0f, 0.0f), mirror_material);
    root_node->childnodes.push_back(std::move(robot_body));
    root_node->childtransforms.push_back(
        translate(vec3(0.0f, -2.0f, 0.0f)) *
        scale(vec3(1.2f, 1.3f, 1.2f))
    );
    
    // Robot head
    std::unique_ptr<Node> robot_head = std::make_unique<Node>();
    robot_head->model = std::make_unique<Sphere>(0.15f, vec3(0.0f, 0.0f, 0.0f), head_material);
    root_node->childnodes.push_back(std::move(robot_head));
    root_node->childtransforms.push_back(translate(vec3(0.0f, -0.8f, 0.0f)));
    
    // Left eyeball
    std::unique_ptr<Node> left_eye = std::make_unique<Node>();
    left_eye->model = std::make_unique<Sphere>(0.08f, vec3(0.0f, 0.0f, 0.0f), eyeball_material);
    root_node->childnodes.push_back(std::move(left_eye));
    root_node->childtransforms.push_back(
        translate(vec3(-0.3f, -0.4f, 0.85f)) *
        scale(vec3(0.15f, 0.15f, 0.15f))
    );
    
    // Right eyeball
    std::unique_ptr<Node> right_eye = std::make_unique<Node>();
    right_eye->model = std::make_unique<Sphere>(0.08f, vec3(0.0f, 0.0f, 0.0f), eyeball_material);
    root_node->childnodes.push_back(std::move(right_eye));
    root_node->childtransforms.push_back(
        translate(vec3(0.3f, -0.4f, 0.85f)) *
        scale(vec3(0.15f, 0.15f, 0.15f))
    );
    
    // Initialize the scene
    return new Scene(std::move(root_node));
}