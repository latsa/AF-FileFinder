#pragma once

#include <QApplication>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>

#include "ui_FileFinderSTI.h"

#include "Indexer.h"
class FileFinderSTI :
public QMainWindow {
   Q_OBJECT

public:
   FileFinderSTI(QWidget *parent = nullptr);
   ~FileFinderSTI();

public slots:
   void statusMessage(QString msg);
   void quitApp();

protected:
   void closeEvent(QCloseEvent* event) override;

private:
   void createActions();
   void createTrayIcon();
   void showSearchWindow();
   void hideSearchWindow();
   void stopIndexing();
   void startIndexing();

   Ui::FileFinderSTIClass ui;

   QAction* stopIndexingAction;
   QAction* startIndexingAction;
   QAction* showSearchWindowAction;
   QAction* hideSearchWindowAction;

   QAction* minimizeAction;
   QAction* maximizeAction;
   QAction* restoreAction;
   QAction* quitAction;

   QSystemTrayIcon* trayIcon;
   QMenu* trayIconMenu;

   Indexer m_indexer;
};
