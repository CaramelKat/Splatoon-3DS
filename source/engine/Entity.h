//
// Created by ash on 10/11/22.
//

#ifndef INC_SPLATOON_3DS_ENTITY_H
#define INC_SPLATOON_3DS_ENTITY_H

#include <cstdint>
#include <utility>

#include "vector.h"
#include "gfx/Model.h"

enum EntityType : uint16_t {
    ENTITY_TYPE_STATIC_PROP = 0,
    ENTITY_TYPE_PHYSICS_PROP = 1,
    ENTITY_TYPE_PLAYER = 1000,
    ENTITY_TYPE_BOMB = 1001,
    // etc.
};

class Entity {
public:
    Entity(Model& model, std::string name) : name(std::move(name)), m_model(model) {}
    virtual ~Entity() = default;

    const std::string name;
    EntityType type;

    // todo pass in m_world_position, maybe texture
    void Draw(Gfx::State& gfx) { m_model.Draw(gfx, m_world_position); }

protected:
    Model& m_model;
    vec3 m_world_position {0, 0, 0};
};

#endif //INC_SPLATOON_3DS_ENTITY_H
