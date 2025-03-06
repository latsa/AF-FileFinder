#pragma once
#include "Finder.h"
#include "Indexer1/Index1.h"
#include "Indexer2/Index2.h"
#include "Indexer3/Index3.h"

#ifdef USE_INDEXER

class FINDER_EXPORT Indexer :
   public QObject {
   Q_OBJECT

public:
    Indexer(Finder* parent);
    void cancel();
    bool m_is_about_close = false;

    void start();
    bool query(const QString& root, const QStringList& expr, ListType type, QVector<Hit>& hits);
    int progress();

signals:
   // Signal indexing done
   void done(qint64 item_count);
   void started();

private slots:
   void backend_done(qint64 item_count);

private:
   Finder* m_parent = Q_NULLPTR;
   Indexer1* m_indexer1 = Q_NULLPTR;
   Indexer2* m_indexer2 = Q_NULLPTR;
   Indexer3* m_indexer3 = Q_NULLPTR;
};

#endif
