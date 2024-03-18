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
    void move(C3D_FVec camera_position, C3D_FVec camera_target, float distanceForwards, float distanceSideways) {
        // Ouch oof vector math I barely understand
        // Calculate the direction we're facing
        C3D_FVec direction = FVec3_Subtract(camera_target, camera_position);
        direction = FVec3_Normalize(direction); // Normalize to get unit direction vector?

        // Calculate the perpendicular vector to our current direction for side to side movement
        C3D_FVec perpendicular = FVec3_Cross(direction, FVec3_New(direction.x, direction.y + 1.0, direction.z));

        // Calculate displacement for both forward and sideways movement, yay for scaling!
        C3D_FVec displacementForwards = FVec3_Scale(direction, distanceForwards);
        C3D_FVec displacementSideways = FVec3_Scale(perpendicular, distanceSideways);
        C3D_FVec displacement = FVec3_Add(displacementForwards, displacementSideways);

		// Atan2 of x and z gets us the current angle of the camera so miku is always facing the direction she's moving in
		setRotation({0, atan2f(displacement.x, displacement.z), 0});

        // Move player to the new position
        auto result = FVec3_Add(FVec3_New(m_world_position.x, m_world_position.y, m_world_position.z), displacement);
        m_world_position = {
                result.x,
                m_world_position.y,
                result.z
        };
    }

    void rotate(vec3 off) {
        m_world_rotation += off;
    }

	void setRotation(vec3 rotation) {
        m_world_rotation = rotation;
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
