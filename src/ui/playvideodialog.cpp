

#include "playvideodialog.h"
#include "videoplayerwidget.h"
#include <qmessagebox.h>


#include "src/spread-ffmpeg/hwaccelsdecoder.h"
#include <qdebug.h>
#include "src/spread-ffmpeg/framedata.h"

PlayVideoDialog::PlayVideoDialog(QWidget *parent)
    :QDialog(parent) , videoPlayerWidget(nullptr) {

    ui.setupUi(this);

    connect(ui.searchVideoFileButton,&QPushButton::clicked,this,&PlayVideoDialog::searchVideoFileButtonClicked);
    connect(ui.playButton,&QPushButton::clicked,this,&PlayVideoDialog::playButtonClicked);
}

PlayVideoDialog::~PlayVideoDialog(){

    if(this->videoPlayerWidget!=nullptr){

        delete this->videoPlayerWidget;
    }

}


#include <qfiledialog.h>
void PlayVideoDialog::searchVideoFileButtonClicked(){


    QFileDialog dialog;
    dialog.setNameFilter(tr("Videos (*.mp4 *.avi *.xmv)"));
    dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);

    if(dialog.exec()==QFileDialog::Accepted){


        ui.videoFilePathLineEdit->setText(dialog.selectedFiles().at(0));

    }
}

void PlayVideoDialog::playButtonClicked(){

    if(ui.videoFilePathLineEdit->text()==""){

        QMessageBox box;
        box.setText("empty");
        box.exec();
        return ;
    }


    QFile file(ui.videoFilePathLineEdit->text());

    if(file.exists()==false){

        QMessageBox box;
        box.setText("doesn't exist video File ");
        box.exec();
        return;

    }

/*
    HwAccelsDecoder *decoder=new  HwAccelsDecoder;

    if(decoder->open(ui.videoFilePathLineEdit->text().toUtf8()) == false){

        qDebug() <<"asdsad";
    }
    else{

        qDebug() <<" kkk";


        FrameData data(decoder->getWidth(),decoder->getHeight());

        bool result;
        do{


            result = decoder->readFrame(data);
            qDebug() << " frameNumber " << data.getFrameIndex();
            qDebug() << " pts " <<  data.getPts();
            qDebug() << " pts real time " << data.getPtsRealTime();

        }while(data.isFinalFrame()!=true && result== true);

    }
*/


    if(this->videoPlayerWidget!=nullptr){

        this->videoPlayerWidget->close();
        delete this->videoPlayerWidget;
    }

    this->videoPlayerWidget=new VideoPlayerWidget(ui.videoFilePathLineEdit->text());
    this->videoPlayerWidget->setGeometry(0,0,600,400);
    this->videoPlayerWidget->show();




}
