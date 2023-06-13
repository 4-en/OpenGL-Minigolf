
#include "obstacles.hpp"

namespace golf {

    Pillar::Pillar(const Vec3 &position, double radius, double height) : SimObject(position)
    {
        // create a cylinder from triangles
        // top and bottom
        SimObject* top = new SimObject;
        SimObject* bottom = new SimObject;
        SimObject* sides = new SimObject;
        constexpr size_t resolution = 10;
        Vec3 center(0);
        for(size_t i = 0; i < resolution; i++) {
            double angle = 2 * PI * i / resolution;
            Vec3 p1(radius * cos(angle), 0, radius * sin(angle));
            Vec3 p2(radius * cos(angle + 2 * PI / resolution), 0, radius * sin(angle + 2 * PI / resolution));
            top->addChild(new Triangle(center, p1, p2));
            bottom->addChild(new Triangle(center, p1, p2));

            // sides
            Vec3 p3(radius * cos(angle), height, radius * sin(angle));
            Vec3 p4(radius * cos(angle + 2 * PI / resolution), height, radius * sin(angle + 2 * PI / resolution));
            sides->addChild(new Triangle(p1, p2, p3));
            sides->addChild(new Triangle(p2, p3, p4));

        }
        top->setPosition(Vec3(0, height, 0));
        addChild(top);
        addChild(bottom);
        addChild(sides);
    }

        



}

