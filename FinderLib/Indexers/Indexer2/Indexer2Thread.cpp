#include "Indexer2Thread.h"
#include <QDir>

Indexer2Thread::Indexer2Thread(QStringList& db, Finder* parent)
    : QThread(parent), m_parent(parent), m_db(db) {
}

Indexer2Thread::~Indexer2Thread() {
   wait();
}

void Indexer2Thread::run() {
   qint64 item_count = 0;

   foreach (Volume vol,m_parent->volumes()) {
      walk(vol.PathName);
   }

   m_parent->backend_status("Indexing done");
   emit done(item_count);
}

void Indexer2Thread::stop() {
   m_stop = true;
}

bool Indexer2Thread::walk(const QString& root) {
   bool rc = true;

   QString root_with_trailing_directory_separator = root;
   if (root.mid(root.size() - 1, 1) != "\\" && root.mid(root.size() - 1, 1) != "/")
      root_with_trailing_directory_separator += "/";

   QDir dir(root_with_trailing_directory_separator);
   dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);

   const QFileInfoList& entryInfoList = dir.entryInfoList(QDir::Filter::NoFilter, QDir::SortFlag::NoSort);
   foreach(QFileInfo fileInfo, entryInfoList) {
      if (m_stop)
         return false;

      if (!fileInfo.isReadable())
         continue;

      if (fileInfo.isDir() || fileInfo.isFile()) {

         QString canonicalFilePath = fileInfo.canonicalFilePath();

         //////////////////////////////////////////////////////////////////////
         m_db.append(fileInfo.canonicalFilePath());

         QString msg = QString("Indexing %1").arg(fileInfo.canonicalFilePath());
         m_parent->backend_status(msg);
         //////////////////////////////////////////////////////////////////////

         if (fileInfo.isDir()) {
            if (!walk(fileInfo.filePath())) {
               rc = false;
               break;
            }
               
         }

      }
   }

   return rc;
}
