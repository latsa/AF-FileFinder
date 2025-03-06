#pragma once
#include "FinderGlobal.h"
#include "FinderTypes.h"
#include "Finder.h"
#include <QStringList>
#include <QVector>



bool FINDER_EXPORT walk(const QString& root, const QStringList& expr, ListType type, QVector<Hit>& hits, Finder* parent);

