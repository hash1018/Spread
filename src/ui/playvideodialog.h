

#ifndef PLAYVIDEODIALOG_H
#define PLAYVIDEODIALOG_H


#include <qdialog.h>
#include "ui_playvideodialog.h"

class VideoPlayerWidget;

class PlayVideoDialog : public QDialog {

private:
    Ui::playVideoDialog ui;
    VideoPlayerWidget *videoPlayerWidget;

public:
    PlayVideoDialog(QWidget *parent=nullptr);
    ~PlayVideoDialog();


private slots:
    void playButtonClicked();
    void searchVideoFileButtonClicked();
};

#endif // PLAYVIDEODIALOG_H
