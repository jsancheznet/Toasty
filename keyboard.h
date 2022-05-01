#pragma once

#include "typedefs.h"

struct keyboard
{
    const u8 *State;
    u8 *CurrentState;
    u8 *PrevState;
    i32 Numkeys;
    SDL_Keymod ModState;
};
