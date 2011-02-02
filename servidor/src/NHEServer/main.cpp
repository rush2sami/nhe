#include "nheserver.h"

#include <QtGui>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NHEServer w;
    w.show();
    return a.exec();
}
