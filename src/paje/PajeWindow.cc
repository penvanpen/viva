#include "PajeWindow.h"

PajeWindow *PajeWindow::instance = NULL;

PajeWindow::PajeWindow()
{
  spacetimeFrame = new PajeSpaceTimeFrame ();
  treemapFrame = new PajeTreemapFrame ();
  treemapFrame->hide();

  QSplitter *split = new QSplitter;
  split->setChildrenCollapsible (false);
  split->setOrientation (Qt::Horizontal);
  split->addWidget (spacetimeFrame);
  split->addWidget (treemapFrame);

  setCentralWidget (split);

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();

  setUnifiedTitleAndToolBarOnMac(true);
}

void PajeWindow::closeEvent(QCloseEvent *event)
{
  event->accept();
}

void PajeWindow::treemap()
{
  if (treemapFrame->isHidden()){
    app->connectTreemap ();
    treemapFrame->show();
  }else{
    app->disconnectTreemap ();
    treemapFrame->hide();
  }
}

void PajeWindow::spacetime()
{
  if (spacetimeFrame->isHidden()){
    app->connectSpacetime ();
    spacetimeFrame->show();
  }else{
    app->disconnectSpacetime ();
    spacetimeFrame->hide();
  }
}

void PajeWindow::about()
{
  QMessageBox::about(this, tr("About Paje"),
                     tr("<b>Paje</b>, a space/time viewer of Paje trace files."));
}

void PajeWindow::createActions()
{
  exitAct = new QAction(tr("E&xit"), this);
  exitAct->setShortcuts(QKeySequence::Quit);
  exitAct->setStatusTip(tr("Exit the application"));
  connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

  treemapAct = new QAction(tr("&Treemap View"), this);
  treemapAct->setStatusTip(tr("Enable or disable the treemap view"));
  connect(treemapAct, SIGNAL(triggered()), this, SLOT(treemap()));

  spacetimeAct = new QAction(tr("&Space/Time View"), this);
  spacetimeAct->setStatusTip(tr("Enable or disable the Space/Time view"));
  connect(spacetimeAct, SIGNAL(triggered()), this, SLOT(spacetime()));

  aboutAct = new QAction(tr("&About"), this);
  aboutAct->setStatusTip(tr("Show the application's About box"));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  aboutQtAct = new QAction(tr("About &Qt"), this);
  aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
  connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void PajeWindow::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(exitAct);

  viewMenu = menuBar()->addMenu(tr("&View"));
  viewMenu->addAction (spacetimeAct);
  viewMenu->addAction (treemapAct);

  menuBar()->addSeparator();

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutQtAct);
}

void PajeWindow::createToolBars()
{
}

void PajeWindow::createStatusBar()
{
  statusBar()->showMessage(tr("Ready"));
}
