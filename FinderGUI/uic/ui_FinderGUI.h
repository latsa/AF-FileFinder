/********************************************************************************
** Form generated from reading UI file 'FinderGUI.ui'
**
** Created by: Qt User Interface Compiler version 5.15.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FINDERGUI_H
#define UI_FINDERGUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FinderGUIClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *FinderGUIClass)
    {
        if (FinderGUIClass->objectName().isEmpty())
            FinderGUIClass->setObjectName(QString::fromUtf8("FinderGUIClass"));
        FinderGUIClass->resize(600, 400);
        menuBar = new QMenuBar(FinderGUIClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        FinderGUIClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(FinderGUIClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        FinderGUIClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(FinderGUIClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        FinderGUIClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(FinderGUIClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        FinderGUIClass->setStatusBar(statusBar);

        retranslateUi(FinderGUIClass);

        QMetaObject::connectSlotsByName(FinderGUIClass);
    } // setupUi

    void retranslateUi(QMainWindow *FinderGUIClass)
    {
        FinderGUIClass->setWindowTitle(QCoreApplication::translate("FinderGUIClass", "FinderGUI", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FinderGUIClass: public Ui_FinderGUIClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FINDERGUI_H
