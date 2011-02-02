#ifndef NHECLIENT_H
#define NHECLIENT_H

#include <QtGui/QWidget>
#include "ui_nheclient.h"

class NHEClient : public QWidget
{
    Q_OBJECT

public:
    NHEClient(QWidget *parent = 0);
    ~NHEClient();

private:
    Ui::NHEClientClass ui;
};

#endif // NHECLIENT_H
