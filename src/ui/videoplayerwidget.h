#ifndef VIDEOPLAYERWIDGET_H
#define VIDEOPLAYERWIDGET_H

#include <qopenglwidget.h>
#include <qstring.h>
#include <qopenglbuffer.h>
#include <QOpenGLFunctions>

class VideoReader;
class FrameData;
class QTimer;


class HwAccelsDecoder;
class QOpenGLTexture;
class QOpenGLShader;
class QOpenGLShaderProgram;

class VideoPlayerWidget : public QOpenGLWidget, protected QOpenGLFunctions {

private:
    FrameData *frameData;

private:
    QString filePath;
    VideoReader *videoReader;
    HwAccelsDecoder *hwAccelsDecoder;

private:
    //GLuint tex_handle;


private:
    QOpenGLShader *vertexShader;
    QOpenGLShader *fragmentShader;
    QOpenGLShaderProgram *shaderProgram;
    QOpenGLBuffer *vbo;
    QOpenGLTexture *texture;

private:
    QTimer *timer;
    int time;

public:
    VideoPlayerWidget(const QString &filePath, QWidget *parent=nullptr);
    ~VideoPlayerWidget();

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;

public:
    inline const QString& getFilePath() const {return this->filePath;}

private slots:
    void framePerSecTimePassed();
    void framePerSecTimePassedHw();

private:
    void initVideoReader();
    void initHwDecoder();

};

#endif // VIDEOPLAYERWIDGET_H
