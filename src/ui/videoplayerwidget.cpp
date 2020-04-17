
#include "videoplayerwidget.h"
#include <qopenglfunctions.h>
#include <qdebug.h>
#include "src/spread-ffmpeg/videoreader.h"
#include <qtimer.h>
#include "src/spread-ffmpeg/framedata.h"
#include "src/spread-ffmpeg/hwaccelsdecoder.h"
#include <qopenglshaderprogram.h>
#include <qopengltexture.h>

GLuint          m_textureIds[3];
const char g_indices[] = { 0, 3, 2, 0, 2, 1 };
const char g_vertextShader[] = { "attribute vec4 aPosition;\n"
                                 "attribute vec2 aTextureCoord;\n"
                                 "varying vec2 vTextureCoord;\n"
                                 "void main() {\n"
                                 "  gl_Position = aPosition;\n"
                                 "  vTextureCoord = aTextureCoord;\n"
                                 "}\n" };
const char g_fragmentShader[] = {
    "uniform sampler2D Ytex,Utex,Vtex;\n"
    "varying vec2 vTextureCoord;\n"
    "void main(void) {\n"
    "vec3 yuv;\n"
    "vec3 rgb;\n"
    "yuv.x = texture2D(Ytex, vTextureCoord).r;\n"
    "yuv.y = texture2D(Utex, vTextureCoord).r - 0.5;\n"
    "yuv.z = texture2D(Vtex, vTextureCoord).r - 0.5;\n"
    "rgb = mat3(1,1,1,\n"
    "0,-0.39465,2.03211,\n"
    "1.13983, -0.58060,  0) * yuv;\n"
    "gl_FragColor = vec4(rgb, 1);\n"
    "}\n" };


GLfloat m_verticesA[20] = { 1, 1, 0, 1, 0, -1, 1, 0, 0, 0, -1, -1, 0, 0,
                            1, 1, -1, 0, 1, 1, };



VideoPlayerWidget::VideoPlayerWidget(const QString &filePath, QWidget *parent)
    :QOpenGLWidget(parent), frameData(nullptr), filePath(filePath),
      videoReader(nullptr),hwAccelsDecoder(nullptr) {


    this->time=0;

    this->initVideoReader();

    //this->initHwDecoder();
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

    //this->timer->setInterval(1000/ this->videoReader->getFps());
    this->timer->setInterval(10);
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


    /*
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

    */

    if(this->frameData==nullptr){

        qDebug()<< "sdssdsdsdsdsdsdsd";
        return;

    }


    this->initializeOpenGLFunctions();

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glEnable(GL_TEXTURE_2D);

    /* The following three commented lines if uncommented generate following errors:
         * undefined reference to `_imp__glShadeModel@4'
         * undefined reference to `_imp__glClearDepth@8'
         * bad reloc address 0x20 in section `.eh_frame'
         */
    // glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    // glClearDepth(1.0f);
    // glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


    this->vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
    this->vertexShader->compileSourceCode(g_vertextShader);
    this->fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
    this->fragmentShader->compileSourceCode(g_fragmentShader);
    this->shaderProgram = new QOpenGLShaderProgram;
    this->shaderProgram->addShader(this->vertexShader);
    this->shaderProgram->addShader(this->fragmentShader);
    this->shaderProgram->link();

    int positionHandle=this->shaderProgram->attributeLocation("aPosition");
    int textureHandle=this->shaderProgram->attributeLocation("aTextureCoord");

    glVertexAttribPointer(positionHandle, 3, GL_FLOAT, false,
                          5 * sizeof(GLfloat), m_verticesA);

    glEnableVertexAttribArray(positionHandle);

    glVertexAttribPointer(textureHandle, 2, GL_FLOAT, false,
                          5 * sizeof(GLfloat), &m_verticesA[3]);

    glEnableVertexAttribArray(textureHandle);

    int i=this->shaderProgram->uniformLocation("Ytex");
    glUniform1i(i, 0);
    i=this->shaderProgram->uniformLocation("Utex");
    glUniform1i(i, 1);
    i=this->shaderProgram->uniformLocation("Vtex");
    glUniform1i(i, 2);


    ///////////////////////////////////////////////////////////////////////////




    // UUU
    glActiveTexture(GL_TEXTURE1);
    glGenTextures(2,&m_textureIds[1]);
    glBindTexture(GL_TEXTURE_2D, m_textureIds[1]);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // This is necessary for non-power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glEnable(GL_TEXTURE_2D);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_LUMINANCE,
                 this->frameData->getWidth() / 2,
                 this->frameData->getHeight() / 2,
                 0,
                 GL_LUMINANCE,
                 GL_UNSIGNED_BYTE,
                 NULL);




    // VVV
    glActiveTexture(GL_TEXTURE2);
    glGenTextures(3,&m_textureIds[2]);
    glBindTexture(GL_TEXTURE_2D, m_textureIds[2]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // This is necessary for non-power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glEnable(GL_TEXTURE_2D);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_LUMINANCE,
                 this->frameData->getWidth() / 2,
                 this->frameData->getHeight() / 2,
                 0,
                 GL_LUMINANCE,
                 GL_UNSIGNED_BYTE,
                 NULL);






    // YYY
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1,&m_textureIds[0]);
    glBindTexture(GL_TEXTURE_2D, m_textureIds[0]);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // This is necessary for non-power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glEnable(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,this->frameData->getWidth(),this->frameData->getHeight(),
                     0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);






}

void VideoPlayerWidget::paintGL() {

    if(this->frameData==nullptr)
        return;


    /*
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Set up orphographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, this->width(), this->height(),0, -1, 1);
    glMatrixMode(GL_MODELVIEW);


    glBindTexture(GL_TEXTURE_2D,this->tex_handle);
    //load frame into texture.
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB , this->frameData->getWidth(), this->frameData->getHeight(),
    //                 0, GL_RGBA, GL_UNSIGNED_BYTE, this->frameData->getBuffer());

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, this->frameData->getWidth(), this->frameData->getHeight(),
                     0, GL_LUMINANCE, GL_UNSIGNED_BYTE, this->frameData->getBuffer());
    glBindTexture(GL_TEXTURE_2D,0);

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

    glBindTexture(GL_TEXTURE_2D,0);

    */



    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //glOrtho(0, this->width(), this->height(),0, -1, 1);
    //glMatrixMode(GL_MODELVIEW);

    this->shaderProgram->bind();
    int width=this->frameData->getWidth();
    int height=this->frameData->getHeight();

    int _idxU = width * height;
    int _idxV = _idxU + (_idxU / 4);


    uint8_t *buffer=const_cast<uint8_t*>(this->frameData->getBuffer());




    //  UUUUUUUUU
    int i=this->shaderProgram->uniformLocation("Utex");
    glUniform1i(i, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_textureIds[1]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE,
                    GL_UNSIGNED_BYTE, &buffer[_idxU]);





    //   VVVVVVVVVVVV
    i=this->shaderProgram->uniformLocation("Vtex");
    glUniform1i(i, 2);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_textureIds[2]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE,
                    GL_UNSIGNED_BYTE, &buffer[_idxV]);







    //    YYYYYYYYYYYY
    i=this->shaderProgram->uniformLocation("Ytex");
    glUniform1i(i, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureIds[0]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE,
                    GL_UNSIGNED_BYTE, buffer);







    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, g_indices);
    this->shaderProgram->release();





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

