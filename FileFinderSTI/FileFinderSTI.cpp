#include "FileFinderSTI.h"

#include <QIcon>
#include <QMessageBox>
#include <QCloseEvent>

FileFinderSTI::FileFinderSTI(QWidget *parent)
    : QMainWindow(parent) {
   ui.setupUi(this);

   createActions();
   createTrayIcon();

   trayIcon->setIcon(QIcon(":/images/FileFinderSTI.png"));
   trayIcon->setVisible(true);
   trayIcon->show();

   showSearchWindow();

   //connect(&m_indexer, SIGNAL(statusMessage(QString)), this, SLOT(statusMessage(QString)));
   connect(&m_indexer, &Indexer::statusMessage, this, &FileFinderSTI::statusMessage);
   m_indexer.start();
   // 
   //ui.statusBar->showMessage(tr("Ready"));
   statusBar()->showMessage(tr("Ready"));
}

FileFinderSTI::~FileFinderSTI() {
}

void FileFinderSTI::statusMessage(QString msg) {
   statusBar()->showMessage(msg);
}

void FileFinderSTI::showSearchWindow() {
   show();
   showSearchWindowAction->setVisible(false);
   hideSearchWindowAction->setVisible(true);

}

void FileFinderSTI::hideSearchWindow() {
   hide();
   hideSearchWindowAction->setVisible(false);
   showSearchWindowAction->setVisible(true);
}

void FileFinderSTI::stopIndexing() {
   stopIndexingAction->setVisible(false);
   startIndexingAction->setVisible(true);
   m_indexer.cancel();
}

void FileFinderSTI::startIndexing() {
   startIndexingAction->setVisible(false);
   stopIndexingAction->setVisible(true);
   m_indexer.start();
}

void FileFinderSTI::createActions() {
   stopIndexingAction = new QAction(tr("&Stop Indexing"), this);
   connect(stopIndexingAction, &QAction::triggered, this, &FileFinderSTI::stopIndexing);
   //stopIndexingAction->setEnabled(false);

   startIndexingAction = new QAction(tr("Re&Index"), this);
   connect(startIndexingAction, &QAction::triggered, this, &FileFinderSTI::startIndexing);
   startIndexingAction->setVisible(false);

   showSearchWindowAction = new QAction(tr("&Show Search Window"), this);
   connect(showSearchWindowAction, &QAction::triggered, this, &FileFinderSTI::showSearchWindow);

   hideSearchWindowAction = new QAction(tr("&Hide Search Window"), this);
   connect(hideSearchWindowAction, &QAction::triggered, this, &FileFinderSTI::hideSearchWindow);
   hideSearchWindowAction->setVisible(false);

   minimizeAction = new QAction(tr("Mi&nimize"), this);
   connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

   maximizeAction = new QAction(tr("Ma&ximize"), this);
   connect(maximizeAction, &QAction::triggered, this, &QWidget::showMaximized);

   restoreAction = new QAction(tr("&Restore"), this);
   connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

   quitAction = new QAction(tr("&Quit"), this);
   connect(quitAction, &QAction::triggered, this, &FileFinderSTI::quitApp);
}

void FileFinderSTI::createTrayIcon() {
   trayIconMenu = new QMenu(this);
   trayIconMenu->addAction(stopIndexingAction);
   trayIconMenu->addAction(startIndexingAction);
   trayIconMenu->addSeparator();
   trayIconMenu->addAction(showSearchWindowAction);
   trayIconMenu->addAction(hideSearchWindowAction);
   trayIconMenu->addSeparator();
   trayIconMenu->addAction(minimizeAction);
   trayIconMenu->addAction(maximizeAction);
   trayIconMenu->addAction(restoreAction);
   trayIconMenu->addSeparator();
   trayIconMenu->addAction(quitAction);

   trayIcon = new QSystemTrayIcon(this);
   trayIcon->setContextMenu(trayIconMenu);
}

void FileFinderSTI::closeEvent(QCloseEvent* event) {

#ifdef Q_OS_MACOS
   if (!event->spontaneous() || !isVisible()) {
      return;
   }
#endif

   if (trayIcon->isVisible()) {
      QMessageBox::information(this, tr("File Finder"),
         tr("The program will keep running in the system tray. To terminate the program, "
            "choose <b>Quit</b> in the context menu of the system tray entry."));
      hideSearchWindow();
      event->ignore();
   }
}

void FileFinderSTI::quitApp() {
   m_indexer.cancel();
   qApp->quit();
}
