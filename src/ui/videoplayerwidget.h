#ifndef VIDEOPLAYERWIDGET_H
#define VIDEOPLAYERWIDGET_H

#include <qopenglwidget.h>
#include <qstring.h>

class VideoReader;
class FrameData;
class QTimer;


class HwAccelsDecoder;

class VideoPlayerWidget : public QOpenGLWidget {

private:
    FrameData *frameData;

private:
    QString filePath;
    VideoReader *videoReader;
    HwAccelsDecoder *hwAccelsDecoder;

private:
    GLuint tex_handle;

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
