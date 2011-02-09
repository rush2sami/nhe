#include "videowidget.h"
#include <QApplication>

videowidget::videowidget(QWidget *parent) :
    QWidget(parent)
{
    connect(&thread, SIGNAL(renderedImage(QImage)),
                this, SLOT(setPicture(QImage)));

    setAutoFillBackground(true);
}

void videowidget::paintEvent(QPaintEvent */*event*/) {
    QPainter painter(this);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 30));
    painter.drawText(rect(), Qt::AlignCenter, "Qt");

    painter.drawPixmap(this->rect(),pixmap);
    //painter.drawPixmap(((QWidget*)parent())->rect(),pixmap);
}

void videowidget::setPicture(QImage i){
    pixmap=QPixmap::fromImage(i);
    update();    
    qApp->processEvents();
}

videowidget::~videowidget(){

}

void videowidget::setFileName(QString f){
    thread.setFileName(f);
}

void videowidget::setDeviceName(QString f){
    thread.setDeviceName(f);
}
