//
// Created by ash on 10/11/22.
//

#ifndef INC_SPLATOON_3DS_ENTITY_H
#define INC_SPLATOON_3DS_ENTITY_H

#include <cstdint>
#include <utility>

#include "vector.h"
#include "gfx/Model.h"
#include "EntityType.h"

class Entity {
public:
    Entity(Model& model, std::string name) : name(std::move(name)), m_model(model) {}
    virtual ~Entity() = default;

    const std::string name;
    EntityType type;

    // todo pass in m_world_position, maybe texture
    void Draw(Gfx::State& gfx) {
        C3D_Mtx model_matrix;
        Mtx_Identity(&model_matrix);
        Mtx_RotateX(&model_matrix, m_world_rotation.x, false);
        Mtx_RotateY(&model_matrix, m_world_rotation.y, false);
        Mtx_RotateZ(&model_matrix, m_world_rotation.z, false);
        Mtx_Translate(&model_matrix, m_world_position.x, m_world_position.y, m_world_position.z, false);
        Mtx_Scale(&model_matrix, m_world_scale.x, m_world_scale.y, m_world_scale.z);
        m_model.Draw(gfx, model_matrix, colours);
    }

protected:
    Model& m_model;
    vec3 m_world_position {0, 0, 0};
    vec3 m_world_rotation {0, 0, 0};
    vec3 m_world_scale {1, 1, 1};
    std::span<Model::colour_type> colours {};
};

#endif //INC_SPLATOON_3DS_ENTITY_H
