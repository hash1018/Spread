
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include "ui_mainwindow.h"

class PlayVideoDialog;

class MainWindow : public QMainWindow {

private:
    Ui::MainWindow ui;
    PlayVideoDialog *playVideoDialog;

public:
    MainWindow(QWidget *parent =nullptr);
    ~MainWindow();

protected:
    virtual void resizeEvent(QResizeEvent *event);

private slots:
    void playButtonClicked();
};

#endif // MAINWINDOW_H
