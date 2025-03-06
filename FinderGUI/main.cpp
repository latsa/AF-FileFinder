#include "FinderGUI.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   QApplication::setApplicationName("Finder");
   QApplication::setApplicationVersion("1.0");

   FinderGUI w;
   w.show();
   return a.exec();
}
