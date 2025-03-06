#pragma once
#include "Finder.h"
#include <QStringList>
#include <QFileInfo>
#include <QDir>

bool match(const QFileInfo& fileInfo, const QStringList& expr);
bool match(const QString& fileName, const QStringList& expr);