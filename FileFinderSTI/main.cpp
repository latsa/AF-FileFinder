#include "FileFinderSTI.h"

#ifndef QT_NO_SYSTEMTRAYICON

#include <QMessageBox>

int main(int argc, char* argv[]) {
   Q_INIT_RESOURCE(FileFinderSTI);

   QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

   QApplication app(argc, argv);

   if (!QSystemTrayIcon::isSystemTrayAvailable()) {
      QMessageBox::critical(nullptr, QObject::tr("File Finder"),
         QObject::tr("I couldn't detect any system tray "
            "on this system."));
      return 1;
   }
   QApplication::setQuitOnLastWindowClosed(false);

   FileFinderSTI window;
   //window.show();
   return app.exec();
}

#else

#include <QLabel>
#include <QDebug>

int main(int argc, char* argv[])
{
   QApplication app(argc, argv);
   QString text("QSystemTrayIcon is not supported on this platform.");

   QLabel* label = new QLabel(text);
   label->setWordWrap(true);

   label->show();
   qDebug() << text;

   app.exec();
}

#endif

