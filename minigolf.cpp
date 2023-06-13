
#include "minigolf.hpp"
#include <iostream>

namespace golf {

    const std::string getScoreTerm(int score, int par) {
        int diff = score - par;
        if (score == 1) return "hole in one";
        if(diff == 0) return "par";
        if(diff == -1) return "birdie";
        if(diff == -2) return "eagle";
        if(diff <= -3) return "albatross";
        if(diff == 1) return "bogey";
        if(diff == 2) return "double bogey";
        if(diff == 3) return "triple bogey";

        return std::to_string(score) + " strokes";
    }

    Player::Player(const std::string& name) : name(name), ball() {

    }

    void Player::reset(Vec3 position) {
        ball.setPosition(position);
        ball.setVelocity(Vec3(0));
        strokes = 0;
        finishedHole = false;
        startedHole = false;
    }

    Course::Course(Game& game, Vec3 holePosition, Vec3 startPosition) : SimObject(), game(game), holePosition(holePosition), startPosition(startPosition) {

        // reset all players
        for (Player& player : game.getPlayers()) {
            player.reset(startPosition);
        }
    }

    void Course::draw() {
        SimObject::draw();

        // draw hole
        Sphere hole(holePosition, holeRadius);
        hole.draw();

        for (Player& player : game.getPlayers()) {
            // draw ball
            if (!player.isInGame()) continue;
            player.getBall().draw();
            
        }
    }

    bool Course::collide(Sphere& sphere) {
        
        // collide with obstacles
        for (SimObject* child : children) {
            if (child->collide(sphere)) {
                return true;
            }
        }
    }

    void Course::tick(unsigned long long time) {

        checkHole();

    }

    void Course::checkHole() {
        // check if any player is in the hole

        for (Player& player : game.getPlayers()) {
            if(player.hasFinishedHole()) continue;
            if (player.getBall().getPosition().getDistance(holePosition) < holeRadius) {
                // player is in hole
                std::cout << getScoreTerm(player.getStrokes(), par) << "!" << std::endl;
                std::cout << player.getName() << " is in the hole!" << std::endl;
                player.setFinishedHole(true);
                player.getBall().setPosition(Vec3(-1000, -1000, -1000));
            }
        }
    }

    CourseA8::CourseA8(Game& game) : Course(game, Vec3(4, 0, 8), Vec3(0, 1, 0)) {
        // set hole radius
        holeRadius = 0.5;

        // par
        par = 2;

        // add walls
        Box b({-2,-2, -2, 6, 2, 6, 2, 10, 6, 10, 6, 2, 2, 2, 2, -2});
        for (Wall& wall : b.getWalls()) {
            addChild(new Wall(wall));
        }

        // create floor
        Vec3 p1(-2, -2);
        Vec3 p2(-2, 6);
        Vec3 p3(2, 6);
        Vec3 p4(2, 10);
        Vec3 p5(6, 10);
        Vec3 p6(6, 2);
        Vec3 p7(2, 2);
        Vec3 p8(2, -2);
        addChild(new GroundTile(p1, p2, p3));
        addChild(new GroundTile(p4, p5, p6));
        addChild(new GroundTile(p7, p4, p6));
        addChild(new GroundTile(p1, p8, p3));


    }


    Game::Game() {
        // create a player
        Player player("Player 1");
        // add player to game
        player.getBall().setPosition(Vec3(1, 1, 1));
        players.push_back(player);

        /*
        Player player2("Player 2");
        player2.getBall().setPosition(Vec3(2, 1, 4));
        players.push_back(player2);
        */

        // create course
        course = new CourseA8(*this);

        startGame();


    }

    bool Game::collide(Sphere& sphere) {
        return this->course->collide(sphere);
    }

    void Game::draw() {

        // draw course
        course->draw();

        // draw controller
        controller.draw();

    }

    void Game::startGame() {

        for (Player& player : players) {
            player.setStartedHole(true);
        }
    }

    bool Game::nextLevel() {

    }

    void Game::endGame() {

    }

    void Game::checkHoleEnding() {

        // check if all players have finished the hole
        bool allFinished = true;
        for (Player& player : players) {
            if (!player.hasFinishedHole()) {
                allFinished = false;
                break;
            }
        }

        if (allFinished) {
            // all players have finished the hole
            
            // check if there is another hole

            // load next hole if there is one

        }

    }

    void Game::tick(unsigned long long time) {
        // tick course
        course->tick(time);

        // tick controller
        controller.tick(time);

        // tick players
        for (Player& player : players) {
            player.getBall().tick(time);
        }


    }


}
