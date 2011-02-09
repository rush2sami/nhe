/********************************************************************************
** Form generated from reading UI file 'mainwidget.ui'
**
** Created: Tue Feb 8 22:48:04 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWIDGET_H
#define UI_MAINWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "microphone.h"
#include "videowidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWidget
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label;
    QLineEdit *txtFileOut;
    QPushButton *btnhooseOutFile;
    QHBoxLayout *horizontalLayout;
    QLabel *VideoDevice;
    QLineEdit *txtDevice;
    Microphone *microphoneWidget;
    videowidget *videoViever;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *btnStartStopCapture;
    QPushButton *btnPlay;
    QPushButton *btnExit;

    void setupUi(QWidget *MainWidget)
    {
        if (MainWidget->objectName().isEmpty())
            MainWidget->setObjectName(QString::fromUtf8("MainWidget"));
        MainWidget->resize(710, 600);
        verticalLayout = new QVBoxLayout(MainWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label = new QLabel(MainWidget);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_3->addWidget(label);

        txtFileOut = new QLineEdit(MainWidget);
        txtFileOut->setObjectName(QString::fromUtf8("txtFileOut"));

        horizontalLayout_3->addWidget(txtFileOut);

        btnhooseOutFile = new QPushButton(MainWidget);
        btnhooseOutFile->setObjectName(QString::fromUtf8("btnhooseOutFile"));

        horizontalLayout_3->addWidget(btnhooseOutFile);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        VideoDevice = new QLabel(MainWidget);
        VideoDevice->setObjectName(QString::fromUtf8("VideoDevice"));

        horizontalLayout->addWidget(VideoDevice);

        txtDevice = new QLineEdit(MainWidget);
        txtDevice->setObjectName(QString::fromUtf8("txtDevice"));

        horizontalLayout->addWidget(txtDevice);

        microphoneWidget = new Microphone(MainWidget);
        microphoneWidget->setObjectName(QString::fromUtf8("microphoneWidget"));

        horizontalLayout->addWidget(microphoneWidget);


        verticalLayout->addLayout(horizontalLayout);

        videoViever = new videowidget(MainWidget);
        videoViever->setObjectName(QString::fromUtf8("videoViever"));
        videoViever->setMinimumSize(QSize(320, 240));

        verticalLayout->addWidget(videoViever);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setSizeConstraint(QLayout::SetNoConstraint);
        btnStartStopCapture = new QPushButton(MainWidget);
        btnStartStopCapture->setObjectName(QString::fromUtf8("btnStartStopCapture"));

        horizontalLayout_2->addWidget(btnStartStopCapture);

        btnPlay = new QPushButton(MainWidget);
        btnPlay->setObjectName(QString::fromUtf8("btnPlay"));
        btnPlay->setEnabled(false);

        horizontalLayout_2->addWidget(btnPlay);

        btnExit = new QPushButton(MainWidget);
        btnExit->setObjectName(QString::fromUtf8("btnExit"));

        horizontalLayout_2->addWidget(btnExit);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(MainWidget);

        QMetaObject::connectSlotsByName(MainWidget);
    } // setupUi

    void retranslateUi(QWidget *MainWidget)
    {
        MainWidget->setWindowTitle(QApplication::translate("MainWidget", "qt4lapture WebCam Capture ", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWidget", "File Name:", 0, QApplication::UnicodeUTF8));
        btnhooseOutFile->setText(QApplication::translate("MainWidget", "Choose File", 0, QApplication::UnicodeUTF8));
        VideoDevice->setText(QApplication::translate("MainWidget", "Video Device Path", 0, QApplication::UnicodeUTF8));
        txtDevice->setText(QApplication::translate("MainWidget", "/dev/video0", 0, QApplication::UnicodeUTF8));
        btnStartStopCapture->setText(QApplication::translate("MainWidget", "Start Capture", 0, QApplication::UnicodeUTF8));
        btnPlay->setText(QApplication::translate("MainWidget", "Play", 0, QApplication::UnicodeUTF8));
        btnExit->setText(QApplication::translate("MainWidget", "Exit", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWidget: public Ui_MainWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWIDGET_H
