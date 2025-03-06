#include "FinderUtil.h"

QString toString(FindType type) {
   QString out;
   switch (type) {
   case FindType::file:
      out = "File";
      break;
   case FindType::directory:
      out = "Folder";
      break;
   case FindType::link:
      out = "Link";
      break;
   default:
      out = "Other";
      break;
   }
   return out;
}

QString toString(ListType type) {
   QString out;
   switch (type) {
   case ListType::file:
      out = "list files";
      break;
   case ListType::directory:
      out = "list folders";
      break;
   case ListType::both:
      out = "list files and folders";
      break;
   default:
      out = "list none";
      break;
   }
   return out;
}

