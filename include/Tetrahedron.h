#ifndef __TETRAHEDRON_H__
#define __TETRAHEDRON_H__

#include "ModelBase.h"

class Tetrahedron : public ModelBase {
   public:
    float side_len;

    Tetrahedron(float side_len, std::shared_ptr<MaterialBase> mat);

    glm::vec3 get_surface_point() override { return glm::vec3(0.0f); };
};

#endif