#include "librarymerger.h"

#include <QApplication>
#ifdef Q_DEBUG
   #include <QDebug>
#endif

int main( int argc, char *argv[] ) {
   QApplication a( argc, argv );
   LibraryMerger w;
   w.show();
   return a.exec();
}
