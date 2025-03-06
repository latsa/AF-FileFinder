#include "FinderGUI.h"
#include "FinderUtil.h"
#include <QApplication>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QDateTime>
#include <QStatusBar>
#include <QLineEdit>
#include <QComboBox>
#include <QHeaderView>
#include <QAction>
#include <QTableWidgetItem>

FinderGUI::FinderGUI(QWidget *parent)
    : QMainWindow(parent) {

    // set up ui
    ui.setupUi(this);
    ui.twResults->setColumnWidth(0, 100);
    ui.twResults->setColumnWidth(1, 200);
    ui.twResults->setColumnWidth(2, 100);
    ui.twResults->setColumnWidth(3, 150);
    ui.twResults->setColumnWidth(4, 1000);

    ui.twResults->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    int item_height = 14;
    ui.twResults->verticalHeader()->setMaximumSectionSize(item_height);
    ui.twResults->verticalHeader()->setDefaultSectionSize(item_height);

    QHeaderView* twResultsHeader = ui.twResults->horizontalHeader();
    twResultsHeader->setDefaultAlignment(Qt::AlignLeft);

    ui.pbSearchRoot->setEnabled(false);
    ui.leSearchRoot->setEnabled(false);

    // Connect user actions
    connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui.actionAbout, &QAction::triggered, this, &FinderGUI::actionAbout);
    connect(ui.leSearchRoot, &QLineEdit::textEdited, this, &FinderGUI::actionSearchRootTextEdited);
    connect(ui.leSearchExpression, &QLineEdit::textEdited, this, &FinderGUI::actionSearchExprTextEdited);

    connect(ui.cbVolumes, &QComboBox::currentTextChanged, this, &FinderGUI::currentDriveTextChanged);
    connect(ui.cbVolumes, SIGNAL(currentIndexChanged(int)), this, SLOT(currentDriveIndexChanged(int)));
    connect(ui.pbSearchRoot, SIGNAL(clicked()), this, SLOT(openSearchRootDialog()));
    connect(ui.pbGo, SIGNAL(clicked()), this, SLOT(find()));

    // Find out about available volumes
    m_volumes = GetVolumes();
    ui.cbVolumes->addItem(m_current_drive);
    foreach(const Volume& vol, m_volumes) {
       ui.cbVolumes->addItem(vol.PathName);
    }

    // Create Finder object
    m_finder = new Finder;
    connect(m_finder, &Finder::found, this, &FinderGUI::append);
    connect(m_finder, &Finder::walking, this, &FinderGUI::notify);
    connect(m_finder, &Finder::status, this, &FinderGUI::status);

    // initialize the status bar
    statusBar()->showMessage("Ready", 3000);
}

FinderGUI::~FinderGUI() {
   delete m_finder;
}

void FinderGUI::closeEvent(QCloseEvent* event) {
   QMutexLocker locker(&m_finder_mutex);
   if (m_finder)
      m_finder->cancel();
}

void FinderGUI::actionAbout() {
   QMessageBox::about(this, QString("About %1...").arg(qApp->applicationName()),
      QString("%1 %2  is a file and directory seach utility for Windows.")
         .arg(qApp->applicationName()).arg(qApp->applicationVersion()));
}

void FinderGUI::actionSearchRootTextEdited(const QString& text) {
   m_search_root = text;
}

void FinderGUI::actionSearchExprTextEdited(const QString& text) {
   m_search_expr = text;
}

void FinderGUI::clearTableWidget() {
   m_next_row_to_update = 0;
   ui.twResults->clearContents();
   ui.twResults->setRowCount(0);
}

void FinderGUI::currentDriveIndexChanged(int index) {
   ui.pbSearchRoot->setEnabled(index);
   ui.leSearchRoot->setEnabled(index);
   ui.leSearchRoot->clear();
   clearTableWidget();
}

void FinderGUI::currentDriveTextChanged(QString text) {
   m_current_drive = text;
   statusBar()->showMessage(QString("Current drive has been set to %1").arg(m_current_drive), 3000);
}

