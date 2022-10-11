//
// Created by ash on 10/11/22.
//
#include "Map.h"

#include "entities/EntStaticProp.h"
#include "util.h"

#include <vector>
#include <string>

struct FileHeader {
    int models_count;
};
// Idea is for each one of these to correspond to one *model load*
// Since we might use the same model many times
struct FileModel {
    int entity_count;
    int entities_len;
    char name[16];
};
struct FileEntity {
    EntityType type;
    size_t attribs_length;
    std::byte attribs_value[];
};

Map::Map(const std::string &path) {
    FILE* fd = fopen(path.c_str(), "rb");
    if (!fd) return;
    OnLeavingScope _fd_c([&] {
        fclose(fd);
        fd = nullptr;
    });

    FileHeader fhdr;
    if (!ReadOne(&fhdr, fd)) return;

    std::vector<FileModel> f_models(fhdr.models_count);
    if (!ReadVector(f_models, fd)) return;

    for (auto& f_model : f_models) {
        auto& model = m_models.emplace_back(std::string("models/") + f_model.name + ".3dml");
        if (!model.valid) continue;

        std::vector<std::byte> f_entities(f_model.entities_len);
        if (!ReadVector(f_entities, fd)) return;

        size_t tlv_offset = 0;
        for (int i = 0; i < f_model.entity_count; i++) {
            auto* f_entity = reinterpret_cast<FileEntity*>(&f_entities.at(tlv_offset)); // UB goes here
            std::span<std::byte> attribs {f_entity->attribs_value, f_entity->attribs_length };

            // MAKE SURE to check attribs.size before calling subspan!
            switch (f_entity->type) {
                case ENTITY_TYPE_STATIC_PROP: {
                    if (attribs.size_bytes() != 12) return; // corrupt, bail
                    m_entities.push_back(make_shared<EntStaticProp>(
                        model, f_model.name, attribs.subspan<0, 12>()
                    ));
                }
                default: {
                    // likely bug in tlv offset, bail
                    return;
                }
            }

            // <dragons>
            tlv_offset += offsetof(typeof(*f_entity), attribs_value) + f_entity->attribs_length;
            // </dragons>
        }
    }

    valid = true;
}

