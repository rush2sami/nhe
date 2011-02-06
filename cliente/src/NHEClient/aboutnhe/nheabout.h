#ifndef NHEABOUT_H
#define NHEABOUT_H

#include <QtGui/QDialog>
#include "ui_nheabout.h"

class NHEAbout : public QDialog
{
    Q_OBJECT

public:
    NHEAbout(QWidget *parent = 0);
    ~NHEAbout();

private:
    Ui::NHEAboutClass ui;
};

#endif // NHEABOUT_H
