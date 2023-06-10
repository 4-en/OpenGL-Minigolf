
#ifndef MINIGOLF_HPP
#define MINIGOLF_HPP

#include <vector>
#include "simulation.hpp"
#include <string>


namespace golf {

class GroundTile : public Triangle {
public:
    GroundTile(const Vec3& p1, const Vec3& p2, const Vec3& p3) : Triangle(p1, p2, p3) {
        this->frictionCoefficient = 0.03;
        this->faceCollisionOnly = true;
        this->bounceFactor = 0.8;
    }
};

class Golfball : public Sphere {
public:
    Golfball() : Sphere(Vec3(0), 0.4) { this->bounceFactor = 1.0;}
};

class Player {

private:
    std::string name;
    unsigned int score = 0;
    Golfball ball;

public:
    Player(const std::string& name);

    std::string getName() { return name; }
    unsigned int getScore() { return score; }
    void setScore(unsigned int score) { this->score = score; }
    Golfball& getBall() { return ball; }
    void setBall(Golfball ball) { this->ball = ball; }

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
    double getHoleRadius() { return holeRadius; }
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
