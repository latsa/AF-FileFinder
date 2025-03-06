#include "Walker.h"
#include "FinderConfig.h"

#include "Walker1/walk1.h"
#include "Walker2/walk2.h"
#include "Walker3/walk3.h"

bool (*walker_alternatives[3])(const QString&, const QStringList&, ListType, QVector<Hit>&, Finder* parent) = {walk1, walk2, walk3};

bool walk(const QString& root, const QStringList& expr, ListType type, QVector<Hit>& hits, Finder* parent) {
   
   if (parent && parent->isAboutToClose())
      return false;

   return walker_alternatives[use_walker](root, expr, type, hits, parent);
}

