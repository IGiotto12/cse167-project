/**************************************************
Camera is a class for a free-moving camera object.
*****************************************************/

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <glm/glm.hpp>
class Camera {
   public:
    glm::vec3 eye;           // camera position
    glm::vec3 target;        // look-at target (eye + forward)
    glm::vec3 up;            // up vector (may change with pitch/roll)
    float fovy;              // field of view in degrees
    float aspect;            // aspect ratio
    float near;              // near clipping distance
    float far;               // far clipping distance
    glm::mat4 cameraMatrix;  // camera's model matrix (for display or debugging)
    int width;
    int height;

    // default values for reset
    glm::vec3 eye_default = glm::vec3(0.0f, 0.0f, 5.0f);
    // set default target to be a unit step in the -Z direction from the eye:
    glm::vec3 target_default = glm::vec3(0.0f, 0.0f, 4.0f);
    glm::vec3 up_default = glm::vec3(0.0f, 1.0f, 0.0f);
    float fovy_default = 45.0f;
    float near_default = 0.01f;
    float far_default = 100.0f;

    Camera(int width, int height);

    // TRANSLATIONS (note: negative distance gives movement in the opposite direction)
    void moveForward(const float distance);
    void moveRight(const float distance);
    void moveUp(const float distance);

    // ROTATIONS (degrees)
    // rotateRight rotates (yaw) around the camera's current up vector.
    void rotateRight(const float degrees);
    // rotateUp rotates (pitch) around the camera's right vector.
    void rotateUp(const float degrees);
    // rotateRoll rotates around the camera's forward vector.
    void rotateRoll(const float degrees);

    // Implemented as a change in fovy.
    void zoom(const float factor);

    void computeMatrices(void);
    void reset(void);
};

#endif
