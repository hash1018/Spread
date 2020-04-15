
#include "videoplayerwidget.h"
#include <qopenglfunctions.h>
#include <qdebug.h>
#include "src/spread-ffmpeg/videoreader.h"
#include <qtimer.h>
#include "src/spread-ffmpeg/framedata.h"
#include "src/spread-ffmpeg/hwaccelsdecoder.h"

VideoPlayerWidget::VideoPlayerWidget(const QString &filePath, QWidget *parent)
    :QOpenGLWidget(parent), frameData(nullptr), filePath(filePath),
      videoReader(nullptr),hwAccelsDecoder(nullptr) {


    this->time=0;

    //this->initVideoReader();

    this->initHwDecoder();
}

void VideoPlayerWidget::initVideoReader(){

    this->videoReader=new VideoReader;

    if(videoReader->open(filePath.toUtf8()) == false){

        qDebug() <<"failed to open video";
        return;
    }

    qDebug() <<" videoReader width " << this->videoReader->getWidth() <<
                " height  " << this->videoReader->getHeight() <<
               " fps " <<this->videoReader->getFps() <<
               " totalFrameCount " << this->videoReader->getTotalFrameCount();


    this->frameData=new FrameData(this->videoReader->getWidth(),
                                  this->videoReader->getHeight());

    //this->videoReader->seekFrame(450, *this->frameData);



    this->timer=new QTimer(this);

    connect(this->timer,&QTimer::timeout,this,&VideoPlayerWidget::framePerSecTimePassed);

    this->timer->setInterval(1000/ this->videoReader->getFps());
    this->timer->start();

}

void VideoPlayerWidget::initHwDecoder(){


    this->hwAccelsDecoder=new HwAccelsDecoder;

    if(hwAccelsDecoder->open(filePath.toUtf8()) == false){

        qDebug() <<"failed to open video";
        return;
    }

    qDebug() <<" hwAccelsDecoder width " << this->hwAccelsDecoder->getWidth() <<
                " height  " << this->hwAccelsDecoder->getHeight() <<
               " fps " <<this->hwAccelsDecoder->getFps() <<
               " totalFrameCount " << this->hwAccelsDecoder->getTotalFrameCount();


    this->frameData=new FrameData(this->hwAccelsDecoder->getWidth(),
                                  this->hwAccelsDecoder->getHeight());



    this->timer=new QTimer(this);

    connect(this->timer,&QTimer::timeout,this,&VideoPlayerWidget::framePerSecTimePassedHw);

    this->timer->setInterval(1000/ this->hwAccelsDecoder->getFps());
    this->timer->start();


}


VideoPlayerWidget::~VideoPlayerWidget(){



    if(this->videoReader!=nullptr){
        this->videoReader->close();
        delete this->videoReader;
    }

    if(this->frameData!=nullptr)
        delete this->frameData;

    if(this->hwAccelsDecoder!=nullptr){

        this->hwAccelsDecoder->close();
        delete this->hwAccelsDecoder;
    }
}


void VideoPlayerWidget::initializeGL() {

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    //Generate texture
    glGenTextures(1,&this->tex_handle);
    glBindTexture(GL_TEXTURE_2D, this->tex_handle);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);

}

void VideoPlayerWidget::paintGL() {

    if(this->frameData==nullptr)
        return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Set up orphographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, this->width(), this->height(),0, -1, 1);
    glMatrixMode(GL_MODELVIEW);


    //load frame into texture.
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,this->frameData->getWidth(),this->frameData->getHeight(),
                     0,GL_RGBA,GL_UNSIGNED_BYTE,this->frameData->getBuffer());

    //Render
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,this->tex_handle);
    glBegin(GL_QUADS);
        glTexCoord2d(0,0); glVertex2i(0,0);
        glTexCoord2d(1,0); glVertex2i(0 + this->width() , 0);
        glTexCoord2d(1,1); glVertex2i(0 + this->width() , 0 + this->height() );
        glTexCoord2d(0,1); glVertex2i(0,0 + this->height() );
    glEnd();
    glDisable(GL_TEXTURE_2D);


}

#include <qelapsedtimer.h>
void VideoPlayerWidget::framePerSecTimePassed() {


    QElapsedTimer timer;
    timer.start();
    if(videoReader->readFrame(*this->frameData)==false){
        qDebug() <<"failed to read Frame ";
        this->timer->stop();
        return;
    }

    qint64 elapsed=timer.elapsed();
    this->time+=elapsed;
    qDebug() << " timer elapsed   " << elapsed;

    qDebug() << "pts  " << this->frameData->getPts();
    qDebug() <<" ptrRealTime  " << this->frameData->getPtsRealTime();
    qDebug() <<"frameIndex" << this->frameData->getFrameIndex();

    this->update();

    if(this->frameData->isFinalFrame()==true){

        this->timer->stop();
        qDebug() << " videoReader total time : " <<this->time;
    }

}

void VideoPlayerWidget::framePerSecTimePassedHw() {


    QElapsedTimer timer;
    timer.start();
    if(this->hwAccelsDecoder->readFrame(*this->frameData)==false){
        qDebug() <<"failed to read Frame ";
        this->timer->stop();
        return;
    }

    qint64 elapsed=timer.elapsed();
    this->time+=elapsed;
    qDebug() << " timer elapsed   " << elapsed;

    qDebug() << "pts  " << this->frameData->getPts();
    qDebug() <<" ptrRealTime  " << this->frameData->getPtsRealTime();
    qDebug() <<"frameIndex" << this->frameData->getFrameIndex();

    this->update();

    if(this->frameData->isFinalFrame()==true){

        this->timer->stop();
        qDebug() << " Hw total time : " <<this->time;
    }
}

