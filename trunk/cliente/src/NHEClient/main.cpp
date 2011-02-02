#include "nheclient.h"

#include <QtGui>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NHEClient w;
    w.show();
    return a.exec();
}
