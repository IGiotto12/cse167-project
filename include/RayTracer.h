#ifndef __RAY_TRACER_H__
#define __RAY_TRACER_H__

#include "Camera.h"
#include "GeometryBase.h"
#include "Scene.h"
#include "ShadingMode.h"

class Ray;
class Image;

class RayTracer {
   public:
    // Camera
    Camera camera;

    // Scene
    std::unique_ptr<Scene> scene;

    // Image
    Image &image;

    // number of samples per pixel
    int samples_per_pixel = 1;
    int max_bounces = 1;

    RayTracer(int width, int height, Image &image) : camera(width, height), image(image) {};
    Ray ray_thru_pixel(int i, int j);
    void trace_ray_thread_callback(std::queue<Ray> &wipQueue,
                                   Image &image,
                                   Scene &scene,
                                   std::mutex &queueMutex,
                                   std::mutex &imageMutex,
                                   std::condition_variable &cond_var,
                                   int samples_per_pixel,
                                   unsigned int &processed_ray_count,
                                   std::atomic<unsigned int> &active_rays);
    void init_rays_thread_callback(std::queue<Ray> &wipQueue,
                                   std::mutex &queueMutex,
                                   std::condition_variable &cond_var,
                                   std::atomic<unsigned int> &active_rays);
    void draw();
    void init(int scene_id);
    void set_shading_mode(ShadingMode shading_mode);

   private:
    int active_samples_per_pixel;
    int active_max_bounces;

    // to determine shading mode
    ShadingMode shading_mode;  // 0 - normal, 1 - ray tracing, 2 - debug

    void refresh_display(std::mutex &imageMutex);
};
#endif