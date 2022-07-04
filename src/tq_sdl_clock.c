
//------------------------------------------------------------------------------
// tq library :3
//------------------------------------------------------------------------------

#include <SDL.h>

#include "tq_clock.h"
#include "tq_error.h"

//------------------------------------------------------------------------------

static Uint32 mediump_clock_start;
static Uint64 highp_clock_start;
static double highp_clock_freq;

//------------------------------------------------------------------------------

static void initialize(void)
{
    if (SDL_Init(SDL_INIT_TIMER) < 0) {
        tq_error("Failed to initialize SDL clock.");
    }

    mediump_clock_start = SDL_GetTicks();
    highp_clock_start = SDL_GetPerformanceCounter();
    highp_clock_freq = (double) SDL_GetPerformanceFrequency();
}

static void terminate(void)
{
    SDL_Quit();
}

static float get_time_mediump(void)
{
    return (SDL_GetTicks() - mediump_clock_start) / 1000.0f;
}

static double get_time_highp(void)
{
    Uint64 counter = SDL_GetPerformanceCounter();
    return (counter - highp_clock_start) / highp_clock_freq;
}

//------------------------------------------------------------------------------

void construct_sdl_clock(struct clock *clock)
{
    clock->initialize       = initialize;
    clock->terminate        = terminate;
    clock->get_time_mediump = get_time_mediump;
    clock->get_time_highp   = get_time_highp;
}

//------------------------------------------------------------------------------
