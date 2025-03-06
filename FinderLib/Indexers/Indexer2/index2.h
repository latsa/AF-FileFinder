#pragma once
#include "Finder.h"
#include "Indexer2Thread.h"
#include <QStringList>
#include <QVector>
#include <QMutex>

class Indexer2 :
   public QObject {
   Q_OBJECT

public:
   Indexer2(Finder* parent);
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
   Indexer2Thread* m_indexerThread = Q_NULLPTR;

   QMutex m_busy_mutex;
   bool m_is_busy = true;

   QStringList m_database;
};
