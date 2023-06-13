
#ifndef OBSTACLES_HPP
#define OBSTACLES_HPP

#include "simulation.hpp"

namespace golf
{

    class Pillar : public SimObject
    {

    public:
        Pillar(const Vec3 &position, double radius, double height);
    };

}

#endif // OBSTACLES_HPP
