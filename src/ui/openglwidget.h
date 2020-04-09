#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <qopenglwidget.h>

class OpenGLWidget : public QOpenGLWidget {

private:
    unsigned char *frame_data;
    int frame_width;
    int frame_height;
    bool validFrame;

public:
    OpenGLWidget(QWidget *parent=nullptr);
    ~OpenGLWidget();

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;

};

#endif // OPENGLWIDGET_H
