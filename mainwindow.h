#include <QAction>
#include <QCloseEvent>
#include <QComboBox>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QDebug>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QIcon>
#include <QList>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMap>
#include <QProcess>
#include <QPushButton>
#include <QSettings>
#include <QStatusBar>
#include <QString>
#include <QTabWidget>
#include <QToolBar>
#include <QTimer>

#include "metaview.h"
#include "metaerror.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:

  MainWindow();

public slots:

  void change(const QString&);
  void open();
  void update();
  void zoomIn();
  void zoomOut();
  void putOnTop(bool);
  void showError(bool);

private:
  void createActions();
  void createErrorTabs();
  void createMenus();
  void createTimer();
  void createToolBar();
  void createStatusBar();
  void getEpsFiles();
  QList<QString> getExtFiles(QString, QString);
  void getMpFiles();
  void saveSettings();
  void loadSettings();
  void closeEvent(QCloseEvent *event);

  MetaView *metaView;
  ErrorTab *errorTabLog;
  ErrorTab *errorTabMp;
  ErrorTab *errorTabMpx;
  double scaleFactor;
  QComboBox *epsFiles;
  QDockWidget *errorWindow;
  QList<QString> listOfEpsFiles;
  QList<QString> mpFiles;
  QList<QDateTime> modTimes;
  QString workingDirPath;
  QString activeEpsFile;
  QTabWidget *errorTabs;
  QTimer *timer;
  QToolBar *toolBar;
  QMenu *fileMenu;
  QAction *newFolderAct;
  QAction *zoomInAct;
  QAction *zoomOutAct;
  QAction *showErrorAct;
  QAction *closeAct;
  QAction *onTopAct;
};
