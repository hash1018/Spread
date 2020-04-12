#ifndef VIDEOPLAYERWIDGET_H
#define VIDEOPLAYERWIDGET_H

#include <qopenglwidget.h>
#include <qstring.h>

class VideoReader;
class FrameData;
class QTimer;

class VideoPlayerWidget : public QOpenGLWidget {

private:
    FrameData *frameData;
    bool validFrame;

private:
    QString filePath;
    VideoReader *videoReader;

private:
    GLuint tex_handle;

private:
    QTimer *timer;
    bool alreadyDrawn;

public:
    VideoPlayerWidget(const QString &filePath, QWidget *parent=nullptr);
    ~VideoPlayerWidget();

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void moveEvent(QMoveEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;


public:
    inline const QString& getFilePath() const {return this->filePath;}

};

#endif // VIDEOPLAYERWIDGET_H
