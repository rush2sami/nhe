#ifndef NHESERVER_H
#define NHESERVER_H

#include <QtGui/QWidget>
#include "ui_nheserver.h"

class NHEServer : public QWidget
{
    Q_OBJECT

public:
    NHEServer(QWidget *parent = 0);
    ~NHEServer();

private:
    Ui::NHEServerClass ui;
};

#endif // NHESERVER_H
