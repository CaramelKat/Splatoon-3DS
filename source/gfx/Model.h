//
// Created by ash on 9/15/22.
//

#ifndef INC_SPLATOON_3DS_MODEL_H
#define INC_SPLATOON_3DS_MODEL_H

#include <string>
#include <span>
#include <citro3d.h>
#include "Gfx.h"
#include "vector.h"

class Model {
public:
    explicit Model(const std::string& path);
    ~Model();

    void Draw(Gfx::State& gfx);

    bool valid = false;
    C3D_FVec world_position;
private:
    using index_type = unsigned short;
    std::span<index_type> indexes;

    using vertex_type = struct { vec3 position; vec2 uv; vec3 normal; };
    std::span<vertex_type> vertexes;

    C3D_Tex texture;
};

#endif //INC_SPLATOON_3DS_MODEL_H
