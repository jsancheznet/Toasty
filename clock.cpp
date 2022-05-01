#pragma once

#include <SDL.h>
#include "clock.h"

clock CreateClock()
{
    clock Result = {0};

    Result.DeltaTime = 0.0f;
    Result.PerfCounterNow = SDL_GetPerformanceCounter();
    Result.PerfCounterLast = 0;
    Result.SecondsElapsed = 0.0;

    return Result;
}

void UpdateClock(clock *Clock)
{
    Clock->PerfCounterLast = Clock->PerfCounterNow;
    Clock->PerfCounterNow = SDL_GetPerformanceCounter();
    Clock->DeltaTime = (f64)((Clock->PerfCounterNow - Clock->PerfCounterLast)*1000.0f / (f64)SDL_GetPerformanceFrequency() );
    Clock->DeltaTime /= 1000.0f;
    Clock->SecondsElapsed += Clock->DeltaTime;
}
