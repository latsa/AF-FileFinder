#include "match1.h"

bool match1(const QString& glob, const QString& text) {
   int i = 0;
   int j = 0;
   int n = text.size();
   int m = glob.size();
   int text_backup = 0;
   int glob_backup = 0;

   while (i < n) {
      if (j < m && glob.mid(j,1) == "*") {
         text_backup = i;
         glob_backup = ++j;
      } else if (j < m && (glob.mid(j, 1) == "?" || glob.mid(j, 1) == text.mid(i, 1))) {
         i++;
         j++;
      } else {
         if (glob_backup == 0)
            return false;

         i = ++text_backup;
         j = glob_backup;
      }
   }

   while (j < m && glob.mid(j, 1) == "*")
      j++;

   return j >= m;
}

