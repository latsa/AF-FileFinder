#include "index2.h"
#include "match.h"
#include <QFileInfo>

Indexer2::Indexer2(Finder* parent) :
   m_parent(parent) {

}

// Cancel indexing...
void Indexer2::cancel() {
   m_is_about_close = true;
   m_indexerThread->stop();
}

// Start indexing...
void Indexer2::start() {
   m_indexerThread = new Indexer2Thread(m_database, m_parent);
   connect(m_indexerThread, &Indexer2Thread::done, this, &Indexer2::backend_done);
   connect(m_indexerThread, &Indexer2Thread::finished, m_indexerThread, &QObject::deleteLater);

   {
      QMutexLocker locker(&m_busy_mutex);
      m_is_busy = true;
   }

   m_indexerThread->start();
}

void Indexer2::backend_done(qint64 item_count) {
   QMutexLocker locker(&m_busy_mutex);
   m_is_busy = false;

   emit done(item_count);
}

bool Indexer2::query(const QString& root, const QStringList& expr, ListType type, QVector<Hit>& hits) {
   bool is_busy = true;

   {
      QMutexLocker locker(&m_busy_mutex);
      is_busy = m_is_busy;
   }

   // TEST TEST TEST
   if (m_database.size())
      is_busy = false;
   // TEST TEST TEST

   if (is_busy)
      return false;

   // Query the database here..
   foreach(const QString& entry, m_database) {
      if (m_parent && m_parent->isAboutToClose())
         return false;

      if (entry.contains(root,Qt::CaseInsensitive)) {
         QFileInfo fileInfo(entry);

         if (!fileInfo.isReadable())
            continue;

         if (fileInfo.isDir() || fileInfo.isFile()) {
            Hit hit;
            hit.m_file_info = fileInfo;
            hit.m_name = fileInfo.fileName();
            hit.m_path = root;

            if (fileInfo.isDir()) {

               if (type == ListType::directory || type == ListType::both) {
                  if (match(entry, expr)) {
                     hit.m_type = FindType::directory;
                     hits.append(hit);
                     m_parent->backend_found(hit);
                  } else {
                     m_parent->backend_walking(hit);
                  }
               }

            }

            if (fileInfo.isFile()) {
               if (type == ListType::file || type == ListType::both) {
                  if (match(entry, expr)) {
                     hit.m_type = FindType::file;
                     hits.append(hit);
                     m_parent->backend_found(hit);
                  } else {
                     m_parent->backend_walking(hit);
                  }
               }
            }

         }
      }
   }

   return false;
}

// 100 means no indexing in progress, any other value means indexing is in progress
int Indexer2::progress() {
   return 0;
}
