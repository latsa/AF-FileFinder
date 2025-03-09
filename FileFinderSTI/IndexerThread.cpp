#include "IndexerThread.h"
#include <QDir>

IndexerThread::IndexerThread(QStringList& db, QString &volumeName, Indexer* parent)
    : QThread(parent), m_volumeName(volumeName), m_parent(parent), m_db(db) {
}

IndexerThread::~IndexerThread() {
   wait();
}


DWORD IndexerThread::Read(void* buffer, uint64_t from, uint64_t count) {
   LONG high = from >> 32;
   SetFilePointer(drive, from & 0xFFFFFFFF, &high, FILE_BEGIN);
   ReadFile(drive, buffer, (DWORD)count, &bytesAccessed, NULL);
   return bytesAccessed;
}

QString IndexerThread::buildPath(const QString& driveLetter, const File& file) {

   QString path;
   QString fname;

   for (File f = file; fname = f.name, fname != "."; f = files[f.parent]) {
      

      if (!fname.size() || fname[0] == '$')
         return QString();

      path = fname + QString("/") + path;

      if (m_stop || path.size() > 4096)
         return QString();
   }

   int pathsize = path.size();
   int pathsizeminusone = pathsize - 1;
   if (pathsize && path[pathsizeminusone] == '/')
      path = path.mid(0, pathsizeminusone);

   return QString("%1:/%2").arg(driveLetter).arg(path);
}

#include <atlconv.h>
#include <atlstr.h>

