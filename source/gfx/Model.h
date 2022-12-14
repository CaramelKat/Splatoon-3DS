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
#include "colourspaces.h"

class Model {
public:
    explicit Model(const std::string& path);
    ~Model();

    using colour_type = rgb;
    void Draw(Gfx::State& gfx, C3D_Mtx model_matrix, std::span<colour_type> colours);

    //there needs to be some better API here, we'll eventually want access to the vertexes for mesh collision
    unsigned int VertexCount() const {
        return vertexes.size();
    }

    bool valid = false;
private:
    using index_type = unsigned short;
    std::span<index_type> indexes;

    using vertex_type = struct { vec3 position; vec2 uv; vec3 normal; };
    std::span<vertex_type> vertexes;

    colour_type colour = { 1.0f, 1.0f, 1.0f };

    C3D_Tex texture;
};

#endif //INC_SPLATOON_3DS_MODEL_H
