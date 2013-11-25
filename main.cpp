#include <QApplication>

#include "mainwindow.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  MainWindow mainWindow;
  
  mainWindow.resize(400, 400);
  mainWindow.setWindowTitle("MetaPost Previewer");
  mainWindow.setWindowFlags(Qt::WindowStaysOnTopHint);
  mainWindow.show();

  return app.exec();
}
