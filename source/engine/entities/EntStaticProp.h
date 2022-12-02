//
// Created by ash on 10/11/22.
//

#ifndef SPLATOON_3DS_ENTSTATICPROP_H
#define SPLATOON_3DS_ENTSTATICPROP_H

#include "engine/Entity.h"
#include <span>

#define ENT_STATIC_PROP_ATTRIBS_SZ 36

class EntStaticProp : public Entity {
public:
    EntStaticProp(Model& model, std::string name, std::span<std::byte, ENT_STATIC_PROP_ATTRIBS_SZ> attribs);
};


#endif //SPLATOON_3DS_ENTSTATICPROP_H
