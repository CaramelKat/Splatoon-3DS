//
// Created by ash on 10/11/22.
//

#include "EntStaticProp.h"
#include "util.h"

EntStaticProp::EntStaticProp(Model &model, std::string name, std::span<std::byte, 12> attribs) :
    Entity(model, std::move(name)) {

    m_world_position = FromBytes<vec3>(attribs.subspan<0, 12>());
}
