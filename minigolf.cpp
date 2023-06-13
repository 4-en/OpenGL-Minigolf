
#include "minigolf.hpp"
#include <ostream>

namespace golf {

    Player::Player(const std::string& name) : name(name), ball() {
        // give ball momentum
        ball.setVelocity(Vec3(2, 0, 1));

    }

    void Controller::draw(){



        glBegin(GL_TRIANGLE_STRIP);
        glColor3f(1, 0, 1);

//        glVertex3f(0,0.5,0);
//        glVertex3f(1,0.5,0);
//        glVertex3f(startxval,0.5,startyval);



        glVertex3f(0+startpx,0.5,0+startpy);
        glVertex3f(1+startpx,0.5,0+startpy);
        glVertex3f(1+startpx,0.5,1+startpy);
        glVertex3f(0+startpx,0.5,1+startpy);


        glEnd();
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

    enum class ShotState {
        READY,
        AIMING,
        MOVING
    };

    Game::Game() {
        // create a player
        Player player("Player 1");
        // add player to game
        player.getBall().setPosition(Vec3(1, 1, 1));
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
