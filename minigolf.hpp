
#ifndef MINIGOLF_HPP
#define MINIGOLF_HPP

#include <vector>
#include "simulation.hpp"
#include <string>

namespace golf
{

    enum class ShotState
    {
        READY,
        AIMING,
        MOVING,
        FINISHED
    };

    const std::string getScoreTerm(int score, int par);

    class GroundTile : public Triangle
    {
    public:
        GroundTile(const Vec3 &p1, const Vec3 &p2, const Vec3 &p3) : Triangle(p1, p2, p3)
        {
            this->frictionCoefficient = 0.03;
            this->faceCollisionOnly = true;
            this->bounceFactor = 0.8;
        }
    };

    class Golfball : public Sphere
    {
    public:
        Golfball() : Sphere(Vec3(0), 0.4) { this->bounceFactor = 1.0; }
    };

    class Player
    {

    private:
        std::string name;
        unsigned int score = 0;
        unsigned int strokes = 0;
        bool finishedHole = false;
        bool startedHole = false;
        Golfball ball;

    public:
        Player(const std::string &name);

        std::string getName() { return name; }
        unsigned int getScore() { return score; }
        void setScore(unsigned int score) { this->score = score; }
        Golfball &getBall() { return ball; }
        void setBall(Golfball ball) { this->ball = ball; }
        bool hasFinishedHole() { return finishedHole; }
        void reset(Vec3 position);
        void resetAll();
        void setFinishedHole(bool finishedHole) { this->finishedHole = finishedHole; }
        unsigned int getStrokes() { return strokes; }
        void addStroke() { strokes++; }
        bool isInGame() { return startedHole && !finishedHole; }
        bool hasStartedHole() { return startedHole; }
        void startHole();
        void setStartedHole(bool startedHole) { this->startedHole = startedHole; }
    };
    class Game;
    // a base golf course with walls, floor, obstacles and a hole
    class Course : public SimObject
    {
    protected:
        Vec3 holePosition;
        double holeRadius;
        Vec3 startPosition;
        Game &game;
        unsigned int par = 3;

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
    };

    class CourseA8 : public Course
    {
    public:
        CourseA8(Game &game);
    };

    class Course2 : public Course
    {
    public:
        Course2(Game &game);
    };

    // a controller for storing, changing and displaying golf shots
    class Controller : public SimObject
    {

    private:
        double strength = 0;
        double direction = 0;
        //...
        // Ball&

    public:
        Controller() {}
    };

    // the top class controlling other parts like course, controller, ...
    class Game : public SimObject
    {

    private:
        Controller controller;
        Course *course = nullptr;
        std::vector<Player> players;
        int currentPlayer = 0;
        ShotState shotState = ShotState::READY;
        unsigned int noMovementCounter = 0;
        Vec3 shotStart;
        Vec3 lastBallPosition;
        unsigned int currentLevel = -1;

    public:
        Game();

        std::vector<Player> &getPlayers() { return players; }
        Controller &getController() { return controller; }
        Course &getCourse() { return *course; }
        void draw();
        bool collide(Sphere &sphere);
        void tick(unsigned long long time);
        void checkHoleEnding();
        void startGame();
        bool nextLevel();
        void endGame();
        void getNextPlayer();
        void shootBall(Vec3 velocity);
        void setLevel(Course* course);
    };

};

#endif // MINIGOLF_HPP
