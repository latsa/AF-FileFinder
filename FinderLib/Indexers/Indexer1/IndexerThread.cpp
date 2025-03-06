#include "IndexerThread.h"


IndexerThread::IndexerThread(QVector<Hit>& all_hits, Finder* parent)
    : QThread(parent), m_parent(parent), m_all_hits(all_hits) {
}

IndexerThread::~IndexerThread() {
}

void IndexerThread::run() {
   //..
   emit done(m_all_hits.size());
}

