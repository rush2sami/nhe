/********************************************************************************
** Form generated from reading UI file 'nheclient.ui'
**
** Created: Sat Feb 5 22:48:42 2011
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
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTreeWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NHEClientClass
{
public:
    QAction *actionSobre_QT;
    QAction *actionSobre_NHE;
    QAction *actionAtualizar;
    QAction *actionNovo_Projeto;
    QAction *actionInscrever_se;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QGridLayout *gridLayout;
    QLabel *label;
    QTreeWidget *treeWidget;
    QListWidget *listWidget;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButton;
    QPushButton *pushButton_3;
    QPushButton *pushButton_2;
    QMenuBar *menubar;
    QMenu *menuArquivo;
    QMenu *menuAjuda;
    QMenu *menuAjuda_2;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *NHEClientClass)
    {
        if (NHEClientClass->objectName().isEmpty())
            NHEClientClass->setObjectName(QString::fromUtf8("NHEClientClass"));
        NHEClientClass->resize(800, 258);
        actionSobre_QT = new QAction(NHEClientClass);
        actionSobre_QT->setObjectName(QString::fromUtf8("actionSobre_QT"));
        actionSobre_NHE = new QAction(NHEClientClass);
        actionSobre_NHE->setObjectName(QString::fromUtf8("actionSobre_NHE"));
        actionAtualizar = new QAction(NHEClientClass);
        actionAtualizar->setObjectName(QString::fromUtf8("actionAtualizar"));
        actionNovo_Projeto = new QAction(NHEClientClass);
        actionNovo_Projeto->setObjectName(QString::fromUtf8("actionNovo_Projeto"));
        actionInscrever_se = new QAction(NHEClientClass);
        actionInscrever_se->setObjectName(QString::fromUtf8("actionInscrever_se"));
        centralwidget = new QWidget(NHEClientClass);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        treeWidget = new QTreeWidget(centralwidget);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));

        gridLayout->addWidget(treeWidget, 1, 0, 1, 1);

        listWidget = new QListWidget(centralwidget);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));

        gridLayout->addWidget(listWidget, 1, 1, 1, 1);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 0, 1, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        horizontalLayout_2->addWidget(pushButton);

        pushButton_3 = new QPushButton(centralwidget);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));

        horizontalLayout_2->addWidget(pushButton_3);

        pushButton_2 = new QPushButton(centralwidget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));

        horizontalLayout_2->addWidget(pushButton_2);


        gridLayout->addLayout(horizontalLayout_2, 3, 0, 1, 1);


        horizontalLayout->addLayout(gridLayout);

        NHEClientClass->setCentralWidget(centralwidget);
        menubar = new QMenuBar(NHEClientClass);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 25));
        menuArquivo = new QMenu(menubar);
        menuArquivo->setObjectName(QString::fromUtf8("menuArquivo"));
        menuAjuda = new QMenu(menubar);
        menuAjuda->setObjectName(QString::fromUtf8("menuAjuda"));
        menuAjuda_2 = new QMenu(menubar);
        menuAjuda_2->setObjectName(QString::fromUtf8("menuAjuda_2"));
        NHEClientClass->setMenuBar(menubar);
        statusbar = new QStatusBar(NHEClientClass);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        NHEClientClass->setStatusBar(statusbar);

        menubar->addAction(menuArquivo->menuAction());
        menubar->addAction(menuAjuda->menuAction());
        menubar->addAction(menuAjuda_2->menuAction());
        menuArquivo->addAction(actionAtualizar);
        menuAjuda->addAction(actionNovo_Projeto);
        menuAjuda->addAction(actionInscrever_se);
        menuAjuda_2->addSeparator();
        menuAjuda_2->addAction(actionSobre_QT);
        menuAjuda_2->addAction(actionSobre_NHE);

        retranslateUi(NHEClientClass);

        QMetaObject::connectSlotsByName(NHEClientClass);
    } // setupUi

    void retranslateUi(QMainWindow *NHEClientClass)
    {
        NHEClientClass->setWindowTitle(QApplication::translate("NHEClientClass", "MainWindow", 0, QApplication::UnicodeUTF8));
        actionSobre_QT->setText(QApplication::translate("NHEClientClass", "Sobre &QT", 0, QApplication::UnicodeUTF8));
        actionSobre_NHE->setText(QApplication::translate("NHEClientClass", "Sobre &NHE", 0, QApplication::UnicodeUTF8));
        actionAtualizar->setText(QApplication::translate("NHEClientClass", "Atualizar", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionAtualizar->setStatusTip(QApplication::translate("NHEClientClass", "Atualizar os dados", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionNovo_Projeto->setText(QApplication::translate("NHEClientClass", "&Novo Projeto", 0, QApplication::UnicodeUTF8));
        actionInscrever_se->setText(QApplication::translate("NHEClientClass", "&Inscrever-se", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("NHEClientClass", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("NHEClientClass", "TextLabel", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("NHEClientClass", "PushButton", 0, QApplication::UnicodeUTF8));
        pushButton_3->setText(QApplication::translate("NHEClientClass", "PushButton", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("NHEClientClass", "PushButton", 0, QApplication::UnicodeUTF8));
        menuArquivo->setTitle(QApplication::translate("NHEClientClass", "&Arquivo", 0, QApplication::UnicodeUTF8));
        menuAjuda->setTitle(QApplication::translate("NHEClientClass", "&Projetos", 0, QApplication::UnicodeUTF8));
        menuAjuda_2->setTitle(QApplication::translate("NHEClientClass", "A&juda", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class NHEClientClass: public Ui_NHEClientClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NHECLIENT_H
