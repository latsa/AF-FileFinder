#include "Indexer.h"

#ifdef USE_INDEXER

Indexer::Indexer(Finder* parent) : 
   m_parent(parent) {

   if (use_indexer == 0) {
      m_indexer1 = new Indexer1(m_parent);
      connect(m_indexer1,SIGNAL(done(qint64)),this,SLOT(backend_done(qint64)));
   }
      
   if (use_indexer == 1) {
      m_indexer2 = new Indexer2(m_parent);
      connect(m_indexer2, SIGNAL(done(qint64)), this, SLOT(backend_done(qint64)));
   }
      
   if (use_indexer == 2) {
      m_indexer3 = new Indexer3(m_parent);
      connect(m_indexer3, SIGNAL(done(qint64)), this, SLOT(backend_done(qint64)));
   }
      
}

// Cancel indexing...
void Indexer::cancel() {
   m_is_about_close = true;

   if (m_indexer1)
      m_indexer1->cancel();
   if (m_indexer2)
      m_indexer2->cancel();
   if (m_indexer3)
      m_indexer3->cancel();
}


// Start indexing...
void Indexer::start() {
   if (m_indexer1)
      m_indexer1->start();
   if (m_indexer2)
      m_indexer2->start();
   if (m_indexer3)
      m_indexer3->start();

   emit started();
}

bool Indexer::query(const QString& root, const QStringList& expr, ListType type, QVector<Hit>& hits) {
   if (m_indexer1) 
      return m_indexer1->query(root,expr,type,hits);
   if (m_indexer2)
      return m_indexer2->query(root, expr, type, hits);
   if (m_indexer3)
      return m_indexer3->query(root, expr, type, hits);

   return false;
}

// 100 means no indexing in progress, any other value means indexing is in progress
int Indexer::progress() {
   if (m_indexer1)
      return  m_indexer1->progress();
   if (m_indexer2)
      return m_indexer2->progress();
   if (m_indexer3)
      return m_indexer3->progress();
   return 0;
}

void Indexer::backend_done(qint64 item_count) {
   emit done(item_count);
}

#endif
