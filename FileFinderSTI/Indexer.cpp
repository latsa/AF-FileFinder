#include "Indexer.h"
#include "IndexerThread.h"
#include <QMutex>

Indexer::Indexer() {
   
}

Indexer::~Indexer() {
}


// Start indexing...
void Indexer::start() {
   m_tbegin = clock();

   m_busy = true;
   m_volumes = GetVolumes();
   m_volume_count = m_volumes.size();
   m_done_counter = 0;

   emit started();

   for (Volume& vol : m_volumes) {
      IndexerThread* indexerThread = Q_NULLPTR;
      QStringList database;
      QString volumeName = QString(vol.PathName);
      indexerThread = new IndexerThread(database, volumeName, this);
      connect(indexerThread, &IndexerThread::done, this, &Indexer::backend_done);
      connect(indexerThread, &IndexerThread::progress, this, &Indexer::backend_progress);
      connect(indexerThread, &IndexerThread::finished, indexerThread, &QObject::deleteLater);
      connect(indexerThread, &IndexerThread::statusMessage, this, &Indexer::backend_statusMessage);
      m_indexerThreads.append(indexerThread);
      indexerThread->start();
   }
}

// Cancel indexing...
void Indexer::cancel() {
   for (IndexerThread* indexerThread : m_indexerThreads) {
      indexerThread->stop();
   }

   m_indexerThreads.clear();
   m_busy = false;
}


bool Indexer::query(const QString& root, const QStringList& expr, ListType type, QVector<Hit>& hits) {
 
   if (m_busy)
      return false;

   //...

   return true;
}

void Indexer::backend_progress(QString volumeName, int percent_done) {
   //..
   //emit progress(percent_done);
}

void Indexer::backend_done(QString volumeName, qint64 item_count) {
   m_done_counter++;
   if (m_done_counter >= m_volume_count) {
      m_busy = false;
      emit done();
      printDuration(m_tbegin, clock());
   }
}

void Indexer::backend_statusMessage(QString msg) {
   emit statusMessage(msg);
}

void Indexer::printDuration(time_t tbegin, time_t tend) {
   unsigned int milliseconds = static_cast<unsigned int>((tend - tbegin) * 1000 / CLOCKS_PER_SEC);
   unsigned int divisor = 1000 * 60 * 60 * 24;

   unsigned int days = milliseconds / divisor;
   milliseconds -= days * divisor;

   divisor /= 24;
   unsigned int hours = milliseconds / divisor;
   milliseconds -= hours * divisor;

   divisor /= 60;
   unsigned int minutes = milliseconds / divisor;
   milliseconds -= minutes * divisor;

   divisor /= 60;
   unsigned int seconds = milliseconds / divisor;
   milliseconds -= seconds * divisor;

  QString msg = QString("Finished in ");
  if (days > 0) {
      msg += QString("%1d").arg(days);
      msg += QString("%1h").arg(hours);
      msg += QString("%1m").arg(minutes);
      msg += QString("%1s").arg(seconds);
  }
  else if (hours > 0) {
     msg += QString("%1h").arg(hours);
     msg += QString("%1m").arg(minutes);
     msg += QString("%1s").arg(seconds);
  }
  else if (minutes > 0) {
     msg += QString("%1m").arg(minutes);
     msg += QString("%1s").arg(seconds);
  }
  else if (seconds > 0) {
     msg += QString("%1s").arg(seconds);
  }

  msg += QString("%1ms").arg(milliseconds);

  emit statusMessage(msg);
}