
#include "minigolf.hpp"

namespace golf {

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

        // create course
        course = new CourseA8(*this);


    }

    void Game::draw() {

        // draw course
        course->draw();

        // draw controller
        controller.draw();
    }
}
