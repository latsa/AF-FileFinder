#pragma once
#include "Finder.h"
#include <QThread>

class Indexer2Thread  :
   public QThread {
    Q_OBJECT

public:
    Indexer2Thread(QStringList& db, Finder* parent);
    ~Indexer2Thread();

    void run() override;
    void stop();

private: 
   Finder* m_parent = Q_NULLPTR;
   QStringList& m_db;
   bool m_stop = false;

   bool walk(const QString& root);

signals:
   void done(qint64 item_count);
};


