#include <QApplication>
#include <QDir>
#include <QString>
#include <QTextStream>

#include "mainwindow.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  MainWindow window;
  window.resize(400, 600);
  window.show();

  return app.exec();
}
