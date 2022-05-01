#pragma once

#include "typedefs.h"

struct clock
{
    u64 PerfCounterNow;
    u64 PerfCounterLast;
    f64 DeltaTime;
    f64 SecondsElapsed;
};
