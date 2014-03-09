#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QCoreApplication::setOrganizationName("Open Source");
  QCoreApplication::setOrganizationDomain("github.com/Xazax-hun/CppQuery");
  QCoreApplication::setApplicationName("CppQuery");

  CppQuery::MainWindow w;
  w.show();

  return app.exec();
}
