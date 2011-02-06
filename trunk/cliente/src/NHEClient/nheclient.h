#ifndef NHECLIENT_H
#define NHECLIENT_H

#include <QtGui/QMainWindow>
#include "ui_nheclient.h"

class NHEClient : public QMainWindow
{
    Q_OBJECT

public:
    NHEClient(QWidget *parent = 0);
    ~NHEClient();

private:
    Ui::NHEClientClass ui;
};

#endif // NHECLIENT_H
