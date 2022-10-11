//
// Created by ash on 10/11/22.
//

#ifndef INC_SPLATOON_3DS_MAP_H
#define INC_SPLATOON_3DS_MAP_H

#include <list>
#include <string>
#include <memory>
#include "Entity.h"
#include "gfx/Model.h"

class Map {
public:
    Map(const std::string& path);

    bool valid = false;

    void Draw(Gfx::State& gfx) {
        for (const auto& ent : m_entities) {
            ent->Draw(gfx);
        }
    }

private:
    std::list<std::shared_ptr<Entity>> m_entities;
    std::list<Model> m_models;
    // yet unknown "floor geometry" type
};

#endif //INC_SPLATOON_3DS_MAP_H
