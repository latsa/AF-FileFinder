#include "index3.h"

Indexer3::Indexer3(Finder* parent) :
   m_parent(parent) {

}

// Cancel indexing...
void Indexer3::cancel() {
   m_is_about_close = true;
}


// Start indexing...
void Indexer3::start() {
}

bool Indexer3::query(const QString& root, const QStringList& expr, ListType type, QVector<Hit>& hits) {
   return false;
}

// 100 means no indexing in progress, any other value means indexing is in progress
int Indexer3::progress() {
   return 0;
}
