
#include "openglwidget.h"
#include <qopenglfunctions.h>
#include "src/spread-ffmpeg/loadframe.h"
#include <qdebug.h>

OpenGLWidget::OpenGLWidget(QWidget *parent)
    :QOpenGLWidget(parent),validFrame(false) {

    bool result=loadFrame("/Users/seunghoha/Downloads/Sample Videos 4.mp4",&this->frame_width,&this->frame_height,&this->frame_data);

    if(result==false){
        qDebug() << "load video frame Failed\n";
        this->validFrame=false;
    }
    else{

        this->validFrame=true;
    }

}


OpenGLWidget::~OpenGLWidget(){


}


void OpenGLWidget::initializeGL() {

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void OpenGLWidget::paintGL() {

    if(this->validFrame==false)
        return;

    GLuint tex_handle;
    glGenTextures(1,&tex_handle);
    glBindTexture(GL_TEXTURE_2D, tex_handle);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,this->frame_width,this->frame_height,0,GL_RGBA,GL_UNSIGNED_BYTE,this->frame_data);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glfwGetFramebufferSize(window, &window_width, &window_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, this->width(), this->height(),0, -1, 1);
    glMatrixMode(GL_MODELVIEW);


    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,tex_handle);
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
