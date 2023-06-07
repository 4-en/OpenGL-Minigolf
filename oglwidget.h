#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include "simulation.hpp"
#include "minigolf.hpp"

namespace Ui {
class MainWindow;
}

class OGLWidget : public QOpenGLWidget,
                  protected QOpenGLFunctions
{
    Q_OBJECT

public:
    OGLWidget(QWidget *parent = 0);
    ~OGLWidget();
    static bool showAxis;

public slots:
    void setParamA( int newa );
    void setParamB( int newb );
    void setParamC( int newc );
    void setLight( int newlight );
    void setUi( Ui::MainWindow *ui );
    void stopSim() { running = false; }
    void startSim();
    void addSphere();
    void toggleAxis() { showAxis = !showAxis; }
    void resetSpheres() { spheres.clear();}
    void setWallCornerX(int i);
    void setSphere1Radius(int i);
    void setSphere2Radius(int i);
    void setGravity(int i) { gravDirection = i;}

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void runSim();
    bool running = false;
    std::vector<Sphere> spheres = std::vector<Sphere>();
    Box box;
    void setSphereRadius(int idx, int value);

protected:
    double parama;
    double paramb;
    double paramc;
    int gravDirection = 0;
    int lightDirection;
    double woh = 1.0;
    Ui::MainWindow *ui;

};


#endif // OGLWIDGET_H
