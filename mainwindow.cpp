#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // set mainWindow of OGLWidget to this
    ui->glwidget->setUi(ui);

    // Connect slider A to change param A in OGLWidget
    connect(ui->sliderA, SIGNAL(valueChanged(int)), ui->glwidget, SLOT(setParamA(int)));

    // Connect slider B to change param B in OGLWidget
    connect(ui->sliderB, SIGNAL(valueChanged(int)), ui->glwidget, SLOT(setParamB(int)));

    // Connect cornerSlider to change wall corner
    connect(ui->cornerSlider, SIGNAL(valueChanged(int)), ui->glwidget, SLOT(setWallCornerX(int)));


    // Connect addSphereButton to addSphere in OGLWidget
    connect(ui->addSphereButton, SIGNAL(clicked()), ui->glwidget, SLOT(addSphere()));

    // Connect toggleAxisButton to toggleAxis in OGLWidget
    connect(ui->toggleAxisButton, SIGNAL(clicked()), ui->glwidget, SLOT(toggleAxis()));

    // Connect resetSpheresButton
    connect(ui->resetSpheresButton, SIGNAL(clicked()), ui->glwidget, SLOT(resetSpheres()));

    // Connect sphere1Slider to setSphere1Radius in OGLWidget
    connect(ui->sphere1Slider, SIGNAL(valueChanged(int)), ui->glwidget, SLOT(setSphere1Radius(int)));

    // Connect sphere2Slider to setSphere2Radius in OGLWidget
    connect(ui->sphere2Slider, SIGNAL(valueChanged(int)), ui->glwidget, SLOT(setSphere2Radius(int)));

    // COnnect gravDial to setGravity in OGLWidget
    connect(ui->gravDial, SIGNAL(valueChanged(int)), ui->glwidget, SLOT(setGravity(int)));


}

MainWindow::~MainWindow()
{
    delete ui;
}