void IndexerThread::run() {
   int percent_done = 0;

   char driveName[7];
   QString driveLetter = m_volumeName.left(1);
   sprintf_s(driveName, 7, "\\\\.\\%c:", driveLetter.toLocal8Bit().data()[0]);
   
   drive = CreateFileA(driveName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

   DWORD bytesRead = Read(&bootSector, 0, 512);
   if (bytesRead != 512) {
      return;
   }

   uint64_t bytesPerCluster = bootSector.bytesPerSector * bootSector.sectorsPerCluster;

   Read(&mftFile, bootSector.mftStart * bytesPerCluster, MFT_FILE_SIZE);

   FileRecordHeader* fileRecord = (FileRecordHeader*)mftFile;
   assert(fileRecord->magic == 0x454C4946);

   NonResidentAttributeHeader* dataAttribute = nullptr;
   uint64_t approximateRecordCount = 0;

   // Get dataAttribute and approximateRecordCount
   AttributeHeader* attribute = (AttributeHeader*)(mftFile + fileRecord->firstAttributeOffset);

   while (true) {
      if (attribute->attributeType == 0x80) { // $DATA
         dataAttribute = (NonResidentAttributeHeader*)attribute;
      }
      else if (attribute->attributeType == 0xB0) { // $BITMAP
         approximateRecordCount = ((NonResidentAttributeHeader*)attribute)->attributeSize * 8;
      }
      else if (attribute->attributeType == 0xFFFFFFFF) {
         break;
         }
      else if (m_stop)
            break;

      attribute = (AttributeHeader*)((uint8_t*)attribute + attribute->length);
   }

   assert(dataAttribute);

   //
   RunHeader* dataRun = (RunHeader*)((uint8_t*)dataAttribute + dataAttribute->dataRunsOffset);
   uint64_t clusterNumber = 0, recordsProcessed = 0;

   while (((uint8_t*)dataRun - (uint8_t*)dataAttribute) < dataAttribute->length && dataRun->lengthFieldBytes) {
      if (m_stop)
         break;

      uint64_t length = 0, offset = 0;

      for (int i = 0; i < dataRun->lengthFieldBytes; i++) {
         length |= (uint64_t)(((uint8_t*)dataRun)[1 + i]) << (i * 8);
      }

      for (int i = 0; i < dataRun->offsetFieldBytes; i++) {
         offset |= (uint64_t)(((uint8_t*)dataRun)[1 + dataRun->lengthFieldBytes + i]) << (i * 8);
      }

      if (offset & ((uint64_t)1 << (dataRun->offsetFieldBytes * 8 - 1))) {
         for (int i = dataRun->offsetFieldBytes; i < 8; i++) {
            offset |= (uint64_t)0xFF << (i * 8);
         }
      }

      clusterNumber += offset;
      dataRun = (RunHeader*)((uint8_t*)dataRun + 1 + dataRun->lengthFieldBytes + dataRun->offsetFieldBytes);

      uint64_t filesRemaining = length * bytesPerCluster / MFT_FILE_SIZE;
      uint64_t positionInBlock = 0;

      while (filesRemaining) {
         if (m_stop)
            break;

         if (recordsProcessed) {
            //_ftprintf(stdout, "Reading MFT... %d records processed\n", (int)(recordsProcessed));
            QString msg = QString("Indexing drive %1: Reading MFT... %2 records processed\n").arg(driveLetter).arg(recordsProcessed);
            statusMessage(QString(msg));

            if (approximateRecordCount) {
               uint64_t percentProcessed = (int)(recordsProcessed * 100 / approximateRecordCount);
               msg = QString("Indexing drive %1: Reading MFT... %2% of records processed\n").arg(driveLetter).arg(percentProcessed);
               //_ftprintf(stdout, "Reading MFT... %d%% of records processed\n", (int)(recordsProcessed * 100 / approximateRecordCount));
            }
         }
         

         uint64_t filesToLoad = MFT_FILES_PER_BUFFER;
         if (filesRemaining < MFT_FILES_PER_BUFFER) filesToLoad = filesRemaining;
         Read(&mftBuffer, clusterNumber * bytesPerCluster + positionInBlock, filesToLoad * MFT_FILE_SIZE);
         positionInBlock += filesToLoad * MFT_FILE_SIZE;
         filesRemaining -= filesToLoad;

         for (int i = 0; i < filesToLoad; i++) {
            if (m_stop)
               break;

            FileRecordHeader* fileRecord = (FileRecordHeader*)(mftBuffer + MFT_FILE_SIZE * i);
            recordsProcessed++;

            if (!fileRecord->inUse)
               continue;

            AttributeHeader* attribute = (AttributeHeader*)((uint8_t*)fileRecord + fileRecord->firstAttributeOffset);

            if (fileRecord->magic != 0x454C4946)
               break;

            File file = { 0,0,0 };
            file.isDirectory = fileRecord->isDirectory;

            while ((uint8_t*)attribute - (uint8_t*)fileRecord < MFT_FILE_SIZE) {
               if (m_stop)
                  break;

               if (attribute->attributeType == 0x30) { // $FILE_NAME
                  FileNameAttributeHeader* fileNameAttribute = (FileNameAttributeHeader*)attribute;

                  if (fileNameAttribute->namespaceType != 2 && !fileNameAttribute->nonResident) {

                     file.parent = fileNameAttribute->parentRecordNumber;
                     
                     wchar_t* fileName = _wcsdup(fileNameAttribute->fileName);
                     file.name = CW2A(fileName, CP_UTF8);
                     free(fileName);

                     int64_t oldLength = files.size();
                     if (fileRecord->recordNumber >= oldLength) {
                        files.resize(fileRecord->recordNumber + 1);
                     }

                     files[fileRecord->recordNumber] = file;
                  }
               }
               else if (attribute->attributeType == 0xFFFFFFFF) {
                  break;
               }

               attribute = (AttributeHeader*)((uint8_t*)attribute + attribute->length);
            }
         }
      }
   }

  
   int file_count = files.size();

   // Build pathnames
   for (size_t i = 0; i < file_count; i++) {
      if (m_stop)
         break;

      QString path = buildPath(driveLetter, files[(int)i]);

      if (path.size()) {
         distinct_directories.append(QFileInfo(path).absolutePath());
         distinct_files.append(path);
         //_ftprintf(stdout, "%s %s\n", files[i].isDirectory ? "DIR " : "FILE", path.toLocal8Bit().data());

         QString msg = QString("drive %1: Building %2 %3\n").arg(driveLetter).arg(files[(int)i].isDirectory ? "DIR " : "FILE").arg(path.toLocal8Bit().data());
         statusMessage(QString(msg));
      }

      int percent_done = 100*((int)i)/file_count;
      emit progress(m_volumeName, percent_done);
   }
   
   emit done(m_volumeName, distinct_files.size());
}

void IndexerThread::stop() {
   m_stop = true;
}
