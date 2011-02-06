#ifndef NHEVIEWPROJECT_H
#define NHEVIEWPROJECT_H

#include <QtGui/QWidget>
#include "ui_nheviewproject.h"

class NHEViewProject : public QWidget
{
    Q_OBJECT

public:
    NHEViewProject(QWidget *parent = 0);
    ~NHEViewProject();

private:
    Ui::NHEViewProjectClass ui;
};

#endif // NHEVIEWPROJECT_H
