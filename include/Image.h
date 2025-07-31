/*
Image Class to write image as a texture to the screen
*/
#ifndef __IMAGE_H__
#define __IMAGE_H__
class Image {
   public:
    unsigned int fbo;  // frame buffer object
    unsigned int tbo;  // texture buffer object

    int width;
    int height;
    std::vector<glm::vec3> pixels;  // RGB colors
    float exposure = 1.0f;

    Image(int width, int height) : width(width), height(height) {
        // Initialize pixels with black color (0, 0, 0)
        pixels.assign(width * height, glm::vec3(0.5f, 0.0f, 1.0f));
    };

    void init() {
        // Generate Buffers
        glGenFramebuffers(1, &fbo);
        glGenTextures(1, &tbo);
    }

    void updateColor(glm::vec3 color) {
        for (int idx = 0; idx < pixels.size(); idx++) {
            pixels[idx] = glm::vec3(color);
        }
    }

    glm::vec3 tone_map(glm::vec3 color) {
        return color / (1.0f + color);
    }

    glm::vec3 ACES_Tonemap(glm::vec3 color) {
        return color * (2.51f * color + 0.03f) / (color * (2.43f * color + 0.59f) + 0.14f);
    }

    virtual glm::vec3 Linear_to_sRGB(glm::vec3 color) {
        auto l = [](float c) {
            return (c <= 0.0031308) ? (c * 12.92) : (1.055 * pow(c, 1.0 / 2.4) - 0.055);
        };
        return glm::vec3(l(color[0]), l(color[1]), l(color[2]));
    }

    glm::vec3 GammaCorrect(glm::vec3 color) {
        return glm::pow(color, glm::vec3(1.0f / 2.2f));
    }

    void updatePixel(int i, int j, glm::vec3 color) {
        pixels[j * width + i] = color;
    }

    glm::vec3 getPixel(int i, int j) {
        return pixels[j * width + i];
    }

    void copy(Image image) {
        assert(image.pixels.size() == pixels.size());
        for (int idx = 0; idx < pixels.size(); idx++)
            pixels[idx] = glm::vec3(image.pixels[idx]);
    }

    void draw() {
        std::vector<glm::vec3> new_pixels;
        new_pixels.assign(pixels.size(), glm::vec3(0.0f));

        // tone map
        for (int idx = 0; idx < pixels.size(); idx++)
            new_pixels[idx] = GammaCorrect(ACES_Tonemap(pixels[idx]));

        // Load Textures
        glBindTexture(GL_TEXTURE_2D, tbo);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, &new_pixels[0][0]);

        // Attach Texture and Read Frame
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tbo, 0);

        // Copy framebuffer from read to write
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(
            0, 0, width, height,  // source (x0, y0, x1, y1)
            0, height, width, 0,  // destination (x0, y0, x1, y1) flipped vertically
            GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // glEnable(GL_FRAMEBUFFER_SRGB);
    }
};

#endif