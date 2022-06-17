
//------------------------------------------------------------------------------

#include <SFML/System/Clock.hpp>

extern "C" {
    #include "tq_clock.h"
    #include "tq_log.h"
}

//------------------------------------------------------------------------------

namespace
{
    sf::Clock *clock;

    void initialize()
    {
        clock = new sf::Clock();
    }

    void terminate()
    {
        delete clock;
    }

    float get_time_mediump()
    {
        return clock->getElapsedTime().asMilliseconds() / 1000.0f;
    }

    double get_time_highp()
    {
        return clock->getElapsedTime().asMicroseconds() / 1000000.0;
    }
}

//------------------------------------------------------------------------------

void construct_sf_clock(struct clock *clock)
{
    clock->initialize       = ::initialize;
    clock->terminate        = ::terminate;
    clock->get_time_mediump = ::get_time_mediump;
    clock->get_time_highp   = ::get_time_highp;
}

//------------------------------------------------------------------------------
