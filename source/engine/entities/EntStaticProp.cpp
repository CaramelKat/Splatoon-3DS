//
// Created by ash on 10/11/22.
//

#include "EntStaticProp.h"
#include "util.h"

struct EntStaticPropData {
    vec3 position;
    vec3 rotation;
    vec3 scale;
};

EntStaticProp::EntStaticProp(Model &model, std::string name, std::span<std::byte, ENT_STATIC_PROP_ATTRIBS_SZ> attribs) :
    Entity(model, std::move(name)) {

    auto attrib = FromBytes<EntStaticPropData>(attribs.subspan<0, ENT_STATIC_PROP_ATTRIBS_SZ>());
    m_world_position = attrib.position;
    m_world_rotation = attrib.rotation;
    m_world_scale = attrib.scale;
}
