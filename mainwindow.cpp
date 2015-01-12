#include "mainwindow.h"

MainWindow::MainWindow()
{
  metaView = NULL;
  createTimer();
  createActions();
  createToolBar();
  createErrorTabs();
  createStatusBar();
  scaleFactor = 1.0;
  setWindowFlags(Qt::WindowStaysOnTopHint);
  loadSettings();
}

void MainWindow::createTimer()
{
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(update()));
  timer->start(1000);
}

void MainWindow::createStatusBar()
{
  statusBar()->showMessage(tr("MetaView"));
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
  showErrorAct->setStatusTip(tr("Show Error"));
  showErrorAct->setCheckable(true);
  showErrorAct->setChecked(false);
  connect(showErrorAct, SIGNAL(toggled(bool)), this, SLOT(showError(bool)));
  
  closeAct = new QAction(tr("E&xit"), this);
  closeAct->setShortcuts(QKeySequence::Quit);
  closeAct->setStatusTip(tr("Exit the application"));
  connect(closeAct, SIGNAL(triggered()), this, SLOT(close()));
  
  onTopAct = new QAction(tr("On Top"), this);
  onTopAct->setCheckable(true);
  onTopAct->setChecked(true);
  connect(onTopAct, SIGNAL(toggled(bool)), this, SLOT(putOnTop(bool)));
}

void MainWindow::putOnTop(bool onTop)
{
  if (onTop)
  {
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
  }
  else
  {
    setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
  }
  this->show();
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
  epsFiles->setMinimumContentsLength(34);
  epsFiles->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
  connect(epsFiles, SIGNAL(activated(const QString&)), this, SLOT(change(const QString&)));

  toolBar = addToolBar("Main");
  toolBar->addAction(newFolderAct);
  toolBar->addAction(zoomInAct);
  toolBar->addAction(zoomOutAct);
  toolBar->addAction(showErrorAct);
  toolBar->addAction(onTopAct);
  toolBar->addWidget(epsFiles);
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
  addDockWidget(Qt::BottomDockWidgetArea, errorWindow);
  errorWindow->setVisible(false);
}

void MainWindow::showError(bool notVisible)
{
  if (notVisible)
    errorWindow->setVisible(true);
  else
    errorWindow->setVisible(false);
}

void MainWindow::change(const QString& fileName)
{
  if (fileName.size() > 0) {
    QString epsFileName = workingDirPath + "/" + fileName;
    
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
      setWindowTitle(activeEpsFile);
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
  QFile(QCoreApplication::applicationDirPath() + "/standalone.cls").copy(workingDirPath + "/.metaview/standalone.cls");
  QFile(QCoreApplication::applicationDirPath() + "/standalone.sty").copy(workingDirPath + "/.metaview/standalone.sty");
  QFile(QCoreApplication::applicationDirPath() + "/standalone.cfg").copy(workingDirPath + "/.metaview/standalone.cfg");

  getEpsFiles();
  getMpFiles();
}

void MainWindow::update()
{
  statusBar()->showMessage(tr("Scanning..."));
  bool somethingWasCompiled = false;
  bool anErrorOccured = false;
  for (int i = 0; i < mpFiles.size(); i++) {
    QFileInfo info(mpFiles[i]);
    QDateTime currentTime = info.lastModified();
    
    if (currentTime > modTimes[i]) {
      somethingWasCompiled = true;
      QList<QString> arguments;
      arguments.append(mpFiles[i]);
      
      statusBar()->showMessage(tr("COMPILING..."));
      QProcess *process = new QProcess(this);
      process->start(QCoreApplication::applicationDirPath() + "/mpeps.py", arguments);
      process->waitForFinished();

      if (process->exitCode() != 0) {
        statusBar()->showMessage(tr("ERROR OCCURED!"));
        anErrorOccured = true;
        QString fileName = mpFiles[i];
        fileName.remove(fileName.size() - 3, 3);
        QFile logFile(fileName + ".log");
        QFile mpFile(mpFiles[i]);
        QFile mpxFile(fileName + ".mpx");
        errorTabLog->displayFile(logFile);
        errorTabMp->displayFile(mpFile);
        errorTabMpx->displayFile(mpxFile);
        showErrorAct->setChecked(true);
      }
      modTimes[i] = currentTime;
    }
  }
 
  if (somethingWasCompiled && !anErrorOccured) {
    statusBar()->showMessage(tr("SUCCESS!"));
    errorTabLog->clear();
    errorTabMp->clear();
    errorTabMpx->clear();
    getEpsFiles();
    getMpFiles();
    change(activeEpsFile);

    int index = epsFiles->findText(activeEpsFile);
    qDebug() << index;
    if (index != -1)
      epsFiles->setCurrentIndex(index);
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
  int maximumContentsLength = 0;
  QString comboBoxItem;

  for (int i = 0; i < listOfEpsFiles.size(); i++)
  {
    comboBoxItem = listOfEpsFiles[i].remove(0, workingDirPath.size() + 1);
    epsFiles->addItem(comboBoxItem);
    if (comboBoxItem.size() > maximumContentsLength)
      maximumContentsLength = comboBoxItem.size();
  }
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

void MainWindow::saveSettings()
{
  QSettings settings("MV", "MetaView");
  settings.beginGroup("MainWindow");

  // save position and size
  settings.setValue("position", this->pos());
  settings.setValue("size", this->size());

  // save project
  settings.setValue("path", workingDirPath);

  settings.endGroup();
}

void MainWindow::loadSettings()
{
  QSettings settings("MV", "MetaView");
  settings.beginGroup("MainWindow");

  // load geometry
  move(settings.value("position").toPoint());
  if (settings.contains("size"))
    resize(settings.value("size").toSize());
  else
    resize(400, 600);
 
  // load project
  QStringList path = settings.value("path").toStringList();
  if (path.size() > 0)
  {
    workingDirPath = path.at(0);
    QDir(workingDirPath).mkdir(".metaview");
    QFile(QCoreApplication::applicationDirPath() + "/standalone.cls").copy(workingDirPath + "/.metaview/standalone.cls");
    QFile(QCoreApplication::applicationDirPath() + "/standalone.sty").copy(workingDirPath + "/.metaview/standalone.sty");
    QFile(QCoreApplication::applicationDirPath() + "/standalone.cfg").copy(workingDirPath + "/.metaview/standalone.cfg");

    getEpsFiles();
    getMpFiles();
  }

  settings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  saveSettings();
  event->accept();
}