void FinderGUI::openSearchRootDialog() {

   if (m_dialog_directory.isEmpty())
      m_dialog_directory = m_current_drive + "\\";

   m_dialog_directory = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, qApp->applicationName(), m_dialog_directory));

   QString drive = m_dialog_directory.mid(0, 2);
   QString text = m_dialog_directory.mid(2);

   if (!m_dialog_directory.isEmpty()) {
      ui.leSearchRoot->setText(text);
      m_search_root = text;

      for (int i = 1; i < ui.cbVolumes->count(); i++)
         if (!drive.compare(ui.cbVolumes->itemText(i), Qt::CaseInsensitive)) {
            ui.cbVolumes->setCurrentIndex(i);
            m_current_drive = ui.cbVolumes->currentText();
         }
   }
}

void FinderGUI::find() {

   {
      QMutexLocker locker(&m_finder_mutex);

      if (m_finding) {
         m_finder->cancel();
         m_finding = false;
         return;
      }

      m_finding = true;
      ui.pbGo->setText("X");
      ui.pbGo->setToolTip("Press to cancel...");
   }
 
   statusBar()->showMessage("Walking...");
   qApp->processEvents();

   // retrieve search results
   clearTableWidget();

   // get drives to search
   QStringList drives_to_search;
   if (ui.cbVolumes->currentIndex()) {
      drives_to_search << ui.cbVolumes->currentText();
   } else {
      int cnt = ui.cbVolumes->count();
      for (int idx = 1; idx < cnt; idx++) {
         drives_to_search << ui.cbVolumes->itemText(idx);
      }
   }

   // benchmark
   qint64 start_time = QDateTime::currentMSecsSinceEpoch();

   // Parse the semicolon separated list of search expressions
   QStringList search_expr_list;
   
   if (m_search_expr.size())
      //search_expr_list << m_search_expr;
      search_expr_list = m_search_expr.split(';', Qt::SplitBehaviorFlags::SkipEmptyParts);
   else
      search_expr_list << "*";


   // Do the actual search
   int total_hits_count = 0;

   foreach(const QString & drive, drives_to_search) {

      int hits_count = m_finder->find(drive + m_search_root, search_expr_list, ListType::both);
      total_hits_count += hits_count;

      if (m_finder->isAboutToClose()) {
         break;
      }
   }

   {
      QMutexLocker locker(&m_finder_mutex);
      m_finding = false;
      ui.pbGo->setText("Go");
      ui.pbGo->setToolTip("Press to find...");
   }

   // benchmark
   qint64 end_time = QDateTime::currentMSecsSinceEpoch();
   qint64 delta = end_time - start_time;

   statusBar()->showMessage(QString("%1 entries found. Ready.").arg(total_hits_count));
   qApp->processEvents();
}

void FinderGUI::append(const Hit& hit) {

   ui.twResults->setRowCount(m_next_row_to_update + 1);
   ui.twResults->setItem(m_next_row_to_update, 0, new QTableWidgetItem(toString(hit.m_type)));
   ui.twResults->setItem(m_next_row_to_update, 1, new QTableWidgetItem(hit.m_name));
   ui.twResults->setItem(m_next_row_to_update, 2, new QTableWidgetItem(QString("%1").arg(hit.m_file_info.size())));
   ui.twResults->setItem(m_next_row_to_update, 3, new QTableWidgetItem(hit.m_file_info.fileTime(QFile::FileTime::FileModificationTime).toString("yyyy-MM-dd hh:mm:ss")));
   ui.twResults->setItem(m_next_row_to_update, 4, new QTableWidgetItem(hit.m_path));
   m_next_row_to_update++;

   statusBar()->showMessage(QString("%1 entries found. Walking %2... ").arg(m_next_row_to_update).arg(hit.m_path));
   qApp->processEvents();
}

void FinderGUI::notify(const Hit& hit) {
   statusBar()->showMessage(QString("%1 entries found. Walking %2... ").arg(m_next_row_to_update).arg(hit.m_path));
   qApp->processEvents();
}

void FinderGUI::status(QString msg) {
   statusBar()->showMessage(msg);
}