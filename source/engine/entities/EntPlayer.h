//
// Created by ash on 10/19/23.
//

#ifndef SPLATOON_3DS_ENTPLAYER_H
#define SPLATOON_3DS_ENTPLAYER_H

#include "engine/Entity.h"

class EntPlayer : public Entity {
public:
    EntPlayer(Model& model, std::string name);

    // todo this sucks
    void move(float distance) {
        m_world_position.x += distance * sin(m_world_rotation.y);
        m_world_position.z += distance * cos(m_world_rotation.y);
    }
    void rotate(vec3 off) {
        m_world_rotation += off;
    }

    [[nodiscard]] vec3 position() const {
        return m_world_position;
    }

    [[nodiscard]] vec3 head_position() const {
        auto pos = m_world_position;
        pos.y += 1.0f;
        return pos;
    }

    [[nodiscard]] vec3 rotation() const {
        return m_world_rotation;
    }
};


#endif //SPLATOON_3DS_ENTPLAYER_H
