//
// Created by Timothy on 9/5/2024.
//

#ifndef MATH_H
#define MATH_H

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace Tephryte {
    struct Transform {
        glm::dvec3 position = {0, 0, 0};
        glm::quat rotation = {0,0,0,0};

        Transform operator+(const Transform& rhs) const {
            return Transform{ .position = position + rhs.position, .rotation = rotation * rhs.rotation};
        }

        Transform& operator+=(const Transform& rhs) {
            position += rhs.position;
            rotation *= rhs.rotation;
            return *this;
        }

    };


}
#endif //MATH_H
