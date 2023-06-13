
#include "minigolf.hpp"
#include <iostream>
#include <obstacles.hpp>

namespace golf {

    Player::Player(const std::string& name) : name(name), ball() {
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
        bool collided = false;
        for (SimObject* child : children) {
            if(child->collide(sphere)) {
                collided = true;
            }
        }
        return collided;
    }

    void Course::tick(unsigned long long time) {

        checkHole();

    }

    void Course::checkHole() {
        // check if any player is in the hole

        for (Player& player : game.getPlayers()) {
            if(player.hasFinishedHole()) continue;
            if (player.getBall().getPosition().getDistance(holePosition) < holeRadius + player.getBall().getRadius()) {
                // player is in hole
                std::cout << getScoreTerm(player.getStrokes(), par) << "!" << std::endl;
                std::cout << player.getName() << " is in the hole!" << std::endl;
                player.setFinishedHole(true);
                player.getBall().setPosition(Vec3(-1000, -1000, -1000));
                player.setScore(player.getScore() + player.getStrokes());
            }
        }
    }

    // creates a floor of triangles based on a function
    std::vector<Triangle*> Course::createFloor(int minXY, int maxXY, double resolution, std::function<double(double, double)> heightFunction) {

        std::vector<Triangle*> triangles;

        for (double x = minXY; x < maxXY; x += resolution) {
            for (double y = minXY; y < maxXY; y += resolution) {
                Vec3 p1(x, heightFunction(x, y), y);
                Vec3 p2(x + resolution, heightFunction(x + resolution, y), y);
                Vec3 p3(x, heightFunction(x, y + resolution), y + resolution);
                Vec3 p4(x + resolution, heightFunction(x + resolution, y + resolution), y + resolution);
                triangles.push_back(new Triangle(p3, p1, p2));
                triangles.push_back(new Triangle(p3, p4, p2));
            }
        }

        return triangles;
    }

    Wall* Course::buildWallOnGround(double x1, double z1, double x2, double z2, double height, std::function<double(double, double)> heightFunction) {

        double y1 = heightFunction(x1, z1);
        double y2 = heightFunction(x2, z2);

        Vec3 c1 = Vec3(x1, y1-height/2, z1);
        Vec3 c2 = Vec3(x1, y1+height, z1);
        Vec3 c3 = Vec3(x2, y2+height, z2);
        Vec3 c4 = Vec3(x2, y2-height/2, z2);

        Wall* wall = new Wall(c1, c2, c3, c4);
        return wall;
    }

    std::vector<Wall*> Course::buildWallsOnGround(const std::vector<double>& xz, double height, std::function<double(double, double)> heightFunction) {

        std::vector<Wall*> walls;

        for (size_t i = 0; i < xz.size(); i += 2) {
            walls.push_back(buildWallOnGround(xz[i], xz[i+1], xz[(i+2)%xz.size()], xz[(i+3)%xz.size()], height, heightFunction));
        }

        return walls;
    }

    CourseA8::CourseA8(Game& game) : Course(game, Vec3(4, 0, 8), Vec3(0, 1, 0)) {
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

        // add obstacles
        addChild(new Pillar(Vec3(5, 0, 7), 0.5, 4));


    }

    Course2::Course2(Game& game) : Course(game, Vec3(2.5, 0, -2), Vec3(-4, 0.5, -1.5)) {
        // set hole radius
        holeRadius = 0.4;

        // par
        par = 3;

        // add walls
        Box b({-2,0, -2, 3, 4, 3, 4, -3, -5, -3, -5, 0});
        for (Wall& wall : b.getWalls()) {
            addChild(new Wall(wall));
        }

        addChild(new Wall(1, 0, 1, -3));

        // create floor
        Vec3 p1(4, 3);
        Vec3 p2(4, -3);
        Vec3 p3(-2, -3);
        Vec3 p4(-2, 3);
        Vec3 p5(-2, 0);
        Vec3 p6(-5, 0);
        Vec3 p7(-5, -3);
        addChild(new GroundTile(p1, p2, p3));
        addChild(new GroundTile(p3, p4, p1));
        addChild(new GroundTile(p5, p3, p7));
        addChild(new GroundTile(p7, p6, p5));

        // add obstacles
        addChild(new Pillar(Vec3(4, 0, 3), 0.5, 4));
        // add obstacles
        addChild(new Pillar(Vec3(4, 0, -3), 0.5, 4));

        
    }

    Course3::Course3(Game& game) : Course(game, Vec3(2.5, 0, -2), Vec3(-4, 0.5, -1.5)) {
        // set hole radius
        holeRadius = 0.4;

        // par
        par = 4;

        // add walls
        auto heightFunction = [](double x, double y) {
            return sin(x/3) + cos(y/2);
        };

        std::vector<Triangle*> triangles = createFloor(-5, 5, 0.5, heightFunction);
        for (Triangle* triangle : triangles) {
            addChild(triangle);
        }

        
    }


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
