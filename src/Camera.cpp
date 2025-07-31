#define GLM_ENABLE_EXPERIMENTAL
#include "Camera.h"

#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

// --- Quaternion-based rotation functions ---

// Quaternion multiplication p * q
glm::vec4 qmultiply(const glm::vec4 p, const glm::vec4 q) {
    const float p_re = p.w;
    const float q_re = q.w;
    const glm::vec3 p_im(p.x, p.y, p.z);
    const glm::vec3 q_im(q.x, q.y, q.z);
    float r_re = p_re * q_re - glm::dot(p_im, q_im);
    glm::vec3 r_im = p_re * q_im + q_re * p_im + glm::cross(p_im, q_im);
    glm::vec4 r = glm::vec4(r_im, r_re);
    return r;
}

// Quaternion conjugation
glm::vec4 qconj(const glm::vec4 q) {
    return glm::vec4(-q.x, -q.y, -q.z, q.w);
}

// Returns a 3x3 rotation matrix for a rotation of “degrees” about “axis”.
glm::mat3 rotation(const float degrees, const glm::vec3 axis) {
    const float angle = degrees * M_PI / 180.0f;  // convert to radians
    const glm::vec3 a = glm::normalize(axis);
    glm::mat3 R;
    glm::vec4 q = glm::vec4(glm::sin(0.5f * angle) * a, glm::cos(0.5f * angle));
    glm::vec4 ii(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 jj(0.0f, 1.0f, 0.0f, 0.0f);
    glm::vec4 kk(0.0f, 0.0f, 1.0f, 0.0f);
    R[0] = glm::vec3(qmultiply(q, qmultiply(ii, qconj(q))));
    R[1] = glm::vec3(qmultiply(q, qmultiply(jj, qconj(q))));
    R[2] = glm::vec3(qmultiply(q, qmultiply(kk, qconj(q))));
    return R;
}

// --- Camera Methods ---

Camera::Camera(int width, int height) : width(width), height(height) {
    aspect = (float)width / height;  // aspect ratio
    reset();
    computeMatrices();
}

// TRANSLATIONS: update both the eye and the target so that the view direction remains unchanged.

void Camera::moveForward(const float distance) {
    glm::vec3 forward = glm::normalize(target - eye);
    glm::vec3 moveVector = forward * distance;
    eye += moveVector;
    target += moveVector;
}

void Camera::moveRight(const float distance) {
    glm::vec3 forward = glm::normalize(target - eye);
    glm::vec3 right = glm::normalize(glm::cross(forward, up));
    glm::vec3 moveVector = right * distance;
    eye += moveVector;
    target += moveVector;
}

void Camera::moveUp(const float distance) {
    glm::vec3 moveVector = glm::normalize(up) * distance;
    eye += moveVector;
    target += moveVector;
}

// ROTATIONS: instead of orbiting around a fixed target, update the forward vector.
// (Negative degree values can be used for the opposite rotation.)

// rotateRight (yaw): rotate the forward vector around the current up axis.
void Camera::rotateRight(const float degrees) {
    glm::vec3 forward = glm::normalize(target - eye);
    glm::mat3 R = rotation(degrees, up);
    forward = R * forward;
    target = eye + forward;
}

// rotateUp (pitch): rotate around the camera's right axis.
void Camera::rotateUp(const float degrees) {
    glm::vec3 forward = glm::normalize(target - eye);
    glm::vec3 right = glm::normalize(glm::cross(forward, up));
    glm::mat3 R = rotation(degrees, right);
    forward = R * forward;
    up = glm::normalize(R * up);  // update up vector accordingly
    target = eye + forward;
}

// rotateRoll: rotate the up vector around the forward (view) direction.
void Camera::rotateRoll(const float degrees) {
    glm::vec3 forward = glm::normalize(target - eye);
    glm::mat3 R = rotation(degrees, forward);
    up = glm::normalize(R * up);
}

// Here we modify the field-of-view angle.
void Camera::zoom(const float factor) {
    fovy += factor;
}

// computeMatrices() is left similar to your original implementation.
// (In a full application you might compute the view matrix as the inverse of the camera’s transform.)
void Camera::computeMatrices() {
    // Compute camera basis using the free camera’s current orientation.
    glm::vec3 forward = glm::normalize(target - eye);
    glm::vec3 right = glm::normalize(glm::cross(forward, up));
    glm::vec3 trueUp = glm::normalize(glm::cross(right, forward));

    // Note that glm matrices are column-major.
    cameraMatrix[0] = glm::vec4(right, 0.0f);
    cameraMatrix[1] = glm::vec4(trueUp, 0.0f);
    // Use negative forward so that the camera looks down -Z in its view matrix.
    cameraMatrix[2] = glm::vec4(-forward, 0.0f);
    cameraMatrix[3] = glm::vec4(eye, 1.0f);
}

void Camera::reset() {
    eye = eye_default;
    target = target_default;
    up = up_default;
    fovy = fovy_default;
    near = near_default;
    far = far_default;
}
