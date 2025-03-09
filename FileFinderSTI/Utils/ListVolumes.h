///////////////////////////////////////////////////////////////////////////////
// ListVolumes.h
#pragma once
#include "FinderGlobal.h"

#ifdef _WIN32
#include <QVector>
#include <windows.h>

struct Volume {
   char  PathName[MAX_PATH] = "";
   char  DeviceName[MAX_PATH] = "";
   char  VolumeName[MAX_PATH] = "";
};

typedef unsigned long WERROR;

FINDER_EXPORT WERROR GetVolumeCount(int* volcnt);
FINDER_EXPORT WERROR GetVolumeNames(Volume* volumes);

FINDER_EXPORT QVector<Volume> GetVolumes();
#endif

// That's it
///////////////////////////////////////////////////////////////////////////////
