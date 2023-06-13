
#include "minigolf.hpp"
#include <iostream>
#include <obstacles.hpp>

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

    void Player::startHole() {
        startedHole = true;
        ball.setVelocity(Vec3(2,0,2));
    }

    void Player::reset(Vec3 position) {
        ball.setPosition(position);
        ball.setVelocity(Vec3(0));
        strokes = 0;
        finishedHole = false;
        startedHole = false;
    }

    void Player::resetAll() {
        reset(Vec3(0));
        score = 0;
    }

    Course::Course(Game& game, Vec3 holePosition, Vec3 startPosition) : SimObject(), game(game), holePosition(holePosition), startPosition(startPosition) {

        // reset all players
        for (Player& player : game.getPlayers()) {
            player.reset(startPosition);
        }
    }

    void Course::draw() {
        SimObject::draw();

        drawHole();

        for (Player& player : game.getPlayers()) {
            // draw ball
            if (!player.isInGame()) continue;
            player.getBall().draw();
            
        }
    }

    void Course::drawHole() {
        // draw hole

        // TODO: draw flag
        glColor3f(0.3, 0.3, 0.3);
        glPushMatrix();
        glTranslatef(holePosition.x, holePosition.y, holePosition.z);
        glLineWidth(5);
        glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 4, 0);
        glEnd();
        glBegin(GL_TRIANGLES);
        glColor3f(0.9, 1, 0.9);
        glVertex3f(0, 4, 0);
        glVertex3f(0.7, 3.5, 0);
        glVertex3f(0, 3, 0);
        glEnd();
        glPopMatrix();

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
                triangles.push_back(new GroundTile(p3, p1, p2));
                triangles.push_back(new GroundTile(p3, p4, p2));
            }
        }

        return triangles;
    }

    Wall* Course::buildWallOnGround(double x1, double z1, double x2, double z2, double height, std::function<double(double, double)> heightFunction) {

        double y1 = heightFunction(x1, z1);
        double y2 = heightFunction(x2, z2);


        // TODO: somewhere nan for some reason
        Vec3 c1 = Vec3(x1, y1-height/2, z1+0.001);
        Vec3 c2 = Vec3(x1, y1+height, z1+0.002);
        Vec3 c3 = Vec3(x2, y2+height, z2+0.003);
        Vec3 c4 = Vec3(x2, y2-height/2, z2+0.004);

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

    Course3::Course3(Game& game) : Course(game, Vec3(4, 1.8, 0), Vec3(-3, 0.5, -1.5)) {
        // set hole radius
        holeRadius = 0.4;

        // par
        par = 4;

        auto heightFunction = [](double x, double y) {
            return (sin(x/3) + cos(y/2));
        };

        auto minHeightFunction = [&](double x, double y) {
            double height = heightFunction(x, y);
            double minHeight = 0;
            if (height < minHeight) {
                return minHeight;
            }
            return height;
        };

        std::vector<Triangle*> triangles = createFloor(-5, 5, 0.5, minHeightFunction);
        for (Triangle* triangle : triangles) {
            addChild(triangle);
        }

        std::vector<double> xz;
        for(double i = 0; i < 2*PI; i += PI/16) {
            xz.push_back(5 * cos(i));
            xz.push_back(5 * sin(i));
        }

        auto walls = buildWallsOnGround(xz, 1, minHeightFunction);
        for (Wall* wall : walls) {
            addChild(wall);
        }

        
    }

    Course4::Course4(Game& game) : Course(game, Vec3(2, 0, 3), Vec3(-3.5, 0.5, -1)) {
        // set hole radius
        holeRadius = 0.4;

        // par
        par = 4;

        auto heightFunction = [](double x, double y) {
            double start = -2;
            double end = 0;
            double height = 0;
            if (x > start && x < end) {
                x -= start;
                x *= PI/2;
                height = sin(x)*0.5;
            }
            return height;
        };

        

        std::vector<Triangle*> triangles = createFloor(-5, 5, 0.5, heightFunction);
        for (Triangle* triangle : triangles) {
            addChild(triangle);
        }
        std::vector<double> xz = {-4, -2, -4, 0, 1, 0, 1, 0, 1, 4, 3, 4, 3, 0, 1, -2};

        auto walls = buildWallsOnGround(xz, 1, heightFunction);
        for (Wall* wall : walls) {
            addChild(wall);
        }

        // add obstacles
        obstacle = new Pillar(Vec3(1, 0, 2), 0.5, 4);
        addChild(obstacle);

        
    }

    void Course4::tick(unsigned long long time) {
        Course::tick(time);
        this->obstacle->getPosition().z = sin(time/(1000.0*1000.0*1000.0))*2;
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
        //course = new CourseA8(*this);

        startGame();


    }

    bool Game::collide(Sphere& sphere) {
        return this->course->collide(sphere);
    }

    void Game::draw() {

        // draw course
        if (course != nullptr)
            course->draw();

        // draw controller
        controller.draw();

    }

    void Game::startGame() {
        std::cout << "Starting game" << std::endl;
        nextLevel();
    }

    bool Game::nextLevel() {

        currentLevel++;

        switch (currentLevel)
        {
        case 0:
            setLevel(new CourseA8(*this));
            break;
        case 1:
            setLevel(new Course2(*this));
            break;
        case 2:
            setLevel(new Course3(*this));
            break;
        case 3:
            setLevel(new Course4(*this));
            break;
        default:
            setLevel(nullptr);
            return false;
        }

        currentPlayer = -1;
        shotState = ShotState::READY;

        return true;

    }

    void Game::endGame() {
        shotState = ShotState::FINISHED;

        // print final scores
        std::cout << "Final scores:" << std::endl;
        Player* winner = nullptr;
        unsigned int lowestScore = UINT_MAX;
        for (Player& player : players) {
            if (player.getScore() < lowestScore) {
                lowestScore = player.getScore();
                winner = &player;
            }
            std::cout << player.getName() << ": " << player.getScore() << std::endl;
            player.resetAll();
        } 

        std::cout << "\nWinner: " << winner->getName() << std::endl << std::endl;



        currentLevel = -1;


    }

    void Game::checkHoleEnding() {

        // check if all players have finished the hole
        for (Player& player : players) {
            if (!player.hasFinishedHole()) {
                return;
            }
        }

        
        // all players have finished the hole
            
        // check if there is another hole
        if (nextLevel()) {
            return;
        }

        // no more holes, end game
        endGame();

        

    }

    void Game::shootBall(Vec3 velocity) {
        // shoots the ball with the given velocity

        // return if not ready to shoot
        if(shotState == ShotState::MOVING) return;

        if(currentPlayer < 0) return;

        Player& player = players[currentPlayer];
        shotStart = player.getBall().getPosition();
        lastBallPosition = shotStart;
        noMovementCounter = 0;
        shotState = ShotState::MOVING;
        player.getBall().setVelocity(velocity);
        player.addStroke();

    }

    void Game::getNextPlayer() {
        // find the next turn player

        // find a player that hasnt started the hole yet
        for(size_t i = 0; i < players.size(); i++) {
            if(!players[i].hasStartedHole()) {
                currentPlayer = i;
                players[i].startHole();
                shotStart = players[i].getBall().getPosition();
                shotState = ShotState::AIMING;
                return;
            }
        }

        // if all players have started the hole, find the player with most distance to the hole
        bool foundPlayer = false;
        double maxDistance = 0;
        for(size_t i = 0; i < players.size(); i++) {
            if(!players[i].isInGame()) continue;
            double distance = players[i].getBall().getPosition().getDistance(course->getHolePosition());
            if(distance > maxDistance) {
                maxDistance = distance;
                currentPlayer = i;
                foundPlayer = true;
            }
        }
        if(!foundPlayer) {
            // no player is in game...
            return;
        }
        shotStart = players[currentPlayer].getBall().getPosition();
        shotState = ShotState::AIMING;
        
    }

    void Game::setLevel(Course* course) {
        if(this->course != nullptr) delete(this->course);
        this->course = course;
    }

    void Game::tick(unsigned long long time) {

        checkHoleEnding();

        /*
        switch(shotState) {
            case ShotState::READY:
                std::cout << "Ready" << std::endl;
                break;
            case ShotState::AIMING:
                std::cout << "Aiming" << std::endl;
                break;
            case ShotState::MOVING: 
                std::cout << "Moving" << std::endl;
                break;
            case ShotState::FINISHED:   
                std::cout << "Finished" << std::endl;
                break;
        }
        */
        // ...

        switch (shotState)
        {
        case ShotState::READY:
            // find next player
            getNextPlayer();
            break;
        case ShotState::AIMING:
            if(players[currentPlayer].hasFinishedHole()) {
                    shotState = ShotState::READY;
                    break;
            }
            // set to moving since no controller is used
            shotState = ShotState::MOVING;
            // aim the shot
            // wait for shot from controller
            break;
        case ShotState::MOVING:
            // check if ball is in hole
            if(players[currentPlayer].hasFinishedHole()) {
                shotState = ShotState::READY;
                break;
            }
            // check if ball has stopped
            if(lastBallPosition.getDistance(players[currentPlayer].getBall().getPosition()) < 0.01) {
                noMovementCounter++;
            } else {
                noMovementCounter = 0;
            }
            if(noMovementCounter>120) {
                shotState = ShotState::READY;
            }

            // check if ball is out of bounds
            if(players[currentPlayer].getBall().getPosition().y < -10) {
                // out of bounds
                players[currentPlayer].getBall().setPosition(shotStart);
                players[currentPlayer].getBall().setVelocity(Vec3(0));
                shotState = ShotState::AIMING;
                // give penalty
                players[currentPlayer].addStroke();
                std::cout << players[currentPlayer].getName() << " is out of bounds!" << std::endl;
                break;
            }
            break;
        case ShotState::FINISHED:
            startGame();
        default:
            return;
        }


        // tick course
        if(course != nullptr)
            course->tick(time);

        // tick controller
        controller.tick(time);

        // tick players
        for (Player& player : players) {
            player.getBall().tick(time);
        }


    }


}
