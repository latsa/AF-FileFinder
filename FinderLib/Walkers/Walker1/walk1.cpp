#include "walk1.h"
#include "match.h"
#include "FinderConfig.h"
#include <QFileInfoList>
#include <QDir>
#include <QDateTime>

bool walk1(const QString& root, const QStringList& expr, ListType type, QVector<Hit>& hits, Finder* parent) {

   QString root_with_trailing_directory_separator = root;
   if (root.mid(root.size()-1,1) != "\\" && root.mid(root.size() - 1, 1) != "/")
      root_with_trailing_directory_separator += "/";

   QDir dir(root_with_trailing_directory_separator);
   dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);

   const QFileInfoList& entryInfoList = dir.entryInfoList(QDir::Filter::NoFilter, QDir::SortFlag::NoSort);
   foreach(QFileInfo fileInfo, entryInfoList) {

      if (parent && parent->isAboutToClose())
         return false;

      if (!fileInfo.isReadable())
         continue;

      if (fileInfo.isDir() || fileInfo.isFile()) {
         Hit hit;
         hit.m_file_info = fileInfo;
         hit.m_name = fileInfo.fileName();
         hit.m_path = root;

         if (fileInfo.isDir()) {
            if (type == ListType::directory || type == ListType::both) {
               if (match(fileInfo, expr)) {
                  hit.m_type = FindType::directory;
                  hits.append(hit);

                  #ifdef DUMP_WHEN_ONE_FOUND
                  hit.dump();
                  #endif

                  parent->backend_found(hit);
               } else {
                  parent->backend_walking(hit);
               }
            }

            if (!walk1(fileInfo.filePath(), expr, type, hits, parent))
               break;
         }

         if (fileInfo.isFile()) {
            if (type == ListType::file || type == ListType::both) {
               if (match(fileInfo, expr)) {
                  hit.m_type = FindType::file;
                  hits.append(hit);

                  #ifdef DUMP_WHEN_ONE_FOUND
                  hit.dump();
                  #endif

                  parent->backend_found(hit);
               } else {
                  parent->backend_walking(hit);
               }
            }
         }
      }
   }

   return true;
}
