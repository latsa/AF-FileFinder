#include "match2.h"
#include "match_config.h"
using namespace std;

// returns TRUE if text string matches glob pattern with * and ?
bool match2(const QString& glob, const QString& text) {
   int i = 0;
   int j = 0;
   int n = text.size();
   int m = glob.size();
   int text_backup = 0;
   int glob_backup = 0;
   bool nodot = !DOTGLOB;

   string gglob = glob.toLocal8Bit().data();
   string ttext = text.toLocal8Bit().data();

   while (i < n)
   {
      if (j < m)
      {
         switch (gglob[j])
         {
         case '*':
            // match anything except . after /
            if (nodot && ttext[i] == '.')
               break;
            // new star-loop: backup positions in pattern and text
            text_backup = i;
            glob_backup = ++j;
            continue;
         case '?':
            // match anything except . after /
            if (nodot && ttext[i] == '.')
               break;
            // match any character except /
            if (ttext[i] == PATHSEP)
               break;
            i++;
            j++;
            continue;
         case '[':
         {
            // match anything except . after /
            if (nodot && ttext[i] == '.')
               break;
            // match any character in [...] except /
            if (ttext[i] == PATHSEP)
               break;
            int lastchr;
            bool matched = false;
            bool reverse = j + 1 < m && (gglob[j+1] == '^' || gglob[j+1] == '!');
            // inverted character class
            if (reverse)
               j++;
            // match character class
            for (lastchr = 256; ++j < m && gglob[j] != ']'; lastchr = CASE(gglob[j]))
               if (lastchr < 256 && gglob[j] == '-' && j + 1 < m && gglob[j+1] != ']' ?
                  CASE(ttext[i]) <= CASE(gglob[++j]) && CASE(ttext[i]) >= lastchr :
                  CASE(ttext[i]) == CASE(gglob[j]))
                  matched = true;
            if (matched == reverse)
               break;
            i++;
            if (j < m)
               j++;
            continue;
         }
         case '\\':
            // literal match \-escaped character
            if (j + 1 < m)
               j++;
            // FALLTHROUGH
         default:
            // match the current non-NUL character
            if (CASE(gglob[j]) != CASE(ttext[i]) && !(gglob[j] == '/' && ttext[i] == PATHSEP))
               break;
            // do not match a . with *, ? [] after /
            nodot = !DOTGLOB && gglob[j] == '/';
            i++;
            j++;
            continue;
         }
      }
      if (glob_backup == 0 || ttext[text_backup] == PATHSEP)
         return false;
      // star-loop: backtrack to the last * but do not jump over /
      i = ++text_backup;
      j = glob_backup;
   }
   // ignore trailing stars
   while (j < m && gglob[j] == '*')
      j++;
   // at end of text means success if nothing else is left to match
   return j >= m;
}

