
#include "minigolf.hpp"

namespace golf {

    Player::Player(const std::string& name) : name(name), ball(Vec3(0), 0.4) {
        // give ball momentum
        ball.setVelocity(Vec3(2, 0, 1));

    }

    void Course::draw() {
        SimObject::draw();

        // draw hole
        Sphere hole(holePosition, holeRadius);
        hole.draw();

        for (Player& player : game.getPlayers()) {
            // draw ball
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

    CourseA8::CourseA8(Game& game) : Course(game) {
        // set hole position
        holePosition = Vec3(4, 0, 8);
        // set hole radius
        holeRadius = 0.5;
        // set start position
        startPosition = Vec3(0, 0, 0);

        // add walls
        Box b({-2,-2, -2, 6, 2, 6, 2, 10, 6, 10, 6, 2, 2, 2, 2, -2});
        for (Wall& wall : b.getWalls()) {
            addChild(new Wall(wall));
        }


    }

    Game::Game() {
        // create a player
        Player player("Player 1");
        // add player to game
        players.push_back(player);

        Player player2("Player 2");
        player2.getBall().setPosition(Vec3(2, 1, 4));
        players.push_back(player2);

        // create course
        course = new CourseA8(*this);


    }

    bool Game::collide(Sphere& sphere) {
        return this->course->collide(sphere);
    }

    void Game::draw() {

        // draw course
        course->draw();

        // draw controller
        controller.draw();

        // draw ui
    }
}
