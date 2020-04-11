#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <qopenglwidget.h>

class VideoReader;
class FrameData;
class QTimer;

class OpenGLWidget : public QOpenGLWidget {

private:
    FrameData *frameData;
    bool validFrame;

private:
    VideoReader *videoReader;

private:
    GLuint tex_handle;

private:
    QTimer *timer;
    bool alreadyDrawn;

public:
    OpenGLWidget(QWidget *parent=nullptr);
    ~OpenGLWidget();

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;

};

#endif // OPENGLWIDGET_H
