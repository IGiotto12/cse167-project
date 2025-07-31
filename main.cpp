#include <stdlib.h>

#include <chrono>
#include <iostream>
#include <thread>
// OSX systems need their own headers
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

// Use of degrees is deprecated. Use radians for GLM functions

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

// Custom Headers
#include "Image.h"
#include "RayTracer.h"
#include "Screenshot.h"

static const int width = 800;
static const int height = 600;
static const char *title = "Ray Tracer";
static const glm::vec4 background(0.1f, 0.2f, 0.3f, 1.0f);
static Image image(width, height);
static RayTracer rayTracer(width, height, image);
static bool cooldown_active = false;                   // to avoid unecessary display calls
std::chrono::steady_clock::time_point last_key_press;  // To track the last key press time

void printHelp() {
    std::cout << R"(
    Available commands:
      press 'H' to print this message again.
      press Esc to quit.

      Camera Controls:
      press 'O' to save a screenshot.
      press 'W'/'S' for front/back movement.
      press 'A'/'D' for left/right movement.
      press 'Q'/'E' for up/down movement.
      press the arrow keys to rotate camera.
      press 'Z'/'X' to rotate aroud view axis.
      press '-'/'+' to zoom (change of Fovy).
      press 'R' to reset camera.

      Shading Mode:
      press Spacebar to Ray Trace.
      press 'N' for Normal Shading.
      press 'P' for Debug Mode.
    
)";
}

bool is_integer(const std::string &str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isdigit(c) && c != '-' && c != '+') return false;
    }
    return true;
}

void lazy_draw_call(int _) {
    rayTracer.draw();
    cooldown_active = false;
}

void initialize(int argc, char **argv) {
    printHelp();
    glClearColor(background[0], background[1], background[2], background[3]);  // background color
    glViewport(0, 0, width, height);

    // Parse arguments
    int samples_per_pixel = std::atoi(argv[1]);
    int max_bounces = std::atoi(argv[2]);
    int scene_id = std::atoi(argv[3]);

    image.init();
    rayTracer.samples_per_pixel = samples_per_pixel;
    rayTracer.max_bounces = max_bounces;
    rayTracer.init(scene_id);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
}

void display(void) {
    if (!cooldown_active) {
        cooldown_active = true;
        glutTimerFunc(200, lazy_draw_call, 0);  // Set cooldown timer
    }
}

void saveScreenShot(const char *filename = "test.png") {
    int currentwidth = glutGet(GLUT_WINDOW_WIDTH);
    int currentheight = glutGet(GLUT_WINDOW_HEIGHT);
    Screenshot imag = Screenshot(currentwidth, currentheight);
    imag.save(filename);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:  // Escape to quit
            exit(0);
            break;
        case 'h':  // print help
            printHelp();
            break;
        case 'o':  // save screenshot
            saveScreenShot();
            break;

        // Translations
        case 'w':
            rayTracer.set_shading_mode(ShadingMode::NORMAL);
            rayTracer.camera.moveForward(0.2f);
            glutPostRedisplay();
            break;
        case 's':
            rayTracer.set_shading_mode(ShadingMode::NORMAL);
            rayTracer.camera.moveForward(-0.2f);
            glutPostRedisplay();
            break;
        case 'a':
            rayTracer.set_shading_mode(ShadingMode::NORMAL);
            rayTracer.camera.moveRight(-0.2f);
            glutPostRedisplay();
            break;
        case 'd':
            rayTracer.set_shading_mode(ShadingMode::NORMAL);
            rayTracer.camera.moveRight(0.2f);
            glutPostRedisplay();
            break;
        case 'q':
            rayTracer.set_shading_mode(ShadingMode::NORMAL);
            rayTracer.camera.moveUp(-0.2f);
            glutPostRedisplay();
            break;
        case 'e':
            rayTracer.set_shading_mode(ShadingMode::NORMAL);
            rayTracer.camera.moveUp(0.2f);
            glutPostRedisplay();
            break;

        // Roll
        case 'z':
            rayTracer.set_shading_mode(ShadingMode::NORMAL);
            rayTracer.camera.rotateRoll(10.0f);
            glutPostRedisplay();
            break;
        case 'x':
            rayTracer.set_shading_mode(ShadingMode::NORMAL);
            rayTracer.camera.rotateRoll(-10.0f);
            glutPostRedisplay();
            break;

        // Zoom
        case '-':
            rayTracer.set_shading_mode(ShadingMode::NORMAL);
            rayTracer.camera.zoom(0.1f);
            glutPostRedisplay();
            break;
        case '+':
            rayTracer.set_shading_mode(ShadingMode::NORMAL);
            rayTracer.camera.zoom(-0.1f);
            glutPostRedisplay();
            break;

        // Reset Camera
        case 'r':
            rayTracer.set_shading_mode(ShadingMode::NORMAL);
            rayTracer.camera.reset();
            glutPostRedisplay();
            break;

        // Shading modes
        case ' ':
            rayTracer.set_shading_mode(ShadingMode::RAY_TRACE);
            glutPostRedisplay();
            break;
        case 'n':
            rayTracer.set_shading_mode(ShadingMode::NORMAL);
            glutPostRedisplay();
            break;
        case 'p':
            rayTracer.set_shading_mode(ShadingMode::DEBUG);
            glutPostRedisplay();
            break;
        default:
            glutPostRedisplay();
            break;
    }
}

void specialKey(int key, int x, int y) {
    rayTracer.set_shading_mode(ShadingMode::NORMAL);
    switch (key) {
        case GLUT_KEY_UP:  // up
            rayTracer.camera.rotateUp(5.0f);
            glutPostRedisplay();
            break;
        case GLUT_KEY_DOWN:  // down
            rayTracer.camera.rotateUp(-5.0f);
            glutPostRedisplay();
            break;
        case GLUT_KEY_RIGHT:  // right
            rayTracer.camera.rotateRight(-5.0f);
            glutPostRedisplay();
            break;
        case GLUT_KEY_LEFT:  // left
            rayTracer.camera.rotateRight(5.0f);
            glutPostRedisplay();
            break;
    }
}

int main(int argc, char **argv) {
    // program has to be run with 3 int arguments
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <int: samples_per_pixel> <int: max_bounces> <int: scene_id>" << std::endl;
        return 1;
    }
    for (int i = 1; i < 4; ++i) {
        if (!is_integer(argv[i])) {
            std::cerr << "Error: Argument " << i << " ('" << argv[i] << "') is not a valid integer." << std::endl;
            return 1;
        }
    }

    // BEGIN CREATE WINDOW
    glutInit(&argc, argv);

#ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
#else
    glutInitContextVersion(3, 1);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
#endif
    glutInitWindowSize(width, height);
    glutCreateWindow(title);
#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
    }
#endif
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    // END CREATE WINDOW

    initialize(argc, argv);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);

    glutMainLoop();
    return 0; /* ANSI C requires main to return int. */
}
