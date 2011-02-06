/********************************************************************************
** Form generated from reading UI file 'nheviewproject.ui'
**
** Created: Sat Feb 5 22:51:10 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NHEVIEWPROJECT_H
#define UI_NHEVIEWPROJECT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NHEViewProjectClass
{
public:
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;

    void setupUi(QWidget *NHEViewProjectClass)
    {
        if (NHEViewProjectClass->objectName().isEmpty())
            NHEViewProjectClass->setObjectName(QString::fromUtf8("NHEViewProjectClass"));
        NHEViewProjectClass->resize(400, 300);
        horizontalLayoutWidget = new QWidget(NHEViewProjectClass);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(160, 120, 160, 80));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);

        retranslateUi(NHEViewProjectClass);

        QMetaObject::connectSlotsByName(NHEViewProjectClass);
    } // setupUi

    void retranslateUi(QWidget *NHEViewProjectClass)
    {
        NHEViewProjectClass->setWindowTitle(QApplication::translate("NHEViewProjectClass", "NHEViewProject", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class NHEViewProjectClass: public Ui_NHEViewProjectClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NHEVIEWPROJECT_H
