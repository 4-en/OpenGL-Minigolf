
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

public:
    Player(const std::string& name);

    std::string getName() { return name; }
    unsigned int getScore() { return score; }
    void setScore(unsigned int score) { this->score = score; }
    Sphere& getBall() { return ball; }
    void setBall(Sphere ball) { this->ball = ball; }

};
class Game;
// a base golf course with walls, floor, obstacles and a hole
class Course : public SimObject {
protected:
    Vec3 holePosition;
    double holeRadius;
    Vec3 startPosition;
    Game& game;

public:
    Course(Game& game) : game(game) {}
    void draw();
    const Vec3& getHolePosition() { return holePosition; }
    const double getHoleRadius() { return holeRadius; }
    const Vec3& getStartPosition() { return startPosition; }
    bool collide(Sphere& sphere);

};

class CourseA8 : public Course {
public:
    CourseA8(Game& game);
};


// a controller for storing, changing and displaying golf shots
class Controller : public SimObject {

private:
    double strength = 0;
    double direction = 0;
    //...
    //Ball&

public:
    Controller() {}
};

// the top class controlling other parts like course, controller, ...
class Game : public SimObject {

private:
    Controller controller;
    Course* course;
    std::vector<Player> players;
    unsigned int currentLevel = -1;

public:
    Game();

    std::vector<Player>& getPlayers() { return players; }
    Controller& getController() { return controller; }
    Course& getCourse() { return *course; }
    void draw();
    bool collide(Sphere& sphere);


};


};

#endif // MINIGOLF_HPP
