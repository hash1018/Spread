

#include "playvideodialog.h"
#include "videoplayerwidget.h"
#include <qmessagebox.h>

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

    if(this->videoPlayerWidget!=nullptr){

        this->videoPlayerWidget->close();
        delete this->videoPlayerWidget;
    }

    this->videoPlayerWidget=new VideoPlayerWidget(ui.videoFilePathLineEdit->text());
    this->videoPlayerWidget->setGeometry(0,0,600,400);
    this->videoPlayerWidget->show();




}
