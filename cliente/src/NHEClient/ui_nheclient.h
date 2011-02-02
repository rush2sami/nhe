/********************************************************************************
** Form generated from reading UI file 'nheclient.ui'
**
** Created: Wed 2. Feb 14:48:22 2011
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
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NHEClientClass
{
public:

    void setupUi(QWidget *NHEClientClass)
    {
        if (NHEClientClass->objectName().isEmpty())
            NHEClientClass->setObjectName(QString::fromUtf8("NHEClientClass"));
        NHEClientClass->resize(400, 300);

        retranslateUi(NHEClientClass);

        QMetaObject::connectSlotsByName(NHEClientClass);
    } // setupUi

    void retranslateUi(QWidget *NHEClientClass)
    {
        NHEClientClass->setWindowTitle(QApplication::translate("NHEClientClass", "NHEClient", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class NHEClientClass: public Ui_NHEClientClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NHECLIENT_H
