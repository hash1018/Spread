#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <qopenglwidget.h>

class VideoReader;
class QTimer;

class OpenGLWidget : public QOpenGLWidget {

private:
    uint8_t *frame_data;
    int frame_width;
    int frame_height;
    bool validFrame;

private:
    VideoReader *videoReader;

private:
    GLuint tex_handle;

private:
    QTimer *timer;

public:
    OpenGLWidget(QWidget *parent=nullptr);
    ~OpenGLWidget();

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;

};

#endif // OPENGLWIDGET_H
