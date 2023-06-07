
#ifndef MINIGOLF_HPP
#define MINIGOLF_HPP

#include <vector>
#include "simulation.hpp"
#include <string>


namespace golf {


class Player {

private:
    std::string name;
    unsigned int score = 0;
    Sphere ball;

};

// a base golf course with walls, floor, obstacles and a hole
class Course : SimObject {


};

// a controller for storing, changing and displaying golf shots
class Controller : SimObject {

private:
    double strength = 0;
    double direction = 0;
    //...
    //Ball&

public:
    Controller() {}
};

// the top class controlling other parts like course, controller, ...
class Game : SimObject {

private:
    Controller controller;
    Course course;
    unsigned int currentLevel = -1;

public:
    Game() {}


};


};

#endif // MINIGOLF_HPP
