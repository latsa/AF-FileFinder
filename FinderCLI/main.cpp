///////////////////////////////////////////////////////////////////////////////
// This is a command line application that uses find() from FinderLib to
// list file and directory path names that match the query expressions provided
// at the command line. 
// 
// Query expressions are strings containing the special wildcard characters * 
// and ?. * matches any number of any character, and ? matches any single 
// character. All other characters match themselves.
//
// More than one query expression can be specified. They must be separated by 
// at least one space character. If the query expression contains spaces then
// put the query expression within double quotes, for example "*.h" ".c" . This
// expression will match all file names ending with ".h" or ".c" .

#include <QtCore/QCoreApplication>
#include "Finder.h"
#include <QCommandLineParser>
#include <QStringList>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <iostream>

static QStringList strings = {
   "Displays help on commandline options.",
   "Optional search root path. It defaults to current directory.",
   "Optional search string. A glob expression. It defaults to \"*\". If one or more search strings are specified, the search root path also needs to be specified.",
   "Find files only",
   "Find directories only",
   "A concise file and directory finder utility for Windows."
   };

void usage() {
   std::cout << 
   "Usage: findercli[options]\n"
   "       findercli[options] <root> [<expr>]\n" + strings[5].toStdString() + "\n"
   "\nOptions:\n"
   "   -h                        " + strings[0].toStdString() + "\n"
   "   -v                        " "Displays version information.\n"
   "   -f                        " + strings[3].toStdString() + "\n"
   "   -d                        " + strings[4].toStdString() + "\n"
   "\nArguments:\n"
   "   <root>                    " + strings[1].toStdString() + "\n"
   "   <expr>                    " + strings[2].toStdString() + "\n"
   ;
}

bool is_valid_expr(const QString& search_expr) {
   return true;
}

bool is_valid_path(const QString& search_path) {
   QDir dir(search_path);
   return dir.exists();
}

int main(int argc, char* argv[]) {
   QString appName("Finder");
   #define app_name appName.toStdString().data()

   QCoreApplication a(argc, argv);
   QCoreApplication::setApplicationName(appName);
   QCoreApplication::setApplicationVersion("1.0");

   QCommandLineParser parser;
   parser.setApplicationDescription(strings[5]);

   //parser.addHelpOption();
   QCommandLineOption helpOption("h", strings[0]);
   parser.addOption(helpOption);
   
   parser.addVersionOption();

   parser.addPositionalArgument("root", strings[1]);
   parser.addPositionalArgument("expr", strings[2]);

   QCommandLineOption filesOnlyOption("f", strings[3]);
   parser.addOption(filesOnlyOption);
   QCommandLineOption dirsOnlyOption("d", strings[4]);
   parser.addOption(dirsOnlyOption);

   parser.process(a);

   if (parser.isSet(helpOption)) {
      usage();
      exit(0);
   }
   
   const QStringList args = parser.positionalArguments();

   QString search_root(".");
   if (args.size() > 0)
      search_root = args.at(0);

   QStringList search_expressions;
   for (int i = 1; i < args.size(); i++)
      search_expressions.append(args.at(i));

   if (search_expressions.size() < 1)
      search_expressions.append("*");

   bool files_only = parser.isSet(filesOnlyOption);
   bool dirs_only = parser.isSet(dirsOnlyOption);

   if (files_only && dirs_only) {
      printf("%s: WARNING: Options -f and -d are exclusive. Both files and directories will be listed. \n", app_name);
      files_only = false;
      dirs_only = false;
   }

   ListType list_type = ListType::both;
   if (files_only)
      list_type = ListType::file;
   if (dirs_only)
      list_type = ListType::directory;

   int errors = 0;
   for (int i = 1; i < search_expressions.size(); i++) {
      QString search_expr = search_expressions.at(i);
      if (!is_valid_expr(search_expressions.at(i))) {
         printf("%s: ERROR: Search expression #%d is invalid.\n", app_name, i);
         errors++;
      }
   }

   if (errors)
      exit(errors);

   if (!is_valid_path(search_root)) {
      printf("%s: ERROR: Search root does not exist.\n", app_name);
      exit(2);
   }

   qDebug() << "Search root:";
   qDebug() << search_root;

   qDebug() << "Search filters:";
   foreach(const QString & search_expr, search_expressions) {
      qDebug() << "   " << search_expr;
   }


   ////////////////////////////////////////////////////////////////////////////
   //
   qint64 start_time = QDateTime::currentMSecsSinceEpoch();

   Finder finder;
   int entry_count = finder.find(search_root, search_expressions, list_type);

   qint64 end_time = QDateTime::currentMSecsSinceEpoch();
   //
   ////////////////////////////////////////////////////////////////////////////

#ifdef DUMP_WHEN_ALL_FOUND
   foreach(const Hit &hit, finder.m_hits) {
      hit.dump();
   }
#endif

   qint64 duration = end_time - start_time;
   qDebug() << "The operation took " << duration << "milliseconds.";
   qDebug() << "Number of entries: " << entry_count;

   return 0;
}
