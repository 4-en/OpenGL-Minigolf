#include "oglwidget.h"
#include <math.h>
#include "ui_mainwindow.h"
#include "QVector3D"
#include <iostream>
#include <thread>
#include <chrono>
#include <stdlib.h>

// simulation loop
// runs in separate thread
void OGLWidget::runSim()
{
    constexpr unsigned int fps = 60;
    constexpr double dtime = 1.0 / fps;
    double dt = dtime;
    constexpr double G = 6.67408e-11;
    constexpr double planetMass = 5.972e24;
    constexpr double planetRadius = 6.371e6;
    constexpr int micros = dtime * 1000 * 1000;
    constexpr auto waitTime = std::chrono::microseconds(micros);
    auto lastTime = std::chrono::high_resolution_clock::now();
    unsigned long long frame = 0;

    Plane floor(Vec3(0, 1.1, 0), Vec3(0, 0.1, 0));
    constexpr double floorBounce = 0.8;

    running = true;
    while (running)
    {
        lastTime = std::chrono::high_resolution_clock::now();
        // parama+=0.1;
        dt = dtime * paramb;
        // apply gravity
        for (Sphere &sphere : spheres)
        {
            // calculate gravity for planet
            double mass = sphere.getMass();
            double force = G * planetMass * mass / pow((sphere.getRadius()) + planetRadius, 2);
            // apply force
            double vel = force * dt / mass;
            //sphere.getVelocity().y -= vel;
            double radGrav = gravDirection * PI / 180.0;
            sphere.getVelocity().y -=cos(radGrav) * vel;
            sphere.getVelocity().x +=sin(radGrav) * vel;

        }

        // apply velocity
        for (Sphere &sphere : spheres)
        {
            auto movement = sphere.getVelocity() * dt;
            sphere.move(movement);
        }

        // check collisions
        std::vector<Sphere *> bouncedSpheres;
        for (Sphere &sphere : spheres)
        {
            // check collision with floor
            if (floor.getDistance(sphere.getCenter()) < sphere.getRadius())
            {
                sphere.setFloorNormal(floor.normal);
                // collision
                // calculate reflection vector
                Vec3 reflection = floor.bounce(sphere.getVelocity());
                // reduce velocity, otherwise it will bounce forever
                // only reduce velocity in direction of floor to keep movement
                reflection.y *= floorBounce;
                sphere.setVelocity(reflection);
                // move sphere out of floor
                double dist = floor.getDistance(sphere.getCenter());
                sphere.move(floor.normal * (sphere.getRadius() - dist));
            }

            // check collision with box
            Box &b = box;


            for(size_t i = 0; i < b.getWalls().size(); i++) {

                Wall& wall = b.getWalls()[i];
                if (wall.collide(sphere)) break;
            }

            // check if already bounced
            if (std::find(bouncedSpheres.begin(), bouncedSpheres.end(), &sphere) != bouncedSpheres.end())
                continue;

            for (Sphere &other : spheres)
            {

                // continue if same pointer
                if (&sphere == &other)
                    continue;
                if (sphere.getCenter().getDistance(other.getCenter()) < sphere.getRadius() + other.getRadius())
                {
                    sphere.bounce(other);

                    // add to bounced spheres
                    bouncedSpheres.push_back(&sphere);
                    bouncedSpheres.push_back(&other);
                }
            }
        }

        // remove spheres that are too far away
        for (size_t i = 0; i < spheres.size(); i++)
        {
            Sphere &sphere = spheres[i];
            if (sphere.getCenter().getDistance(Vec3(0, 0, 0)) > 100)
            {
                spheres.erase(spheres.begin() + i);
                i--;
            }
        }

        update();

        // print update every second
        if (frame % fps == 0)
        {
            auto secondsFromFrames = frame / fps;
            int ballCount = spheres.size();
            std::cout << "\rFPS: " << fps << " Frame: " << frame << " Seconds: " << secondsFromFrames << " Spheres: " << ballCount <<"             " << std::flush;
        }
        std::this_thread::sleep_until(lastTime + waitTime);
        frame++;
    }
}

// triggered by ui to add random sphere
void OGLWidget::addSphere()
{

    //std::cout << "addSphere" << std::endl;

    Sphere s(Vec3(0, randRange(0, 5), 0), randRange(0.5,1.2));
    double zVel = randRange(-6, 6);
    double xVel = randRange(-6, 6);
    double yVel = randRange(-1, 6);

    s.setVelocity(Vec3(xVel, yVel, zVel));

    Vec3 color(randRange(0, 1), randRange(0, 1), randRange(0, 1));
    s.setColor(color);

    spheres.push_back(s);
}



// default OGLWidget functions

