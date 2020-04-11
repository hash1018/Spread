
#include "openglwidget.h"
#include <qopenglfunctions.h>
#include <qdebug.h>
#include "src/spread-ffmpeg/videoreader.h"
#include <qtimer.h>
#include "src/spread-ffmpeg/framedata.h"

OpenGLWidget::OpenGLWidget(QWidget *parent)
    :QOpenGLWidget(parent),validFrame(false),alreadyDrawn(false) {


    this->videoReader=new VideoReader;
    this->frameData=new FrameData(600,400);

    if(videoReader->open("/Users/seunghoha/Downloads/Sample Videos 4.mp4")==false){

        qDebug() <<"failed to open video";
        this->validFrame=false;
        return;
    }

    this->validFrame=true;

    this->timer=new QTimer(this);

    connect(this->timer,&QTimer::timeout,this,[this]{

        this->alreadyDrawn=false;
        this->update();
    });

    qDebug() <<" videoReader width " << this->videoReader->getWidth() <<
                " height  " << this->videoReader->getHeight() <<
               " fps " <<this->videoReader->getFps();

    this->videoReader->readFrame(*this->frameData);


    this->timer->setInterval(33);
    this->timer->start();
}


OpenGLWidget::~OpenGLWidget(){

    this->videoReader->close();

    if(this->videoReader!=nullptr)
        delete this->videoReader;

    if(this->frameData!=nullptr)
        delete this->frameData;
}


void OpenGLWidget::initializeGL() {

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

void OpenGLWidget::paintGL() {

    if(this->validFrame==false)
        return;

    if(this->alreadyDrawn==true)
        return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Set up orphographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, this->width(), this->height(),0, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    //Read a new frame and load it into texture
    if(videoReader->readFrame(/*this->frame_data*/ *this->frameData)==false){
        qDebug() <<"failed to read Frame ";
        return;
    }

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,this->frameData->getWidth(),this->frameData->getHeight(),
                     0,GL_RGBA,GL_UNSIGNED_BYTE,this->frameData->getBuffer());

    //Render

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,this->tex_handle);
    glBegin(GL_QUADS);
        glTexCoord2d(0,0); glVertex2i(0,0);
        glTexCoord2d(1,0); glVertex2i(0 + this->frameData->getWidth() * 2 , 0);
        glTexCoord2d(1,1); glVertex2i(0 + this->frameData->getWidth() * 2 , 0 + this->frameData->getHeight() *2 );
        glTexCoord2d(0,1); glVertex2i(0,0 + this->frameData->getHeight() * 2);
    glEnd();
    glDisable(GL_TEXTURE_2D);


    qDebug() << "pts  " << this->frameData->getPts();
    qDebug() <<" ptrRealTime  " << this->frameData->getPtsRealTime();
    qDebug() <<"frameNumber" << this->frameData->getFrameNumber();

    this->alreadyDrawn=true;

}

void OpenGLWidget::resizeGL(int w, int h) {

    Q_UNUSED(w)
    Q_UNUSED(h)
}
