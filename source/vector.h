//
// Created by ash on 10/11/22.
//

#ifndef INC_SPLATOON_3DS_VECTOR_H
#define INC_SPLATOON_3DS_VECTOR_H

#include <c3d/types.h>

struct vec2 {
    float x;
    float y;

    explicit operator C3D_FVec() const { return {{0.0f, 0.0f, y, x}}; }
};
struct vec3 {
    float x;
    float y;
    float z;

    explicit operator C3D_FVec() const { return {{0.0f, z, y, x}}; }

    vec3 operator+(const vec3 &other) const {
        return {x + other.x, y + other.y, z + other.z};
    }
    vec3& operator+=(const vec3 &other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
};


#endif //INC_SPLATOON_3DS_VECTOR_H