OGLWidget::OGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    parama = 1;
    paramb = 1;
    paramc = 1;
    lightDirection = 0;

    Wall w(-3, -3, -4, 5);

    // ramp
    Vec3 vectors[4];
    vectors[0] = Vec3(-2, -1, -3);
    vectors[1] = Vec3(4, -1, -3);
    vectors[2] = Vec3(4, 3.3, -7);
    vectors[3] = Vec3(-2, 3.3, -7);

    Wall w2(vectors[0], vectors[1], vectors[2], vectors[3]);

    // wall corners with x1, z1, x2, z2, ...
    std::vector<double> wallCorners = {-8,7,2,8,3,4,1,4,7,-7,-4,-6};
    box = Box(wallCorners);
    box.getWalls().push_back(w);
    box.getWalls().push_back(w2);
}

void OGLWidget::startSim()
{
    std::thread thr([&]
                    { this->runSim(); });
    thr.detach();
}

OGLWidget::~OGLWidget()
{
}

void OGLWidget::setUi(Ui::MainWindow *ui)
{
    this->ui = ui;
}

void OGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    // glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    float light_diffuse_color[] = {0.1, 0.1, 0.1, 0};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse_color);
    glEnable(GL_LIGHT1);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    this->startSim();
}

// used to move corner of wall from ui
void OGLWidget::setWallCornerX(int i) {
    constexpr double minX = 1.0;
    constexpr double maxX = 9.0;
    double x = minX + (maxX - minX) * (double)i / 100.0;

    // add 0.001 to avoid a point where surface = 0
    x+=0.001;

    constexpr size_t cornerIndex = 3;
    // wall 0: corner 0 and 1, wall 1: corner 1 and 2, wall 2: corner 2 and 3, wall 3: corner 3 and 0
    size_t outerCount = box.getOuterWallCount();
    size_t wall1Index = (cornerIndex-1)%outerCount;
    size_t wall2Index = cornerIndex%outerCount;

    Wall& wall1 = box.getWalls()[wall1Index];
    Wall& wall2 = box.getWalls()[wall2Index];

    Vec3* corners[4];
    corners[0] = &wall1.getCorners()[2];
    corners[1] = &wall1.getCorners()[3];
    corners[2] = &wall2.getCorners()[0];
    corners[3] = &wall2.getCorners()[1];

    for (size_t i = 0; i < 4; i++)
    {
        auto& corner = *corners[i];
        corner.x = x;
    }


}

void OGLWidget::setSphereRadius(int idx, int value) {
    if(idx < 0 || static_cast<size_t>(idx) >= spheres.size()) return;
    auto& sphere = spheres[idx];
    double percent = (double)value / 50.0;
    double oldMass = sphere.getMass();
    sphere.setRadius(percent);

    // apply old velocity to new radius
    // E = 1/2 * m * v^2
    // E stays the same, m changes -> v changes
    // 1/2 m1 v1^2 = 1/2 m2 v2^2
    // v2 = v1 * sqrt(m1/m2)
    auto newMass = sphere.getMass();
    auto newVelocity = sphere.getVelocity() * sqrt(oldMass / newMass);
    sphere.setVelocity(newVelocity);
}

void OGLWidget::setSphere1Radius(int i) {
    setSphereRadius(0, i);
}

void OGLWidget::setSphere2Radius(int i) {
    setSphereRadius(1, i);
}

void OGLWidget::setParamA(int newa)
{
    parama = newa / 10.0; // Set new value

    update(); // Trigger redraw of scene with paintGL
}

void OGLWidget::setParamB(int newb)
{
    paramb = newb / 10.0;
    update();
}

void OGLWidget::setParamC(int newc)
{
    paramc = newc / 10.0;
    update();
}

void OGLWidget::setLight(int newlight)
{
    lightDirection = newlight;
    update();
}

void OGLWidget::paintGL()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();

    // rotate with gravity
    //glRotatef(gravDirection, 0, 0, 1);

    glRotatef(-45, 1, 0, 0);
    glRotatef(-135, 0, 1, 0);
    glScaled(0.1, 0.1, 0.1);

    float lightRot = parama * 36;
    glPushMatrix();
    glRotatef(lightRot, 0, 0, 1);

    // light at 0/0/10
    float light_position[] = {10, 5, -10, 0};
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    glEnable(GL_LIGHT1);
    glPopMatrix();

    if(OGLWidget::showAxis) {
        
        // draw 3D axes and grid
        glNormal3f(3, 1, 1);
        glBegin(GL_LINES);
        glColor3f(1, 0, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(10, 0, 0);
        glColor3f(0, 1, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 10, 0);
        glColor3f(0, 0, 1);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 10);

        glEnd();
    }

    box.draw();

    for (Sphere &s : spheres)
    {
        // s.setRadius(paramc);
        s.draw();
    }


    glPopMatrix();
}

bool OGLWidget::showAxis = false;

void OGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    woh = static_cast<double>(w) / static_cast<double>(h);
    glMatrixMode(GL_PROJECTION);
    /*glLoadIdentity();

    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();*/
}
