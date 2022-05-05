#pragma once

#include <SDL.h>
#include "window.h"

struct renderer
{
    window Window;

    glm::vec4 BackgroundColor = glm::vec4(100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 1.0f);

    u32 MatricesUBO;
};
