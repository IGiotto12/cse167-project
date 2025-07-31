#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <atomic>
#include <condition_variable>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <thread>
#include <vector>

#include "./scenes/open_sky.inl"
#include "./scenes/sphere_in_box.inl"
#include "./scenes/teapot_in_box.inl"
#include "./scenes/robot.inl"
#include "Image.h"
#include "ProgressBar.h"
#include "Ray.h"
#include "RayTracer.h"
#include "Scene.h"
#include "Sphere.h"


using namespace glm;

void RayTracer::init(int scene_id) {
    // Initialize the scene
    if (scene_id == 1)
        scene = std::unique_ptr<Scene>(sphere_in_box());
    else if (scene_id == 2)
        scene = std::unique_ptr<Scene>(teapot_in_box());
    else if (scene_id == 3)
        scene = std::unique_ptr<Scene>(open_sky());
    else if (scene_id == 4)
        scene = std::unique_ptr<Scene>(robot());
    else {
        std::cout << "Invalid <scene_id> " << scene_id << " given" << std::endl;
        exit(1);
    }
}

Ray RayTracer::ray_thru_pixel(int i, int j) {
    /**
     * This function generated a ray passing through camera origin
     * and pixel (i, j)
     */

    Ray ray;
    ray.pixel_x_coordinate = i;
    ray.pixel_y_coordinate = j;

    // p0
    ray.p0 = glm::vec3(camera.eye);

    /**
     * TODO: Task 1.2
     * Randomly sample x and y inside pixel(i, j)
     */
    float x = glm::linearRand(0.0f, 1.0f);
    float y = glm::linearRand(0.0f, 1.0f);

    /**
     * TODO: Task 1.1
     * calculate and assign direction to ray which is passoing
     * through current pixel (i, j)
    //  */
    // float alpha = 0.0f;  // TODO: Implement this
    // float beta = 0.0f;   // TODO: Implement this
    float alpha = (2.0f * ((x + i) / camera.width) - 1.0f) * camera.aspect;
    float beta = 1.0f - 2.0f * ((y + j) / camera.height);

    vec3 u(camera.cameraMatrix[0]);
    vec3 v(camera.cameraMatrix[1]);
    vec3 w(camera.cameraMatrix[2]);

    // ray.dir = vec3(-1.0f);  // TODO: Implement this
    ray.dir = normalize(-w + beta * v + alpha * u);

    return ray;
}

void RayTracer::set_shading_mode(ShadingMode shading_mode) {
    // Update shading mode for both ray tracer and scene
    this->shading_mode = shading_mode;
    this->scene->shading_mode = shading_mode;
}

void RayTracer::refresh_display(std::mutex& imageMutex) {
    // This has to happen on main thread
    std::unique_lock<std::mutex> image_lock(imageMutex);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    image.draw();
    glutSwapBuffers();
    glFlush();
}

// --------------------- Consumer Thread Callback ---------------------
void RayTracer::trace_ray_thread_callback(
    std::queue<Ray>& wip_queue,
    Image& image,
    Scene& scene,
    std::mutex& queueMutex,
    std::mutex& imageMutex,
    std::condition_variable& cond_var,
    int active_samples_per_pixel,
    unsigned int& processed_ray_count,
    std::atomic<unsigned int>& active_rays) {
    while (true) {
        std::unique_lock<std::mutex> queue_lock(queueMutex);
        // Wait until there is work available or no rays remain in-flight.
        cond_var.wait(queue_lock, [&]() {
            return !wip_queue.empty() || active_rays.load() == 0;
        });

        // Terminate if no rays remain
        if (wip_queue.empty() && active_rays.load() == 0)
            break;

        // Retrieve next ray
        Ray ray = wip_queue.front();
        wip_queue.pop();
        queue_lock.unlock();

        // Process the ray
        ray = scene.intersect(ray);

        // Update the pixel color
        {
            std::unique_lock<std::mutex> image_lock(imageMutex);
            glm::vec3 ray_color = (this->shading_mode == ShadingMode::DEBUG)
                                      ? ray.debug_color
                                      : ray.color;
            glm::vec3 color = image.getPixel(ray.pixel_x_coordinate, ray.pixel_y_coordinate) +
                              ray_color / static_cast<float>(active_samples_per_pixel);
            image.updatePixel(ray.pixel_x_coordinate, ray.pixel_y_coordinate, color);
            processed_ray_count++;
        }

        queue_lock.lock();
        // If the ray should bounce, re-enqueue it without modifying active_rays.
        if (ray.n_bounces < active_max_bounces && ray.isWip) {
            wip_queue.push(ray);
            cond_var.notify_one();
        } else {
            // Ray is finished: decrement active_rays.
            active_rays--;
            cond_var.notify_all();  // wake up waiting threads in case this was the last ray.
        }
        queue_lock.unlock();
    }
}

