/********************************************************************************
** Form generated from reading UI file 'nheserver.ui'
**
** Created: Sat Feb 5 22:31:09 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NHESERVER_H
#define UI_NHESERVER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NHEServerClass
{
public:

    void setupUi(QWidget *NHEServerClass)
    {
        if (NHEServerClass->objectName().isEmpty())
            NHEServerClass->setObjectName(QString::fromUtf8("NHEServerClass"));
        NHEServerClass->resize(400, 300);

        retranslateUi(NHEServerClass);

        QMetaObject::connectSlotsByName(NHEServerClass);
    } // setupUi

    void retranslateUi(QWidget *NHEServerClass)
    {
        NHEServerClass->setWindowTitle(QApplication::translate("NHEServerClass", "NHEServer", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class NHEServerClass: public Ui_NHEServerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NHESERVER_H
