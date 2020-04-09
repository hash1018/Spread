
#include "openglwidget.h"
#include <qopenglfunctions.h>
#include "src/spread-ffmpeg/loadframe.h"
#include <qdebug.h>
#include "src/spread-ffmpeg/videoreader.h"
#include <qtimer.h>

OpenGLWidget::OpenGLWidget(QWidget *parent)
    :QOpenGLWidget(parent),validFrame(false) {
/*
    bool result=loadFrame("/Users/seunghoha/Downloads/Sample Videos 4.mp4",&this->frame_width,&this->frame_height,&this->frame_data);

    if(result==false){
        qDebug() << "load video frame Failed\n";
        this->validFrame=false;
    }
    else{

        this->validFrame=true;
    }
    */

    this->videoReader=new VideoReader;

    if(videoReader->open("/Users/seunghoha/Downloads/Sample Videos 4.mp4")==false){

        qDebug() <<"failed to open video";
        this->validFrame=false;
        return;
    }

    this->frame_width=videoReader->getWidth();
    this->frame_height=videoReader->getHeight();
    this->frame_data= new uint8_t[this->frame_width * this->frame_height * 4 ];


    this->validFrame=true;

    this->timer=new QTimer(this);

    connect(this->timer,&QTimer::timeout,this,[this]{

        this->update();
    });

    this->timer->setInterval(30);
    this->timer->start();
}


OpenGLWidget::~OpenGLWidget(){


    this->videoReader->close();

    if(this->videoReader!=nullptr)
        delete this->videoReader;
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

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Set up orphographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, this->width(), this->height(),0, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    //Read a new frame and load it into texture
    if(videoReader->readFrame(this->frame_data)==false){
        qDebug() <<"failed to read Frame ";
        return;
    }

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,this->frame_width,this->frame_height,
                 0,GL_RGBA,GL_UNSIGNED_BYTE,this->frame_data);

    //Render

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,this->tex_handle);
    glBegin(GL_QUADS);
        glTexCoord2d(0,0); glVertex2i(0,0);
        glTexCoord2d(1,0); glVertex2i(0 + this->frame_width * 2 , 0);
        glTexCoord2d(1,1); glVertex2i(0 + this->frame_width * 2 , 0 + this->frame_height *2 );
        glTexCoord2d(0,1); glVertex2i(0,0 + this->frame_height * 2);
    glEnd();
    glDisable(GL_TEXTURE_2D);

}

void OpenGLWidget::resizeGL(int w, int h) {

    Q_UNUSED(w)
    Q_UNUSED(h)
}
