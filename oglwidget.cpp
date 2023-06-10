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
        for (golf::Player& player : game.getPlayers())
        {
            Sphere& sphere = player.getBall();
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
        for (golf::Player& player : game.getPlayers())
        {
            Sphere& sphere = player.getBall();
            auto movement = sphere.getVelocity() * dt;
            sphere.move(movement);
        }

        // check collisions
        std::vector<Sphere *> bouncedSpheres;
        for (golf::Player& player : game.getPlayers())
        {
            Sphere& sphere = player.getBall();
            // check collision with floor
            if (floor.getDistance(sphere.getPosition()) < sphere.getRadius())
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
                double dist = floor.getDistance(sphere.getPosition());
                sphere.move(floor.normal * (sphere.getRadius() - dist));
            }

            // check collision with golf objects
            game.collide(sphere);

            // check if already bounced
            if (std::find(bouncedSpheres.begin(), bouncedSpheres.end(), &sphere) != bouncedSpheres.end())
                continue;

            for (golf::Player& player : game.getPlayers())
            {
                Sphere& other = player.getBall();

                // continue if same pointer
                if (&sphere == &other)
                    continue;
                if (sphere.getPosition().getDistance(other.getPosition()) < sphere.getRadius() + other.getRadius())
                {
                    sphere.bounce(other);

                    // add to bounced spheres
                    bouncedSpheres.push_back(&sphere);
                    bouncedSpheres.push_back(&other);
                }
            }
        }



        update();

        // print update every second
        if (frame % fps == 0)
        {
            auto secondsFromFrames = frame / fps;
            std::cout << "\rFPS: " << fps << " Frame: " << frame << " Seconds: " << secondsFromFrames <<"             " << std::flush;
        }
        std::this_thread::sleep_until(lastTime + waitTime);
        frame++;
    }
}



// default OGLWidget functions

OGLWidget::OGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    parama = 1;
    paramb = 1;
    paramc = 1;
    lightDirection = 0;

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

    game.draw();

    for (golf::Player& player : game.getPlayers())
    {
        // s.setRadius(paramc);
        player.getBall().draw();
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

void OGLWidget::mousePressEvent(QMouseEvent *event)
{
    // Upon mouse pressed, we store the current position...
    lastMousePos.x = event->x();
    lastMousePos.y = event->y();

}

void OGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    // ... and while moving, we calculate the dragging deltas
    // Left button: Rotating around x and y axis
    int rx = (event->buttons() & Qt::LeftButton) ? lastMousePos.y - event->y() : 0;
    int ry = lastMousePos.x - event->x();
    // Right button: Rotating around z and y axis
    int rz = (event->buttons() & Qt::RightButton) ? lastMousePos.y - event->y() : 0;

    
    lastMousePos.x = event->x();
    lastMousePos.y = event->y();
    std::cout << "X: " <<lastMousePos.x << ", Y: " << lastMousePos.y << std::endl;
}

void OGLWidget::keyPressEvent(QKeyEvent *event)
{

    switch(event->key())
    {
        // Up/Down: Rotating around x axis
        case Qt::Key_Up:
            break;

        // All other will be ignored
        default:
            break;
    }
}
