#pragma once
#include "Indexer.h"
#include "ntfs.h"
#include <QVector>
#include <QThread>

struct File {
   bool isDirectory = false;
   uint64_t    parent = 0;
   char* name = 0;
};

class IndexerThread  :
   public QThread {
    Q_OBJECT

public:
    IndexerThread(QStringList& db, QString& volumeName, Indexer* parent);
    ~IndexerThread();

    void run() override;
    void stop();

    QVector<QString> distinct_directories;
    QVector<QString> distinct_files;

private:
   Indexer* m_parent = Q_NULLPTR;
   QStringList& m_db;
   QString m_volumeName;
   bool m_stop = false;

   QVector<QString> m_distinct_directories;
   QVector<QString> m_distinct_files;

   //bool walk(const QString& root);
private:
   HANDLE drive;
   DWORD bytesAccessed;
   BootSector bootSector;
   QVector<File> files;

   #define MFT_FILE_SIZE (1024)
   uint8_t mftFile[MFT_FILE_SIZE];

   #define MFT_FILES_PER_BUFFER (65536)
   uint8_t mftBuffer[MFT_FILES_PER_BUFFER * MFT_FILE_SIZE];

   DWORD Read(void* buffer, uint64_t from, uint64_t count);
   QString buildPath(const QString& driveLetter, const File& file);

signals:
   void progress(QString volumeName, int percent_done);
   void done(QString volumeName, qint64 item_count);
   void statusMessage(QString msg);
};


