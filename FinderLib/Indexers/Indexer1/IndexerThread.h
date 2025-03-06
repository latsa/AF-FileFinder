#pragma once
#include "Finder.h"
#include <QThread>

class IndexerThread  :
   public QThread {
    Q_OBJECT

public:
    IndexerThread(QVector<Hit>& all_hits, Finder* parent);
    ~IndexerThread();

    void run() override;

private:
   Finder* m_parent = Q_NULLPTR;
   QVector<Hit>& m_all_hits;

signals:
   void done(qint64 item_count);
};


