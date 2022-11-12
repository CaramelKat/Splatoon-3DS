//
// Created by ash on 11/12/22.
//

#ifndef SPLATOON_3DS_ENTLEVELGEOMETRY_H
#define SPLATOON_3DS_ENTLEVELGEOMETRY_H

#include "engine/Entity.h"

// Level geometry will presumably have some differences in colission and such. It's also notably involving colours
class EntLevelGeometry : public Entity {
public:
    EntLevelGeometry(Model& model, std::string name);
    ~EntLevelGeometry() override;
};


#endif //SPLATOON_3DS_ENTLEVELGEOMETRY_H
