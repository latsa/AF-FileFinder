#pragma once
#include "Finder.h"
#include <QStringList>
#include <QVector>

class Indexer3 :
   public QObject {
   Q_OBJECT

public:
   Indexer3(Finder* parent);
   void cancel();
   bool m_is_about_close = false;

   void start();
   bool query(const QString& root, const QStringList& expr, ListType type, QVector<Hit>& hits);
   int progress();

signals:
   // Signal indexing done
   void done(qint64 item_count);

private:
   Finder* m_parent = Q_NULLPTR;
};
