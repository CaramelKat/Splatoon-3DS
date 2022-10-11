//
// Created by ash on 10/11/22.
//
#include "Map.h"

#include <vector>
#include <string>

// Idea is for each one of these to correspond to one *model load*
// Since we might use the same model many times
struct FileEntity {
    int count;
    char name[16];
};
struct FileEntityAttributes {
    EntityType type;
    // byte[]?
};

Map::Map(const std::string &path) {
    // just draft stuff to work out the file format
    std::vector<FileEntity> entities;
    // fread...

    for (auto& fentity : entities) {
        auto& model = m_models.emplace_back(std::string("models/") + fentity.name + ".3dml");
        if (!model.valid) continue;

        for (int i = 0; i < fentity.count; i++) {
            auto& ent = m_entities.emplace_back(model);
            // how to handle attributes (position etc?)
            // I guess here we *actually* want to have a big switch case
            // and add many different subclasses to the array..?
        }
    }
}

