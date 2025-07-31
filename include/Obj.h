#ifndef __OBJ_H__
#define __OBJ_H__

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "GeomTriangle.h"
#include "ModelBase.h"

class Obj : public ModelBase {
   public:
    Obj(const char* filename, std::shared_ptr<MaterialBase> mat) {
        // Material Object can be shared between multiple geometries
        this->material = mat;

        // Load triangle soup
        loadObj(filename);
    }

   private:
    void loadObj(const char* filename) {
        std::vector<glm::vec3> temp_vertices;
        std::vector<glm::vec3> temp_normals;
        std::vector<unsigned int> vertexIndices, normalIndices;

        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Cannot open file: " << filename << std::endl;
            exit(-1);
        }
        std::cout << "Loading " << filename << "..." << std::endl;

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream ss(line);
            std::string prefix;
            ss >> prefix;

            if (prefix == "v") {
                glm::vec3 vertex;
                ss >> vertex.x >> vertex.y >> vertex.z;
                temp_vertices.push_back(vertex);
            } else if (prefix == "vn") {
                glm::vec3 normal;
                ss >> normal.x >> normal.y >> normal.z;
                temp_normals.push_back(normal);
            } else if (prefix == "f") {
                unsigned int vIndex[3], nIndex[3];
                char slash;
                for (int i = 0; i < 3; i++) {
                    ss >> vIndex[i] >> slash >> slash >> nIndex[i];
                }
                for (int i = 0; i < 3; i++) {
                    vertexIndices.push_back(vIndex[i] - 1);
                    normalIndices.push_back(nIndex[i] - 1);
                }
            }
        }
        file.close();

        std::cout << "Processing data..." << std::endl;
        for (size_t i = 0; i < vertexIndices.size(); i += 3) {
            std::vector<glm::vec3> triVertices = {
                temp_vertices[vertexIndices[i]],
                temp_vertices[vertexIndices[i + 1]],
                temp_vertices[vertexIndices[i + 2]]};
            std::vector<glm::vec3> triNormals = {
                temp_normals[normalIndices[i]],
                temp_normals[normalIndices[i + 1]],
                temp_normals[normalIndices[i + 2]]};
            geometries.push_back(std::make_unique<GeomTriangle>(triVertices, triNormals));
        }
        std::cout << "Done loading and processing. " << geometries.size() << std::endl;
    }

    glm::vec3 get_surface_point() override {
        return glm::vec3(0.0f);  // Placeholder implementation
    }
};

#endif
