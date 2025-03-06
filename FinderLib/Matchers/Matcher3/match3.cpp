#include "match3.h"
#include "match_config.h"
using namespace std;

// returns TRUE if text string matches gitignore-style glob pattern
// gitignore glob match
bool match3(const QString& glob, const QString& text) {
   int i = 0;
   int j = 0;
   int n = text.size();
   int m = glob.size();
   int text1_backup = 0;
   int glob1_backup = 0;
   int text2_backup = 0;
   int glob2_backup = 0;
   bool nodot = !DOTGLOB;

   string gglob = glob.toLocal8Bit().data();
   string ttext = text.toLocal8Bit().data();

   // match pathname if glob contains a / otherwise match the basename
   if (j + 1 < m && gglob[j] == '/')
   {
      // if pathname starts with ./ then ignore these pairs
      while (i + 1 < n && ttext[i] == '.' && ttext[i + 1] == PATHSEP)
         i += 2;
      // if pathname starts with a / then ignore it
      if (i < n && ttext[i] == PATHSEP)
         i++;
      j++;
   }
   else if (gglob.find('/') == string::npos)
   {
      size_t sep = ttext.rfind(PATHSEP);
      if (sep != string::npos)
         i = sep + 1;
   }
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
            if (++j < m && gglob[j] == '*')
            {
               // trailing ** match everything after /
               if (++j >= m)
                  return true;
               // ** followed by a / match zero or more directories
               if (gglob[j] != '/')
                  return false;
               // new **-loop, discard *-loop
               text1_backup = 0; //string::npos;
               glob1_backup = 0; //string::npos;
               text2_backup = i;
               glob2_backup = ++j;
               continue;
            }
            // trailing * matches everything except /
            text1_backup = i;
            glob1_backup = j;
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
            bool reverse = j + 1 < m && (gglob[j + 1] == '^' || gglob[j + 1] == '!');
            // inverted character class
            if (reverse)
               j++;
            // match character class
            for (lastchr = 256; ++j < m && gglob[j] != ']'; lastchr = CASE(gglob[j]))
               if (lastchr < 256 && gglob[j] == '-' && j + 1 < m && gglob[j + 1] != ']' ?
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
      if (glob1_backup != string::npos && ttext[text1_backup] != PATHSEP)
      {
         // *-loop: backtrack to the last * but do not jump over /
         i = ++text1_backup;
         j = glob1_backup;
         continue;
      }
      if (glob2_backup != string::npos)
      {
         // **-loop: backtrack to the last **
         i = ++text2_backup;
         j = glob2_backup;
         continue;
      }
      return false;
   }
   // ignore trailing stars
   while (j < m && gglob[j] == '*')
      j++;
   // at end of text means success if nothing else is left to match
   return j >= m;
}


