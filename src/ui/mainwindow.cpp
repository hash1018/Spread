
#include "mainwindow.h"
#include "videoplayerwidget.h"
#include "playvideodialog.h"

MainWindow::MainWindow(QWidget *parent)
    :QMainWindow(parent), playVideoDialog(nullptr) {

    ui.setupUi(this);

    connect(ui.playButton,&QPushButton::clicked,this,&MainWindow::playButtonClicked);
}


MainWindow::~MainWindow(){

    if(this->playVideoDialog!=nullptr){

        this->playVideoDialog->close();
        delete this->playVideoDialog;
    }
}

void MainWindow::resizeEvent(QResizeEvent *event){

    QMainWindow::resizeEvent(event);

}

void MainWindow::playButtonClicked(){


    if(this->playVideoDialog!=nullptr){

        this->playVideoDialog->close();
        delete this->playVideoDialog;
    }

    this->playVideoDialog = new PlayVideoDialog;
    this->playVideoDialog->show();
}
