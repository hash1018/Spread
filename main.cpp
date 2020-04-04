#include "ui/mainwindow.h"

#include <QApplication>

extern "C" {
#include <libavcodec/avcodec.h>

}

#include <qdebug.h>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << avcodec_configuration();
    MainWindow w;
    w.show();
    return a.exec();
}
