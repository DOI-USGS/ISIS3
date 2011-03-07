#include "CnetEditorWindow.h"

#include <iostream>

#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QRadioButton>
#include <QSettings>
#include <QStatusBar>
#include <QString>
#include <QToolBar>

#include "CnetEditorWidget.h"
#include "ControlNet.h"
#include "iException.h"
#include "Pvl.h"


using std::cerr;



const int defaultWindowWidth = 1100;
const int defaultWindowHeight = 700;


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
    setNameFilter("Control Network files (*.net);;All files (*)");
    QGridLayout * mainLayout = dynamic_cast< QGridLayout * >(layout());
    Q_ASSERT(mainLayout);
    if (mainLayout)
      mainLayout->addLayout(l, mainLayout->rowCount(), 0, 1, -1);
  }


  CnetEditorWindow::CnetEditorWindow()
  {
    // For some reason GUI style is not detected correctly by Qt for Isis.
    // This solution is less than ideal since it assumes one of at least 4
    // possible styles that could exist for X11 systems.  Plastique will
    // probably be the most common for KDE users.  If its wrong (say it was
    // really Oxygen) then it will still be better than using Windows style.
#ifdef Q_WS_X11
    QApplication::setStyle("Plastique");
#endif
#ifdef Q_WS_MACX
    QApplication::setStyle("macintosh");
#endif

    nullify();
    curFile = new QString;

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    readSettings();

    setNoFileState();
  }


  CnetEditorWindow::~CnetEditorWindow()
  {
    if (curFile)
    {
      delete curFile;
      curFile = NULL;
    }

    nullify();
  }


  void CnetEditorWindow::nullify()
  {
    cnet = NULL;
    editorWidget = NULL;
    curFile = NULL;

    openAct = NULL;
    saveAct = NULL;
    saveAsAct = NULL;
    aboutAct = NULL;
    closeAct = NULL;
    quitAct = NULL;

    fileMenu = NULL;
    helpMenu = NULL;

    mainToolBar = NULL;
  }


  void CnetEditorWindow::closeEvent(QCloseEvent * event)
  {
    if (okToContinue())
    {
      writeSettings();
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

    addToolBar(Qt::TopToolBarArea, mainToolBar);
  }


  void CnetEditorWindow::createStatusBar()
  {
    statusBar()->showMessage(tr("Ready"));
  }


  void CnetEditorWindow::readSettings()
  {
    QSettings settings("USGS", "cneteditor");

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
    QSettings settings("USGS", "cneteditor");

    // save window position and size
    settings.setValue("pos", pos());
    settings.setValue("size", size());
  }


  bool CnetEditorWindow::okToContinue()
  {
    bool ok = true;

    if (dirty)
    {
      int r = QMessageBox::warning(this, tr("cneteditor"),
          tr("There are unsaved changes!\n\nContinue anyway?"),
          QMessageBox::Yes | QMessageBox::No);

      if (r == QMessageBox::No)
        ok = false;
    }

    return ok;
  }


  void CnetEditorWindow::setDirty()
  {
    setDirty(true);
  }


  void CnetEditorWindow::setSaveAsPvl(int state)
  {
//     cerr << "CnetEditorWindow::setSaveAsPvl... changing from " << saveAsPvl;
    saveAsPvl = (bool) state;
//     cerr << " to " << saveAsPvl << "\n";
  }


  void CnetEditorWindow::open()
  {
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Open a control net file"), ".",
        tr("Control Network files (*.net);;All files (*)"));

    if (!filename.isEmpty())
      load(filename);
  }


  void CnetEditorWindow::setHasFileState(QString filename)
  {
    openAct->setEnabled(false);

    saveAsAct->setEnabled(true);
    closeAct->setEnabled(true);
    setDirty(false);
    *curFile = filename;
    setWindowTitle(filename + "[*] - cneteditor");
  }


  void CnetEditorWindow::setNoFileState()
  {
    openAct->setEnabled(true);

    saveAsAct->setEnabled(false);
    closeAct->setEnabled(false);
    setDirty(false);
    saveAsPvl = false;
    *curFile = "";
    setWindowTitle("cneteditor");
  }


  void CnetEditorWindow::load(QString filename)
  {
    try
    {
      cnet = new ControlNet(filename);
      editorWidget = new CnetEditorWidget(cnet);
      connect(editorWidget, SIGNAL(cnetModified()), this, SLOT(setDirty()));
      setCentralWidget(editorWidget);
      setHasFileState(filename);
      saveAsPvl = !Pvl((iString) filename).HasObject("ProtoBuffer");
    }
    catch (iException e)
    {
      QMessageBox::critical(this, tr("cneteditor"),
          tr("Failed to open the file provided"));
      e.Clear();
    }
  }


  void CnetEditorWindow::save()
  {
    Q_ASSERT(!curFile->isEmpty());

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
        setHasFileState(filename);
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

      setNoFileState();
    }
  }


  void CnetEditorWindow::setDirty(bool state)
  {
    dirty = state;
    saveAct->setEnabled(state);
    setWindowModified(state);
  }

}
