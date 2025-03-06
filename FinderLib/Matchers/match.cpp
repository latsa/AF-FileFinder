#include "match.h"
#include "match_config.h"

#include "Matcher1/match1.h"
#include "Matcher2/match2.h"
#include "Matcher3/match3.h"
#include "Matcher4/match4.h"
#include "Matcher5/match5.h"

static bool (*amatch_alternatives[5])(const QString& glob, const QString& text) = { match1, match2, match3, match4, match5 };

static bool amatch(const QFileInfo& fileInfo, const QStringList& expr) {
   bool m = false;

   foreach(const QString & e, expr) {
      m |= amatch_alternatives[use_matcher](e, fileInfo.fileName());
   }

   return m;
}

static bool amatch(const QString& fileName, const QStringList& expr) {
   bool m = false;

   foreach(const QString & e, expr) {
      m |= amatch_alternatives[use_matcher](e, fileName);
   }

   return m;
}


static bool qmatch(const QFileInfo& fileInfo, const QStringList& expr) {
   return QDir::match(expr, fileInfo.fileName());
}

static bool (*match_alternatives[2])(const QFileInfo&, const QStringList&) = { qmatch, amatch };

bool match(const QFileInfo& fileInfo, const QStringList& expr) {
   return match_alternatives[use_qdirmatch?0:1](fileInfo, expr);
}

bool match(const QString& fileName, const QStringList& expr) {
   return amatch(fileName, expr);
}

