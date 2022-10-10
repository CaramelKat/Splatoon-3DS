//
// Created by ash on 10/11/22.
//

#ifndef INC_3DS_GFX_VECTOR_H
#define INC_3DS_GFX_VECTOR_H

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
};

#endif //INC_3DS_GFX_VECTOR_H
