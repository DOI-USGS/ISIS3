#include "IsisDebug.h"

#include "CnetEditorWindow.h"

#include <iostream>

#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QLayout>
#include <QFont>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QQueue>
#include <QRadioButton>
#include <QSettings>
#include <QStatusBar>
#include <QString>
#include <QToolBar>

#include "ConcurrentControlNetReader.h"
#include "ControlNet.h"
#include "Filename.h"
#include "iException.h"
#include "ProgressBar.h"
#include "Pvl.h"

#include "CnetEditorWidget.h"


using std::cerr;


namespace Isis
{

  class CnetEditorFileDialog : public QFileDialog
  {

    public:
      CnetEditorFileDialog(QLayout * l, QWidget * parent = NULL);
  };

  CnetEditorFileDialog::CnetEditorFileDialog(QLayout * l,
      QWidget * parent) : QFileDialog(parent)
  {
    setAcceptMode(AcceptSave);
    setNameFilter("Control Network files (*.net *.bin);;All files (*)");
    QGridLayout * mainLayout = qobject_cast< QGridLayout * >(layout());
    ASSERT(mainLayout);

    if (mainLayout)
      mainLayout->addLayout(l, mainLayout->rowCount(), 0, 1, -1);
  }


  CnetEditorWindow::CnetEditorWindow()
  {
    // For some reason GUI style is not detected correctly by Qt for Isis.
    // This solution is less than ideal since it assumes one of at least 4
    // possible styles that could exist for X11 systems.  Plastique will
    // probably be the most common style for KDE users.  If its wrong (say it
    // was really Oxygen) then it will still be better than using Windows style,
    // which is what Qt is falling back to after it fails to choose the correct
    // style.
#ifdef Q_WS_X11
    QApplication::setStyle("Plastique");
#endif
#ifdef Q_WS_MACX
    QApplication::setStyle("macintosh");
#endif

    nullify();
    curFile = new QString;
    labelFont = new QFont("Sansserif", 9);
    
    toolBars = new QList< QToolBar * >;

    createActions();
    createDockWidgets();
    createMenus();
    createToolBars();
    createStatusBar();
    readSettings();

    setFileState(NoFile, "");

    if (QApplication::arguments().size() > 1)
    {
      load(QApplication::arguments().at(1));
    }
  }


  CnetEditorWindow::~CnetEditorWindow()
  {
    delete openAct;
    openAct = NULL;

    delete saveAct;
    saveAct = NULL;

    delete saveAsAct;
    saveAsAct = NULL;

    delete aboutAct;
    aboutAct = NULL;

    delete closeAct;
    closeAct = NULL;

    delete quitAct;
    quitAct = NULL;

    delete cnet;
    cnet = NULL;

    delete cnetReader;
    cnetReader = NULL;

    delete editorWidget;
    editorWidget = NULL;

    delete curFile;
    curFile = NULL;

    delete labelFont;
    labelFont = NULL;

    delete loadingProgressBar;
    loadingProgressBar = NULL;
    
    delete toolBars;
    toolBars = NULL;

    nullify();
  }


  void CnetEditorWindow::nullify()
  {
    cnet = NULL;
    cnetReader = NULL;
    editorWidget = NULL;
    curFile = NULL;
    labelFont = NULL;

    openAct = NULL;
    saveAct = NULL;
    saveAsAct = NULL;
    aboutAct = NULL;
    closeAct = NULL;
    quitAct = NULL;

    fileMenu = NULL;
    helpMenu = NULL;

    mainToolBar = NULL;
    toolBars = NULL;

    loadingProgressBar = NULL;

    pointTreeDockWidget = NULL;
    serialTreeDockWidget = NULL;
    connectionTreeDockWidget = NULL;

    pointFilterDockWidget = NULL;
    serialFilterDockWidget = NULL;
    connectionFilterDockWidget = NULL;
  }


  void CnetEditorWindow::closeEvent(QCloseEvent * event)
  {
    if (okToContinue())
    {
      writeSettings();

      if (editorWidget)
        editorWidget->writeSettings();

      event->accept();
    }
    else
    {
      event->ignore();
    }
  }


