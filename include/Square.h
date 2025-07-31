#ifndef __SQUARE_H__
#define __SQUARE_H__

#include "GeomTriangle.h"
#include "ModelBase.h"

class Square : public ModelBase {
   public:
    glm::vec3 center;
    float side_len;

    Square(glm::vec3 center, float side_len, glm::vec3 normal, std::shared_ptr<MaterialBase> mat);

    glm::vec3 get_surface_point() override;

   private:
    glm::vec3 tangent, bitangent;
};

#endif