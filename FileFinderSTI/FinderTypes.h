#pragma once
#include "FinderGlobal.h"
#include <QFileInfo>
#include <QDebug>
#include <iostream>

enum class FINDER_EXPORT FindType {
   other, file, directory, link
};
Q_DECLARE_METATYPE(FindType)

enum class FINDER_EXPORT ListType {
   none, file, directory, both
};
Q_DECLARE_METATYPE(ListType)

class FINDER_EXPORT Hit {
public:

   QString dump() const {
      QString out;

      QString sType;
      switch (m_type) {
      case FindType::directory:
         sType = "directory";
         break;
      case FindType::file:
         sType = "file";
         break;
      }

   out = QString("Found %1 \"%2\" in directory \"%3\"").arg(sType).arg(m_name).arg(m_path);
   std::cout << out.toLocal8Bit().data() << std::endl;
   return out;
   }

   QFileInfo m_file_info;
   FindType m_type = FindType::file;
   QString m_path;
   QString m_name;
};
Q_DECLARE_METATYPE(Hit)