// --------------------- Producer Thread Callback ---------------------
void RayTracer::init_rays_thread_callback(
    std::queue<Ray>& wip_queue,
    std::mutex& queueMutex,
    std::condition_variable& cond_var,
    std::atomic<unsigned int>& active_rays) {
    // Generate rays for every sample and pixel.
    for (int s = 0; s < active_samples_per_pixel; s++) {
        for (int i = 0; i < camera.width; i++) {
            for (int j = 0; j < camera.height; j++) {
                Ray ray = ray_thru_pixel(i, j);
                while (true) {
                    std::unique_lock<std::mutex> queue_lock(queueMutex);
                    if (wip_queue.size() < 1e6) {
                        wip_queue.push(ray);
                        active_rays++;          // Increment counter for each new ray.
                        cond_var.notify_one();  // Notify that new work is available.
                        break;
                    }
                    queue_lock.unlock();
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
        }
    }
    // Notify in case any threads are waiting and active_rays==0.
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        cond_var.notify_all();
    }
}

// --------------------- Main Draw Function ---------------------
void RayTracer::draw() {
    image.updateColor(glm::vec3(0.0f));
    camera.computeMatrices();

    // Normal Shading
    if (this->shading_mode == ShadingMode::NORMAL) {
        /**
         * TODO: After Completing Task 3
         * set `active_samples_per_pixel = 1`
         */
        active_samples_per_pixel = 1;  // Task 3 is complete, so we set this to 1
        active_max_bounces = 1;
    } else {
        active_samples_per_pixel = samples_per_pixel;
        active_max_bounces = max_bounces;
    }

    std::queue<Ray> wip_queue;
    std::atomic<unsigned int> active_rays(0);  // Counter for rays in-flight

    unsigned int processed_ray_count = 0;
    unsigned int total_rays_count = active_samples_per_pixel * camera.height * camera.width * active_max_bounces;
    ProgressBar bar(total_rays_count);

    // Multi-threading setup
    std::vector<std::thread> threads;
    std::mutex queueMutex;
    std::mutex imageMutex;
    std::condition_variable cond_var;

    // Launch producer thread to enqueue initial rays.
    threads.emplace_back(
        std::thread(&RayTracer::init_rays_thread_callback, this,
                    std::ref(wip_queue),
                    std::ref(queueMutex),
                    std::ref(cond_var),
                    std::ref(active_rays)));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Launch consumer threads.
    int num_threads = std::thread::hardware_concurrency();
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back(
            std::thread(&RayTracer::trace_ray_thread_callback, this,
                        std::ref(wip_queue),
                        std::ref(image),
                        std::ref(*scene),
                        std::ref(queueMutex),
                        std::ref(imageMutex),
                        std::ref(cond_var),
                        active_samples_per_pixel,
                        std::ref(processed_ray_count),
                        std::ref(active_rays)));
    }

    unsigned int frame_update_count = 0;
    // Main thread loop: update display until all rays are processed.
    while (true) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (wip_queue.empty() && active_rays.load() == 0)
                break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        {
            refresh_display(imageMutex);
            if (frame_update_count % 4 == 0)
                bar.update(processed_ray_count);
            frame_update_count++;
        }
    }

    // Wait for all threads to finish.
    for (auto& thread : threads)
        thread.join();

    bar.update(processed_ray_count);
    refresh_display(imageMutex);

    std::cout << std::endl
              << "Done! " << processed_ray_count << " rays processed" << std::endl;
}