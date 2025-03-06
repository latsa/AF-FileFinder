///////////////////////////////////////////////////////////////////////////////
// ListVolumes.cpp
//
#include "ListVolumes.h"
#include <algorithm>

// This works on Windows only. 
// TODO Provide an implementation for Linux as well.
#ifdef _WIN32

static char* GetVolumePaths(char* VolumeName ) {
   DWORD  CharCount = MAX_PATH + 1;
   char* Names = nullptr;
   BOOL   Success = FALSE;

   for (;;) {
      Names = new char[CharCount];
      ZeroMemory(Names,CharCount);

      if (!Names)
         return nullptr;

      Success = GetVolumePathNamesForVolumeNameA( VolumeName, Names, CharCount, &CharCount );
      if (Success)
         break;

      if (GetLastError() != ERROR_MORE_DATA)
         break;

      //  Try again with the  new suggested size.
      delete[] Names;
      Names = nullptr;
   }

   return Names;
}

WERROR GetVolumeCount(int* volcnt) {
   unsigned long  CharCount = 0;
   char  DeviceName[MAX_PATH] = "";
   char  VolumeName[MAX_PATH] = "";

   WERROR  Error = ERROR_SUCCESS;
   HANDLE FindHandle = INVALID_HANDLE_VALUE;

   int volumes_count = 0;

   //  Enumerate all volumes in the system.
   FindHandle = FindFirstVolumeA(VolumeName, ARRAYSIZE(VolumeName));

   if (FindHandle == INVALID_HANDLE_VALUE) {
      Error = GetLastError();
      return Error;
   }

   for (;;) {
      size_t Index = strlen(VolumeName) - 1;

      if (VolumeName[0] != '\\' || VolumeName[1] != '\\' || VolumeName[2] != '?' || VolumeName[3] != '\\' || VolumeName[Index] != '\\') {
         Error = ERROR_BAD_PATHNAME;
         break;
      }

      VolumeName[Index] = '\0';
      CharCount = QueryDosDeviceA(&VolumeName[4], DeviceName, ARRAYSIZE(DeviceName));
      VolumeName[Index] = '\\';

      if (CharCount == 0) {
         Error = GetLastError();
         break;
      }

      char* Names = GetVolumePaths(VolumeName);
      if (Names) {

         if (strlen(Names)) {
            for (char* NameIdx = Names; NameIdx[0]; NameIdx += strlen(NameIdx) + 1) {
               volumes_count++;
            }
         } else {
            volumes_count++;
         }

         delete[] Names;
         Names = nullptr;
      }

      BOOL Success = FindNextVolumeA(FindHandle, VolumeName, ARRAYSIZE(VolumeName));

      if (!Success) {
         Error = GetLastError();

         if (Error != ERROR_NO_MORE_FILES)
            break;

         Error = ERROR_SUCCESS;
         break;
      }
   }

   FindVolumeClose(FindHandle);

   *volcnt = volumes_count;
   return Error;
}

WERROR GetVolumeNames(Volume* volumes) {
   unsigned long  CharCount = 0;
   char  DeviceName[MAX_PATH] = "";
   char  VolumeName[MAX_PATH] = "";

   WERROR  Error = ERROR_SUCCESS;
   HANDLE FindHandle = INVALID_HANDLE_VALUE;

   int volumes_count = 0;

   FindHandle = FindFirstVolumeA(VolumeName, ARRAYSIZE(VolumeName));

   if (FindHandle == INVALID_HANDLE_VALUE)
      return GetLastError();

   for (;;) {
      size_t Index = strlen(VolumeName) - 1;

      if (VolumeName[0] != '\\' || VolumeName[1] != '\\' || VolumeName[2] != '?' || VolumeName[3] != '\\' || VolumeName[Index] != '\\') {
         Error = ERROR_BAD_PATHNAME;
         break;
      }

      VolumeName[Index] = '\0';
      CharCount = QueryDosDeviceA(&VolumeName[4], DeviceName, ARRAYSIZE(DeviceName));
      VolumeName[Index] = '\\';

      if (CharCount == 0) {
         Error = GetLastError();
         break;
      }

      char* Names = GetVolumePaths(VolumeName);
      if (Names) {

         if (strlen(Names)) {
            for (char* NameIdx = Names; NameIdx[0]; NameIdx += strlen(NameIdx) + 1) {
               strcpy(volumes[volumes_count].PathName, NameIdx);

               if (volumes[volumes_count].PathName[strlen(volumes[volumes_count].PathName)-1] == '\\')
                  volumes[volumes_count].PathName[strlen(volumes[volumes_count].PathName) - 1] = 0;

               strcpy(volumes[volumes_count].VolumeName, VolumeName);
               strcpy(volumes[volumes_count].DeviceName, DeviceName);
               volumes_count++;
            }
         } else {
            volumes[volumes_count].PathName[0] = 0;
            strcpy(volumes[volumes_count].VolumeName, VolumeName);
            strcpy(volumes[volumes_count].DeviceName, DeviceName);
            volumes_count++;
         }

         delete[] Names;
         Names = nullptr;
      }

      if (!FindNextVolumeA(FindHandle, VolumeName, ARRAYSIZE(VolumeName))) {
         Error = GetLastError();
         if (Error != ERROR_NO_MORE_FILES)
            break;

         Error = ERROR_SUCCESS;
         break;
      }
   }

   FindVolumeClose(FindHandle);
   return Error;
}

FINDER_EXPORT QVector<Volume> GetVolumes() {
   QVector<Volume> volumes;

   int volume_count = 0;
   if (!GetVolumeCount(&volume_count)) {
      Volume* vols = new Volume[volume_count];
      if (!GetVolumeNames(vols)) {
         for (int i = 0; i < volume_count; i++) {
            if (strlen(vols[i].PathName))
               volumes.append(vols[i]);
         }
      }
      delete[] vols;
   }

   //qSort(volumes.begin(), volumes.end(), [](const Volume& a, const Volume& b) { return a.PathName[0] < b.PathName[0]; });
   std::sort(volumes.begin(), volumes.end(), [](const Volume& a, const Volume& b) { return a.PathName[0] < b.PathName[0]; });
   return volumes;
}

#endif // _WIN32

// That's it
///////////////////////////////////////////////////////////////////////////////