  void CnetEditorWindow::createActions()
  {
    openAct = new QAction(QIcon(":open"), tr("&Open"), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open a control network file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":save"), tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("save changes"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(QIcon(":saveAs"), tr("Save&As"), this);
    saveAsAct->setStatusTip(tr("Save control network to specified file"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    closeAct = new QAction(QIcon(":close"), tr("&Close"), this);
    closeAct->setStatusTip(tr("Close control net file"));
    connect(closeAct, SIGNAL(triggered()), this, SLOT(closeNetwork()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show cneteditor's about box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    quitAct = new QAction(QIcon(":quit"), tr("&Quit"), this);
    quitAct->setShortcut(tr("Ctrl+Q"));
    quitAct->setStatusTip(tr("Quit cneteditor"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
  }


  void CnetEditorWindow::createDockWidgets()
  {
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);

    pointTreeDockWidget = new QDockWidget("Point View", this, Qt::SubWindow);
    pointTreeDockWidget->setObjectName("PointTreeDock");
    pointTreeDockWidget->setFeatures(QDockWidget::DockWidgetFloatable |
        QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::BottomDockWidgetArea, pointTreeDockWidget);

    serialTreeDockWidget = new QDockWidget("Serial View", this, Qt::SubWindow);
    serialTreeDockWidget->setObjectName("SerialTreeDock");
    serialTreeDockWidget->setFeatures(QDockWidget::DockWidgetFloatable |
        QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::BottomDockWidgetArea, serialTreeDockWidget);
    tabifyDockWidget(pointTreeDockWidget, serialTreeDockWidget);

    connectionTreeDockWidget = new QDockWidget("Connection View",
        this, Qt::SubWindow);
    connectionTreeDockWidget->setObjectName("ConnectionTreeDock");
    connectionTreeDockWidget->setFeatures(QDockWidget::DockWidgetFloatable |
        QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::BottomDockWidgetArea, connectionTreeDockWidget);
    tabifyDockWidget(serialTreeDockWidget, connectionTreeDockWidget);
    pointTreeDockWidget->raise();

    pointFilterDockWidget = new QDockWidget("Filter Points and Measures",
        this, Qt::SubWindow);
    pointFilterDockWidget->setObjectName("PointFilterDock");
    pointFilterDockWidget->setFeatures(QDockWidget::DockWidgetFloatable |
        QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::BottomDockWidgetArea, pointFilterDockWidget);

    serialFilterDockWidget = new QDockWidget("Filter Images and Points",
        this, Qt::SubWindow);
    serialFilterDockWidget->setObjectName("SerialFilterDock");
    serialFilterDockWidget->setFeatures(QDockWidget::DockWidgetFloatable |
        QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::BottomDockWidgetArea, serialFilterDockWidget);
    tabifyDockWidget(pointFilterDockWidget, serialFilterDockWidget);

    connectionFilterDockWidget = new QDockWidget("Filter Connections",
        this, Qt::SubWindow);
    connectionFilterDockWidget->setObjectName("ConnectionFilterDock");
    connectionFilterDockWidget->setFeatures(QDockWidget::DockWidgetFloatable |
        QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::BottomDockWidgetArea, connectionFilterDockWidget);
    tabifyDockWidget(serialFilterDockWidget, connectionFilterDockWidget);
    pointFilterDockWidget->raise();
  }


  void CnetEditorWindow::createMenus()
  {
    fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAct);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);

    menuBar()->addMenu("&Tables");

    helpMenu = menuBar()->addMenu("&Help");
//     helpMenu->addAction(aboutAct);
  }


  void CnetEditorWindow::createToolBars()
  {
    mainToolBar = new QToolBar(tr("Main ToolBar"));
    mainToolBar->setObjectName("main toolbar");
    mainToolBar->setFloatable(false);
    //mainToolBar->setAllowedAreas(Qt::TopToolBarArea);
    mainToolBar->addAction(openAct);
    mainToolBar->addSeparator();
    mainToolBar->addAction(saveAct);
    mainToolBar->addAction(saveAsAct);
    mainToolBar->addSeparator();
    mainToolBar->addAction(closeAct);
    mainToolBar->addSeparator();

    addToolBar(Qt::TopToolBarArea, mainToolBar);
  }


  void CnetEditorWindow::createStatusBar()
  {
    loadingProgressBar = new ProgressBar("Reading network");
    statusBar()->addPermanentWidget(loadingProgressBar);
    loadingProgressBar->setVisible(false);
  }


  void CnetEditorWindow::readSettings()
  {
    QSettings settings(Filename(
        "$HOME/.Isis/cneteditor/cneteditor.config").Expanded().c_str(),
        QSettings::NativeFormat);

    // load window position and size
    QPoint pos = settings.value("pos", QPoint(100, 100)).toPoint();
    QSize size = settings.value("size", QSize(defaultWindowWidth,
        defaultWindowHeight)).toSize();
    resize(size);
    move(pos);

    setWindowIcon(QIcon(":usgs"));
    restoreState(settings.value("windowState").toByteArray());
  }


  void CnetEditorWindow::writeSettings()
  {
    QSettings settings(Filename(
        "$HOME/.Isis/cneteditor/cneteditor.config").Expanded().c_str(),
        QSettings::NativeFormat);

    // save window position and size
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("windowState", saveState());
  }


  bool CnetEditorWindow::okToContinue()
  {
    bool ok = true;

    if (dirty)
    {
      QString name = QFileInfo(*curFile).fileName();
      int r = QMessageBox::warning(this, tr("cneteditor"),
          "The network \"" + name + "\" has been modified.\n"
          "Do you want to save your changes or discard them?",
          QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

      if (r == QMessageBox::Cancel)
        ok = false;
      else
        if (r == QMessageBox::Save)
          save();
    }

    return ok;
  }


  void CnetEditorWindow::setDirty()
  {
    setDirty(true);
  }


  void CnetEditorWindow::setSaveAsPvl(int state)
  {
    saveAsPvl = (bool) state;
  }


  void CnetEditorWindow::open()
  {
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Open a control net file"), ".",
        tr("Control Network files (*.net *.bin);;All files (*)"));

    if (!filename.isEmpty())
      load(filename);
  }


  void CnetEditorWindow::setFileState(CnetEditorWindow::FileState state,
      QString filename)
  {
    if (centralWidget() && centralWidget() != editorWidget)
      setCentralWidget(NULL);

    switch (state)
    {
      case HasFile:
//         setDockWidgetsVisible(true);

        if (centralWidget() != editorWidget)
          setCentralWidget(editorWidget);

        openAct->setEnabled(false);
        saveAsAct->setEnabled(true);
        closeAct->setEnabled(true);
        setDirty(false);
        *curFile = filename;
        setWindowTitle(filename + "[*] - cneteditor *BETA VERSION*");
        loadingProgressBar->setVisible(false);
        break;

      case NoFile:
//         setDockWidgetsVisible(false);
        setCentralWidget(new QWidget());
        openAct->setEnabled(true);
        saveAsAct->setEnabled(false);
        closeAct->setEnabled(false);
        setDirty(false);
        saveAsPvl = false;
        *curFile = "";
        setWindowTitle("cneteditor *BETA VERSION*");
        loadingProgressBar->setVisible(false);
        break;

      case FileLoading:
//         setDockWidgetsVisible(false);
        setCentralWidget(new QWidget());
        openAct->setEnabled(false);
        saveAsAct->setEnabled(false);
        closeAct->setEnabled(false);
        *curFile = filename;
        setWindowTitle("cneteditor *BETA VERSION* (loading "
                       + filename + "...)");
        loadingProgressBar->setValue(loadingProgressBar->minimum());
        loadingProgressBar->setVisible(true);
        break;
    }
  }


  void CnetEditorWindow::load(QString filename)
  {
    try
    {
      cnetReader = new ConcurrentControlNetReader;

      // progress for reading the network
      connect(cnetReader, SIGNAL(progressRangeChanged(int, int)),
          loadingProgressBar, SLOT(setRange(int, int)));
      connect(cnetReader, SIGNAL(progressValueChanged(int)),
          loadingProgressBar, SLOT(setValue(int)));

      // call networkLoaded when the reading is complete
      connect(cnetReader, SIGNAL(networkReadFinished(ControlNet *)),
          this, SLOT(networkLoaded(ControlNet *)));

      cnetReader->read(filename);
      setFileState(FileLoading, filename);
    }
    catch (iException e)
    {
      QMessageBox::critical(this, tr("cneteditor"),
          tr("Failed to open the file provided"));
      e.Clear();
      setFileState(NoFile, "");
    }
  }


  void CnetEditorWindow::save()
  {
    ASSERT(!curFile->isEmpty());

    cnet->Write(*curFile, saveAsPvl);
    setDirty(false);
  }


  void CnetEditorWindow::saveAs()
  {
    QString whatsThis = "Use these radio buttons to select how the file will "
        "be saved.  Your choice is either plain text (in the PVL format) or "
        "binary.  Although the default is inherited from the currently opened "
        "file (no matter how big the network), binary is recommended ("
        "especially for large control networks).  Choose plain text if you "
        "need to be able to view and/or edit your control net file using a "
        "text editor.";

    QRadioButton * binButton = new QRadioButton("Save in binary format");
    binButton->setToolTip("Save the control network as a binary file");
    binButton->setWhatsThis(whatsThis);

    QRadioButton * pvlButton = new QRadioButton("Save in text (PVL) format");
    pvlButton->setToolTip("Save the control network in plain text (PVL "
        "format)");
    pvlButton->setWhatsThis(whatsThis);

    QButtonGroup * buttonGroup = new QButtonGroup;
    buttonGroup->addButton(binButton, 0);
    buttonGroup->addButton(pvlButton, 1);
    connect(buttonGroup, SIGNAL(buttonClicked(int)), this,
        SLOT(setSaveAsPvl(int)));

    if (saveAsPvl)
      pvlButton->click();
    else
      binButton->click();

    QHBoxLayout * buttonLayout = new QHBoxLayout;

    buttonLayout->addWidget(binButton);

    buttonLayout->addWidget(pvlButton);

    CnetEditorFileDialog * fileDialog = new CnetEditorFileDialog(buttonLayout);

    if (fileDialog->exec() == QDialog::Accepted)
    {
      QString filename = fileDialog->selectedFiles().value(0);

      if (!filename.isEmpty())
      {
        setFileState(CnetEditorWindow::HasFile, filename);
        save();
      }
    }
  }


  void CnetEditorWindow::closeNetwork()
  {
    if (okToContinue())
    {
      disconnect(editorWidget, SIGNAL(cnetModified()), this, SLOT(setDirty()));
      delete editorWidget;
      editorWidget = NULL;
      delete cnet;
      cnet = NULL;
      delete cnetReader;
      cnetReader = NULL;
      
      foreach (QToolBar * tb, *toolBars)
      {
        foreach (QAction * tbAct, tb->actions())
        {
          delete tbAct;
        }
        delete tb;
        toolBars->removeOne(tb);
      }
      
//       removeEmptyMenus();

      setFileState(NoFile, "");
    }
  }


  void CnetEditorWindow::networkLoaded(ControlNet * net)
  {
    cnet = net;
    editorWidget = new CnetEditorWidget(cnet, Filename(
        "$HOME/.Isis/cneteditor/cneteditor.config").Expanded().c_str());
    populateMenus();
    populateToolBars();
    connect(editorWidget, SIGNAL(cnetModified()), this, SLOT(setDirty()));
    setFileState(CnetEditorWindow::HasFile, *curFile);

    pointTreeDockWidget->setWidget(editorWidget->getPointTreeView());
    serialTreeDockWidget->setWidget(editorWidget->getSerialTreeView());
    connectionTreeDockWidget->setWidget(editorWidget->getConnectionTreeView());

    pointFilterDockWidget->setWidget(
        editorWidget->getPointFilterWidget());
    serialFilterDockWidget->setWidget(
        editorWidget->getSerialFilterWidget());
    connectionFilterDockWidget->setWidget(
        editorWidget->getConnectionFilterWidget());

    setFileState(HasFile, *curFile);
    saveAsPvl = !Pvl((iString) * curFile).HasObject("ProtoBuffer");
  }
  
  
  void CnetEditorWindow::populateMenus()
  {
    QMap< QAction *, QList< QString > > actionMap;
    actionMap = editorWidget->getMenuActions();
    QMapIterator< QAction *, QList< QString > > i(actionMap);

    QWidget * widget = NULL;
    
    while (i.hasNext())
    {
      i.next();
      QAction * act = i.key();
      QList< QString > location = i.value();
      
      widget = menuBar();
      
      while (location.size())
      {
        QString menuName = location.takeFirst();
        
        int actListIndex = indexOfActionList(widget->actions(), menuName);
        
        // if menuName not found in current widget's actions,
        // then add needed submenu
        if (actListIndex == -1)
        {
          QMenuBar * mb = qobject_cast< QMenuBar * >(widget);
          QMenu * m = qobject_cast< QMenu * >(widget);
          ASSERT((mb != NULL) ^ (m != NULL));
          
          if (mb)
          {
            mb->addMenu(menuName);
            int index = indexOfActionList(mb->actions(), menuName);
            widget = mb->actions()[index]->menu();
          }
          else
          {
            m->addMenu(menuName);
            int index = indexOfActionList(m->actions(), menuName);
            widget = m->actions()[index]->menu();
          }
        }
        else // menu exists so just update widget
        {
          widget = widget->actions()[actListIndex]->menu();
        }
      }
      
      widget->addAction(act);
    }
  }
  
  
  int CnetEditorWindow::indexOfActionList(QList< QAction * > actionList,
                                          QString actionText)
  {
    int index = -1;
    for (int i = 0; index == -1 && i < actionList.size(); i++)
      if (actionList[i]->text() == actionText)
        index = i;
      
    return index;
  }
  
  
  void CnetEditorWindow::populateToolBars()
  {
    QMap< QString, QList< QAction * > > actionMap;
    actionMap = editorWidget->getToolBarActions();
    QMapIterator< QString, QList< QAction * > > i(actionMap);

    while (i.hasNext())
    {
      i.next();
      QString objName = i.key();
      QList< QAction * > actionList = i.value();
      
//       foreach (QAction * action, actionList)
//       {
//         mainToolBar->addAction(action);
//       }
      
      // if toolbar already exists, just add the actions to it
      int index = indexOfToolBar(objName);
//       cerr << "index: " << index << "\n";
      if (index != -1)
      {
//         QToolBar * tb = (*toolBars)[index];
//         QList< QAction * > tbActions = tb->actions();
//         for (int i = tbActions.size() - 1; i >= 0; i--)
//         {
//           if (actionList.contains(tbActions[i]))
//           {
//             delete tbActions[i];
//             actionList.removeAt(i);
//           }
//         }
        
        foreach (QAction * action, actionList)
          (*toolBars)[index]->addAction(action);
      }
      // otherwise, it needs to be created
      else
      {
        // don't allow use of mainToolBar outside this class!
        if (objName != mainToolBar->objectName())
        {
          QToolBar * newToolBar = new QToolBar(objName);
          toolBars->append(newToolBar);
          newToolBar->setObjectName(objName);
          newToolBar->setFloatable(false);
          foreach (QAction * action, actionList)
            newToolBar->addAction(action);
          
          addToolBar(Qt::TopToolBarArea, newToolBar);
        }
      }
    }
  }
  
  
  int CnetEditorWindow::indexOfToolBar(QString objName)
  {
    ASSERT(toolBars);
    
    int index = -1;
    
    for (int i = 0; index == -1 && i < toolBars->size(); i++)
      if (toolBars->at(i)->objectName() == objName)
        index = i;

    return index;
  }


  void CnetEditorWindow::removeEmptyMenus()
  {
    QQueue< QWidget * > q;
    q.enqueue(menuBar());
    
    while (q.size())
    {
      QWidget * widget = q.dequeue();
      QList< QAction * > actionList = widget->actions();
      foreach (QAction * action, actionList)
      {
        QMenu * menu = action->menu();
        if (menu)
        {
          if (menu->actions().size())
            q.enqueue(menu);
          else
            widget->removeAction(action);
        }
      }
    }
  }
  
  
  void CnetEditorWindow::about()
  {
    cerr << "CneteditorWindow::about() implement me!\n";
  }


  void CnetEditorWindow::setDirty(bool state)
  {
    dirty = state;
    saveAct->setEnabled(state);
    setWindowModified(state);
  }


  void CnetEditorWindow::setDockWidgetsVisible(bool visibilityState)
  {
    pointTreeDockWidget->setVisible(visibilityState);
    serialTreeDockWidget->setVisible(visibilityState);
    connectionTreeDockWidget->setVisible(visibilityState);
    pointFilterDockWidget->setVisible(visibilityState);
    serialFilterDockWidget->setVisible(visibilityState);
    connectionFilterDockWidget->setVisible(visibilityState);
  }
}

