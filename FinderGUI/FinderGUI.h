#pragma once

#include <QtWidgets/QMainWindow>
#include <QMutexLocker>
#include <QString>

#include "ui_FinderGUI.h"
#include "Finder.h"
#include "ListVolumes.h"

class FinderGUI : 
   public QMainWindow {
   Q_OBJECT

public:
   FinderGUI(QWidget *parent = nullptr);
   ~FinderGUI();
   void closeEvent(QCloseEvent* event) override;

private slots:
   void currentDriveTextChanged(QString text);
   void currentDriveIndexChanged(int index);
   void openSearchRootDialog();
   void find();
   void notify(const Hit& hit);
   void append(const Hit& hit);
   void status(QString msg);

private:
   void actionAbout();
   void actionSearchRootTextEdited(const QString& text);
   void actionSearchExprTextEdited(const QString& text);
   void clearTableWidget();

   Ui::FinderGUIClass ui;
   
   QVector<Volume> m_volumes;
   QString m_dialog_directory = Q_NULLPTR;
   QString m_current_drive = "<All drives>";
   QString m_search_root;
   QString m_search_expr;

   Finder* m_finder = Q_NULLPTR;
   QMutex m_finder_mutex;
   bool m_finding = false;
   int m_next_row_to_update = 0;
};
