#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QFileDialog>
#include <QWidget>
#include <QDebug>
#include "videowidget.h"

namespace Ui {
    class MainWidget;
}

class MainWidget : public QWidget {
    Q_OBJECT
public:
    MainWidget(QWidget *parent = 0);
    ~MainWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWidget *ui;

private slots:
    void on_btnStartStopCapture_clicked();
    void on_btnhooseOutFile_clicked();
    void on_btnExit_clicked();
    void videoDidnotStart();
    void soundGeldi();
};

#endif // MAINWIDGET_H
