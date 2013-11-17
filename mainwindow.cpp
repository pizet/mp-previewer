#include "mainwindow.h"

MainWindow::MainWindow()
{
  metaView = NULL;
  createTimer();
  createActions();
  createMenus();
  createToolBar();
  createErrorTabs();
  scaleFactor = 1.0;
}

void MainWindow::createTimer()
{
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(update()));
  timer->start(1000);
}

void MainWindow::createActions()
{
  newFolderAct = new QAction(QIcon(":/images/new-folder.png"), tr("&Open..."), this);
  newFolderAct->setShortcuts(QKeySequence::Open);
  newFolderAct->setStatusTip(tr(":/images/Choose an existing directory"));
  connect(newFolderAct, SIGNAL(triggered()), this, SLOT(open()));
  
  zoomInAct = new QAction(QIcon(":/images/zoom-in.png"), tr("Zoom In"), this);
  zoomInAct->setStatusTip(tr("Zoom in"));
  connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));
  
  zoomOutAct = new QAction(QIcon(":/images/zoom-out.png"), tr("Zoom Out"), this);
  zoomOutAct->setStatusTip(tr("Zoom out"));
  connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));
  
  showErrorAct = new QAction(QIcon(":/images/error.png"), tr("Show Error"), this);
  showErrorAct->setStatusTip(tr("Zoom out"));
  
  closeAct = new QAction(tr("E&xit"), this);
  closeAct->setShortcuts(QKeySequence::Quit);
  closeAct->setStatusTip(tr("Exit the application"));
  connect(closeAct, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(newFolderAct);
  fileMenu->addAction(zoomInAct);
  fileMenu->addAction(zoomOutAct);
  fileMenu->addSeparator();
  fileMenu->addAction(closeAct);
}

void MainWindow::createToolBar()
{
  epsFiles = new QComboBox(this);
  connect(epsFiles, SIGNAL(activated(const QString&)), this, SLOT(change(const QString&)));

  toolBar = addToolBar("Main");
  toolBar->addAction(newFolderAct);
  toolBar->addWidget(epsFiles);
  toolBar->addAction(zoomInAct);
  toolBar->addAction(zoomOutAct);
  toolBar->addAction(showErrorAct);
}

void MainWindow::createErrorTabs()
{
  errorWindow = new QDockWidget(tr("Error Window"), this);

  errorTabLog = new ErrorTab(this);
  errorTabMp = new ErrorTab(this);
  errorTabMpx = new ErrorTab(this);

  errorTabs = new QTabWidget(this);
  errorTabs->addTab(errorTabLog, tr("log"));
  errorTabs->addTab(errorTabMp, tr("mp"));
  errorTabs->addTab(errorTabMpx, tr("mpx"));

  errorWindow->setWidget(errorTabs);
  addDockWidget(Qt::RightDockWidgetArea, errorWindow);
  connect(showErrorAct, SIGNAL(toggled(bool)), errorWindow, SLOT(setVisible(bool)));
}

void MainWindow::change(const QString& fileName)
{
  if (fileName.size() > 0) {
    QString epsFileName = fileName;
    
    QStringList arguments;
    arguments.append(epsFileName);
    arguments.append(workingDirPath);
    arguments.append(QString::number(scaleFactor));

    QProcess *process = new QProcess(this);
    process->start(QCoreApplication::applicationDirPath() + "/epspdf.py", arguments);
    process->waitForFinished();
   
    QString pdfFileName = workingDirPath + QString("/.metaview/");
    int pos = epsFileName.size() - 1;
    while (epsFileName[pos] != '/') pos--;
    epsFileName.remove(0, pos + 1);
    epsFileName.remove(epsFileName.size() - 4, 4);
    pdfFileName += epsFileName + QString(".pdf");
    
    if (metaView == NULL) {
      metaView = new MetaView(pdfFileName, this);
      setCentralWidget(metaView);
    } else {
      metaView->updateMetaView(pdfFileName);
    }

    if (fileName != activeEpsFile) {
      activeEpsFile = fileName;
      scaleFactor = 1.0;
    }
  }
}

void MainWindow::open()
{
  workingDirPath = QFileDialog::getExistingDirectory(
      this, tr("Choose Directory"), "/home",
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  
  QDir(workingDirPath).mkdir(".metaview");

  getEpsFiles();
  getMpFiles();
}

void MainWindow::update()
{
  bool somethingWasCompiled = false;
  bool anErrorOccured = false;
  for (int i = 0; i < mpFiles.size(); i++) {
    QFileInfo info(mpFiles[i]);
    QDateTime currentTime = info.lastModified();
    
    if (currentTime > modTimes[i]) {
      somethingWasCompiled = true;
      QList<QString> arguments;
      arguments.append(mpFiles[i]);
      
      QProcess *process = new QProcess(this);
      process->start(QCoreApplication::applicationDirPath() + "/mpeps.py", arguments);
      process->waitForFinished();

      if (process->exitCode() != 0) {
        anErrorOccured = true;
        QString fileName = mpFiles[i];
        fileName.remove(fileName.size() - 3, 3);
        QFile logFile(fileName + ".log");
        QFile mpFile(mpFiles[i]);
        QFile mpxFile(fileName + ".mpx");
        errorTabLog->displayFile(logFile);
        errorTabMp->displayFile(mpFile);
        errorTabMpx->displayFile(mpxFile);
      }
      modTimes[i] = currentTime;
    }
  }
 
  if (somethingWasCompiled && !anErrorOccured) {
    getEpsFiles();
    getMpFiles();
    change(activeEpsFile);
  }
}

void MainWindow::zoomIn()
{
  scaleFactor += 0.25;
  change(activeEpsFile);
}

void MainWindow::zoomOut()
{
  scaleFactor -= 0.25;
  change(activeEpsFile);
}

QList<QString> MainWindow::getExtFiles(QString dirPath, QString ext)
{
  QDir dir(dirPath);

  QList<QString> extFiles;
  extFiles = dir.entryList(QStringList(ext));
  for (int i = 0; i < extFiles.size(); i++)
    extFiles[i] = dirPath + "/" + extFiles[i];

  dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);

  QList<QString> subdirs;
  subdirs = dir.entryList();

  if (subdirs.size() == 0)
    return extFiles;
  else {
    for (int i = 0; i < subdirs.size(); i++)
      extFiles += getExtFiles(dirPath + "/" + subdirs[i], ext);
    return extFiles;
  }
}

void MainWindow::getEpsFiles()
{
  epsFiles->clear();
  listOfEpsFiles = getExtFiles(workingDirPath, "*.eps");
  for (int i = 0; i < listOfEpsFiles.size(); i++)
    epsFiles->addItem(listOfEpsFiles[i]);
}

void MainWindow::getMpFiles()
{
  mpFiles = getExtFiles(workingDirPath, "*.mp");
  modTimes = QList<QDateTime>();
  for (int i = 0; i < mpFiles.size(); i++) {
    QFileInfo info(mpFiles[i]);
    modTimes.append(info.lastModified());
  }
}
