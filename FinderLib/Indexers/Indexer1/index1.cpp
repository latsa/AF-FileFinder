#if 0
#include "index1.h"
#include "walk.h"
#include "ListVolumes.h"
#include <QStringList>

void index1(QVector<Hit>& hits, Finder* parent) {
   // a basic directory tree indexing method

   // Find out about available volumes
   int volume_count;
   Volume* volume_names = Q_NULLPTR;

   unsigned long error = GetVolumeCount(&volume_count);
   if (!error) {
      volume_names = new Volume[volume_count];
      error = GetVolumeNames(volume_names);
   }

   for (int i=0;i<volume_count;i++) {
      const Volume& vol = volume_names[i];
      QString search_root = vol.PathName;
      QStringList search_expr_list;
      search_expr_list << "*";

      walk(search_root, search_expr_list, ListType::both, hits, parent);
   }
}
#endif

///////////////////////////////////////////////////////////////////////////////

#include "index1.h"
#include "IndexerThread.h"

Indexer1::Indexer1(Finder* parent) :
   m_parent(parent) {

}

// Cancel indexing...
void Indexer1::cancel() {
   m_is_about_close = true;
}

// Start indexing...
void Indexer1::start() {

   IndexerThread* indexerThread = new IndexerThread(m_all_hits, m_parent);
   connect(indexerThread, &IndexerThread::done, this, &Indexer1::backend_done);
   connect(indexerThread, &IndexerThread::finished, indexerThread, &QObject::deleteLater);

   {
      QMutexLocker locker(&m_busy_mutex);
      m_is_busy = true;
   }

   indexerThread->start();
}

void Indexer1::backend_done(qint64 item_count) {
   QMutexLocker locker(&m_busy_mutex);
   m_is_busy = false;

   emit done(item_count);
}

#include "Walker.h"
bool Indexer1::query(const QString& root, const QStringList& expr, ListType type, QVector<Hit>& hits) {
   bool is_busy = true;

   {
      QMutexLocker locker(&m_busy_mutex);
      is_busy = m_is_busy;
   }

   if (is_busy)
      return false;

   ////////////////////////////////////////////////////////////////////////////
   return walk(root,expr,type,hits,m_parent);
   ////////////////////////////////////////////////////////////////////////////
}

// 100 means no indexing in progress, any other value means indexing is in progress
int Indexer1::progress() {
   return 0;
}
