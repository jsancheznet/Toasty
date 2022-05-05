#pragma once

#include "typedefs.h"

typedef struct camera
{
    glm::vec3 Position;
    glm::vec3 Acceleration;
    glm::vec3 Velocity;

    glm::vec3 Front;
    glm::vec3 Up;

    f32 Speed;
    f32 Yaw;
    f32 Pitch;

    f32 Fov;
    f32 Near;
    f32 Far;

    glm::mat4 View;
    glm::mat4 Perspective;
    glm::mat4 Orthographic;

    // TODO: Add MV matrix for normals!

} camera;
