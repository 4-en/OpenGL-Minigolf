#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include "simulation.hpp"
#include "minigolf.hpp"

#include <QMouseEvent>

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

    // Used to rotate object by mouse
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    // Used to rotate object by keys (requires focus for OpenGL widget)
    void keyPressEvent(QKeyEvent *event);

public slots:
    void setParamA( int newa );
    void setParamB( int newb );
    void setParamC( int newc );
    void setLight( int newlight );
    void setUi( Ui::MainWindow *ui );
    void stopSim() { running = false; }
    void startSim();
    void toggleAxis() { showAxis = !showAxis; }
    void setGravity(int i) { gravDirection = i;}

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void runSim();
    bool running = false;
    golf::Game game;
    void setSphereRadius(int idx, int value);
    Vec3 screenToWorld(int x, int y);
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 modelViewMatrix;
    GLint viewport[4];

protected:
    double parama;
    double paramb;
    double paramc;
    int gravDirection = 0;
    int lightDirection;
    double woh = 1.0;
    Ui::MainWindow *ui;
    Vec3 lastMousePos;
    float startx;
    float starty;
    float distance;

};


#endif // OGLWIDGET_H
