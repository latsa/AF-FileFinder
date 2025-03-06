///////////////////////////////////////////////////////////////////////////////
// Finder.h
//
#pragma once
#include "FinderConfig.h"
#include "FinderTypes.h"
#include "ListVolumes.h"
#include <QStringList>

#ifdef USE_INDEXER
class Indexer;
#endif

class FINDER_EXPORT Finder :
   public QObject {
   Q_OBJECT

public:
    Finder();

    // returns the number of entries found
    int find(const QString& search_root, const QStringList& search_expressions, ListType type);

    void cancel();
    inline bool isAboutToClose() {return m_is_about_close;}
    const QVector<Volume>& volumes() const { return m_volumes; }

signals:
   // Signal a single match while walking the directory tree or while scanning the database
   void walking(const Hit& hit);
   void found(const Hit& hit);
   void status(QString msg);

public slots:
   void backend_walking(const Hit& hit);
   void backend_found(const Hit& hit);
   void backend_status(QString msg);

   #ifdef USE_INDEXER
   void indexing_started();
   void indexing_done(qint64 item_count);

private:
   Indexer* m_indexer = Q_NULLPTR;
   #endif

private:
   bool m_is_about_close = false;

   QString m_search_root;
   QStringList m_search_expressions;
   ListType m_type;
   QVector<Volume> m_volumes;

public:
   QVector<Hit> m_hits;
};

// That's it
///////////////////////////////////////////////////////////////////////////////

