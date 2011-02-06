/********************************************************************************
** Form generated from reading UI file 'nheclient.ui'
**
** Created: Sat Feb 5 21:49:24 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NHECLIENT_H
#define UI_NHECLIENT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NHEClientClass
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *NHEClientClass)
    {
        if (NHEClientClass->objectName().isEmpty())
            NHEClientClass->setObjectName(QString::fromUtf8("NHEClientClass"));
        NHEClientClass->resize(800, 600);
        centralwidget = new QWidget(NHEClientClass);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        NHEClientClass->setCentralWidget(centralwidget);
        menubar = new QMenuBar(NHEClientClass);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 21));
        NHEClientClass->setMenuBar(menubar);
        statusbar = new QStatusBar(NHEClientClass);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        NHEClientClass->setStatusBar(statusbar);

        retranslateUi(NHEClientClass);

        QMetaObject::connectSlotsByName(NHEClientClass);
    } // setupUi

    void retranslateUi(QMainWindow *NHEClientClass)
    {
        NHEClientClass->setWindowTitle(QApplication::translate("NHEClientClass", "MainWindow", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class NHEClientClass: public Ui_NHEClientClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NHECLIENT_H
