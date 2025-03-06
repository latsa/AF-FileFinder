#pragma once
#include "Finder.h"
#include <QStringList>
#include <QVector>

bool walk3(const QString& root, const QStringList& expr, ListType type, QVector<Hit>& hits, Finder* parent);

