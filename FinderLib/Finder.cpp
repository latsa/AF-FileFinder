///////////////////////////////////////////////////////////////////////////////
// Finder.cpp
//
#include "Finder.h"
#include "Walker.h"

Finder::Finder() {
   // Enumerate DOS volumes
   m_volumes = GetVolumes();
}

void Finder::cancel() {
   m_is_about_close = true;
}

int Finder::find(const QString& search_root, const QStringList& search_expressions, ListType type) {

   m_is_about_close = false;
   m_search_root = search_root;
   m_search_expressions = search_expressions;
   m_type = type;

   walk(m_search_root, m_search_expressions, m_type, m_hits, this);

   return m_hits.size();
}

void Finder::backend_walking(const Hit& hit) {
   emit walking(hit);
}

void Finder::backend_found(const Hit& hit) {
   emit found(hit);
}

void Finder::backend_status(QString msg) {
   emit status(msg);
}

// That's it
///////////////////////////////////////////////////////////////////////////////
