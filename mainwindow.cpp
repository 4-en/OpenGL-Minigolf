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

}

MainWindow::~MainWindow()
{
    delete ui;
}
