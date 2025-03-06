///////////////////////////////////////////////////////////////////////////////
// index1.h
//
// This is not a proper indexer. It creates an in-memory list of all files and
// directories only. The query() calls will search this list rather than walking
// the file system. This is the reference implementation for benchmarking real
// indexers.

#pragma once
#include "Finder.h"
#include <QStringList>
#include <QVector>
#include <QMutex>

class Indexer1 :
   public QObject {
   Q_OBJECT

public:
   Indexer1(Finder* parent);
   void cancel();
   bool m_is_about_close = false;

   void start();
   bool query(const QString& root, const QStringList& expr, ListType type, QVector<Hit>& hits);
   int progress();

signals:
   // Signal indexing done
   void done(qint64 item_count);

public slots:
   void backend_done(qint64 item_count);

private:
   Finder* m_parent = Q_NULLPTR;

   QMutex m_busy_mutex;
   bool m_is_busy = true;
   QVector<Hit> m_all_hits;
};

