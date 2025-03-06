///////////////////////////////////////////////////////////////////////////////
// Finder.cpp
//
#include "Finder.h"
#include "Walker.h"


#ifdef USE_INDEXER
#include "Indexer.h"
#endif

Finder::Finder() {
   // Enumerate DOS volumes
   m_volumes = GetVolumes();

   #ifdef USE_INDEXER
   // Initialize indexer
   m_indexer = new Indexer(this);
   if (m_indexer) {
      connect(m_indexer, SIGNAL(started()), this,SLOT(indexing_started()));
      connect(m_indexer, SIGNAL(done(qint64)), this, SLOT(indexing_done(qint64)));
      connect(m_indexer, SIGNAL(found(const Hit&)), this, SLOT(backend_found(const Hit&)));
      m_indexer->start();
   }
   #endif      
}

void Finder::cancel() {

   #ifdef USE_INDEXER
   if (m_indexer)
      m_indexer->cancel();
   #endif

   m_is_about_close = true;
}

///////////////////////////////////////////////////////////////////////////////
// If USE_INDEXER is defined, find hits by querying the database, or if 
// unsuccessful, by walking the directory tree directly.
//
// If USE_INDEXER is not defined, then just walk the directory tree, without
// using any index or querying any database.

int Finder::find(const QString& search_root, const QStringList& search_expressions, ListType type) {

   m_is_about_close = false;
   m_search_root = search_root;
   m_search_expressions = search_expressions;
   m_type = type;

   #ifdef USE_INDEXER
   if (!m_indexer->query(m_search_root, m_search_expressions, m_type, m_hits))
   #endif
      walk(m_search_root, m_search_expressions, m_type, m_hits, this);

   return m_hits.size();
}

void Finder::backend_walking(const Hit& hit) {
   emit walking(hit);
}

void Finder::backend_found(const Hit& hit) {
   emit found(hit);
}

void Finder::backend_status(QString msg) {
   emit status(msg);
}

#ifdef USE_INDEXER
void Finder::indexing_started() {
   qDebug() << "Indexing started...";
}

void Finder::indexing_done(qint64 item_count) {
   qDebug() << "Indexing done.";
}
#endif


// That's it
///////////////////////////////////////////////////////////////////////////////
