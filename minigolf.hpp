
#ifndef MINIGOLF_HPP
#define MINIGOLF_HPP

#include <vector>
#include "simulation.hpp"
#include <string>
#include <functional>


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
        Course(Game &game, Vec3 holePosition, Vec3 startPosition);
        void draw();
        const Vec3 &getHolePosition() { return holePosition; }
        double getHoleRadius() { return holeRadius; }
        const Vec3 &getStartPosition() { return startPosition; }
        bool collide(Sphere &sphere);
        void tick(unsigned long long time);
        void checkHole();
        void drawHole();
        std::vector<Triangle*> createFloor(int minXY, int maxXY, double resolution, std::function<double(double, double)> heightFunction);
        Wall* buildWallOnGround(double x1, double z1, double x2, double z2, double height, std::function<double(double, double)> heightFunction);
        std::vector<Wall*> buildWallsOnGround(const std::vector<double>& xz, double height, std::function<double(double, double)> heightFunction);
    };

};

class CourseA8 : public Course {
public:
    CourseA8(Game& game);
};

    class Course3 : public Course
    {
    public:
        Course3(Game &game);
    };

    // a controller for storing, changing and displaying golf shots
    class Controller : public SimObject
    {

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
