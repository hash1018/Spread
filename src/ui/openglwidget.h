#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <qopenglwidget.h>

class OpenGLWidget : public QOpenGLWidget {

public:
    OpenGLWidget(QWidget *parent=nullptr);
    ~OpenGLWidget();


};

#endif // OPENGLWIDGET_H
