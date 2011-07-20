#include "IsisDebug.h"

#include "CnetEditorWindow.h"

#include <iostream>

#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QLayout>
#include <QFont>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
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

    createActions();
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
    
    loadingProgressBar = NULL;
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

//    aboutAct = new QAction(tr("&About"), this);
//    aboutAct->setStatusTip(tr("Show cneteditor's about box"));
//    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    quitAct = new QAction(QIcon(":quit"), tr("&Quit"), this);
    quitAct->setShortcut(tr("Ctrl+Q"));
    quitAct->setStatusTip(tr("Quit cneteditor"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
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

//    helpMenu = menuBar()->addMenu("&Help");
//    helpMenu->addAction(aboutAct);
  }


  void CnetEditorWindow::createToolBars()
  {
    mainToolBar = addToolBar(tr("Main ToolBar"));
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
  }


  void CnetEditorWindow::writeSettings()
  {
    QSettings settings(Filename(
        "$HOME/.Isis/cneteditor/cneteditor.config").Expanded().c_str(),
        QSettings::NativeFormat);

    // save window position and size
    settings.setValue("pos", pos());
    settings.setValue("size", size());
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
    switch (state)
    {
      case HasFile:
        openAct->setEnabled(false);
        saveAsAct->setEnabled(true);
        closeAct->setEnabled(true);
        setDirty(false);
        *curFile = filename;
        setWindowTitle(filename + "[*] - cneteditor");
        loadingProgressBar->setVisible(false);
        break;
        
      case NoFile:
        openAct->setEnabled(true);
        saveAsAct->setEnabled(false);
        closeAct->setEnabled(false);
        setDirty(false);
        saveAsPvl = false;
        *curFile = "";
        setWindowTitle("cneteditor");
        loadingProgressBar->setVisible(false);
        break;
        
      case FileLoading:
        openAct->setEnabled(false);
        saveAsAct->setEnabled(false);
        closeAct->setEnabled(false);
        *curFile = filename;
        setWindowTitle("cneteditor (loading " + filename + "...)");
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

      setFileState(NoFile, "");
    }
  }
  
  
  void CnetEditorWindow::networkLoaded(ControlNet * net)
  {
    cnet = net;
    setFileState(CnetEditorWindow::HasFile, *curFile);
    editorWidget = new CnetEditorWidget(cnet, Filename(
        "$HOME/.Isis/cneteditor/cneteditor.config").Expanded().c_str());
    connect(editorWidget, SIGNAL(cnetModified()), this, SLOT(setDirty()));
    setCentralWidget(editorWidget);
    setFileState(HasFile, *curFile);
    saveAsPvl = !Pvl((iString) *curFile).HasObject("ProtoBuffer");
  }


  void CnetEditorWindow::setDirty(bool state)
  {
    dirty = state;
    saveAct->setEnabled(state);
    setWindowModified(state);
  }
}

