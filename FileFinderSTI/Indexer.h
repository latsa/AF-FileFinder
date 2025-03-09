#pragma once
#include <QObject>
#include <QString>

#include "FinderGlobal.h"
#include "FinderTypes.h"
#include "Utils/ListVolumes.h"

class IndexerThread;

class FINDER_EXPORT Indexer :
   public QObject {
   Q_OBJECT

public:
    Indexer();
    ~Indexer();

    void start();
    bool query(const QString& root, const QStringList& expr, ListType type, QVector<Hit>& hits);
    void cancel();

    bool m_busy = false;

signals:
   void started();
   void done();
   void progress(int percent_done);
   void statusMessage(QString msg);

public slots:
   void backend_progress(QString volumeName, int percent_done);
   void backend_done(QString volumeName, qint64 item_count);
   void backend_statusMessage(QString msg);

private:
   void printDuration(time_t tbegin, time_t tend);
  time_t m_tbegin;

  int m_progress = 0;
  int m_volume_count = 0;
  int m_done_counter = 0;
  QVector<Volume> m_volumes;
  QVector<IndexerThread*> m_indexerThreads;
};

