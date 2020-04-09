#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>

class OpenGLWidget;

class MainWindow : public QMainWindow {

private:
    OpenGLWidget *openGLWidget;

public:
    MainWindow(QWidget *parent =nullptr);
    ~MainWindow();

protected:
    virtual void resizeEvent(QResizeEvent *event);

};

#endif // MAINWINDOW_H
