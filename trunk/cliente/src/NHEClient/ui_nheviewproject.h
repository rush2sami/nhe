/********************************************************************************
** Form generated from reading UI file 'nheviewproject.ui'
**
** Created: Tue Feb 8 16:49:55 2011
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
#include <QtGui/QGraphicsView>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NHEViewProjectClass
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QListWidget *listWidget;
    QGraphicsView *graphicsView;

    void setupUi(QWidget *NHEViewProjectClass)
    {
        if (NHEViewProjectClass->objectName().isEmpty())
            NHEViewProjectClass->setObjectName(QString::fromUtf8("NHEViewProjectClass"));
        NHEViewProjectClass->resize(759, 300);
        gridLayout = new QGridLayout(NHEViewProjectClass);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(NHEViewProjectClass);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        label_2 = new QLabel(NHEViewProjectClass);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 0, 1, 1, 1);

        label_3 = new QLabel(NHEViewProjectClass);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 0, 2, 1, 1);

        listWidget = new QListWidget(NHEViewProjectClass);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));

        gridLayout->addWidget(listWidget, 1, 0, 1, 1);

        graphicsView = new QGraphicsView(NHEViewProjectClass);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));

        gridLayout->addWidget(graphicsView, 1, 2, 1, 1);


        retranslateUi(NHEViewProjectClass);

        QMetaObject::connectSlotsByName(NHEViewProjectClass);
    } // setupUi

    void retranslateUi(QWidget *NHEViewProjectClass)
    {
        NHEViewProjectClass->setWindowTitle(QApplication::translate("NHEViewProjectClass", "NHEViewProject", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("NHEViewProjectClass", "Usu\303\241rios Conectados", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("NHEViewProjectClass", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("NHEViewProjectClass", "TextLabel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class NHEViewProjectClass: public Ui_NHEViewProjectClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NHEVIEWPROJECT_H
