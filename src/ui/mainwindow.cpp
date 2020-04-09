
#include "mainwindow.h"
#include "openglwidget.h"
#include <QResizeEvent>

MainWindow::MainWindow(QWidget *parent)
    :QMainWindow(parent) ,openGLWidget(new OpenGLWidget(this)) {

    this->setWindowTitle("MainWindow");
    this->setMinimumSize(800,600);


}


MainWindow::~MainWindow(){

    if(this->openGLWidget!=nullptr)
        delete this->openGLWidget;
}

void MainWindow::resizeEvent(QResizeEvent *event){

    QMainWindow::resizeEvent(event);

    this->openGLWidget->setGeometry(0,0,event->size().width(),event->size().height());
}
