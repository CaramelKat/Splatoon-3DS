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
    explicit Map(const std::string& path);

    bool valid = false;

    void Draw(Gfx::State& gfx) {
        for (const auto& ent : m_entities) {
            ent->Draw(gfx);
        }
    }

    template<typename Ent>
    [[nodiscard]] std::shared_ptr<Ent> AddDynamicEntity(const std::string& model_name) {
        auto& model = m_models.emplace_back(std::string("romfs:/models/") + model_name + ".3mdl");
        if (!model.valid) return {};

        auto p = std::make_shared<Ent>(
            model, model_name
        );
        m_entities.push_back(p);

        return p;
    }

private:
    std::list<std::shared_ptr<Entity>> m_entities;
    // should probably be using shared_ptr here
    std::list<Model> m_models;
    // yet unknown "floor geometry" type
};

#endif //INC_SPLATOON_3DS_MAP_H
