/********************************************************************************
** Form generated from reading UI file 'nheabout.ui'
**
** Created: Sat Feb 5 22:40:41 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NHEABOUT_H
#define UI_NHEABOUT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class Ui_NHEAboutClass
{
public:
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QLabel *label;

    void setupUi(QDialog *NHEAboutClass)
    {
        if (NHEAboutClass->objectName().isEmpty())
            NHEAboutClass->setObjectName(QString::fromUtf8("NHEAboutClass"));
        NHEAboutClass->resize(400, 300);
        horizontalLayout_2 = new QHBoxLayout(NHEAboutClass);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_2 = new QLabel(NHEAboutClass);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout->addWidget(label_2);

        label = new QLabel(NHEAboutClass);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);


        horizontalLayout_2->addLayout(horizontalLayout);


        retranslateUi(NHEAboutClass);

        QMetaObject::connectSlotsByName(NHEAboutClass);
    } // setupUi

    void retranslateUi(QDialog *NHEAboutClass)
    {
        NHEAboutClass->setWindowTitle(QApplication::translate("NHEAboutClass", "NHEAbout", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("NHEAboutClass", "TextLabel", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("NHEAboutClass", "TextLabel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class NHEAboutClass: public Ui_NHEAboutClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NHEABOUT_H
