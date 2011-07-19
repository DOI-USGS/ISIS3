#include "QnetTool.h"

#include <sstream>
#include <vector>

#include <QtGui>
#include <QComboBox>

#include "QnetDeletePointDialog.h"
#include "QnetNewMeasureDialog.h"
#include "QnetNewPointDialog.h"
#include "QnetFixedPointDialog.h"
#include "Workspace.h"

#include "Application.h"
#include "Brick.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "ControlMeasure.h"
#include "ControlMeasureLogData.h"
#include "ControlPoint.h"
#include "ControlPointEdit.h"
#include "Distance.h"
#include "Filename.h"
#include "iException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "MdiCubeViewport.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "Pvl.h"
#include "PvlEditDialog.h"
#include "SerialNumber.h"
#include "SpecialPixel.h"
#include "SurfacePoint.h"
#include "ToolPad.h"
#include "UniversalGroundMap.h"

#include "qnet.h"

using namespace Isis;  
using namespace std;

namespace Qisis {

  const int VIEWSIZE = 301;
  using namespace Qnet;
  
  const int CHIPVIEWPORT_WIDTH = 310;


  /**
   * Consructs the Qnet Tool window
   * 
   * @param parent Pointer to the parent widget for the Qnet tool
   * 
   * @internal 
   * @history 2010-06-03 Jeannie Walldren - Initialized pointers to null.
   * 
   */
  QnetTool::QnetTool (QWidget *parent) : Qisis::Tool(parent) {

    p_leftCube = NULL;
    p_rightCube = NULL;
    p_editPoint = NULL;
    p_groundCube = NULL;
    p_groundGmap = NULL;
    p_groundOpen = false;
    p_demCube = NULL;
    p_demOpen = false;
    p_createPoint = NULL;
    p_modifyPoint = NULL;
    p_deletePoint = NULL;
    p_mw = NULL;
    p_pointEditor = NULL;
    p_ptIdValue = NULL;
    p_pointType = NULL;
    p_numMeasures = NULL;
    p_pointLatitude = NULL;
    p_pointLongitude = NULL;
    p_pointRadius = NULL;
    p_lockPoint = NULL;
    p_ignorePoint = NULL;
    p_leftReference = NULL;
    p_leftMeasureType = NULL;
    p_leftSampError = NULL;
    p_leftLineError = NULL;
    p_leftGoodness = NULL;
    p_rightReference = NULL;
    p_rightMeasureType = NULL;
    p_rightSampError = NULL;
    p_rightLineError = NULL;
    p_rightGoodness = NULL;
    p_lockLeftMeasure = NULL;
    p_ignoreLeftMeasure = NULL;
    p_lockRightMeasure = NULL;
    p_ignoreRightMeasure = NULL;
    p_leftCombo = NULL;
    p_rightCombo = NULL;
    p_leftMeasure = NULL;
    p_rightMeasure = NULL;
    p_templateModified = false;
    p_measureTable = NULL;

    createQnetTool(parent);

  }


  QnetTool::~QnetTool () {
#if 0
    if (p_editPoint) {
      delete p_editPoint;
      p_editPoint = NULL;
    }
    if (p_leftMeasure) {
      delete p_leftMeasure;
      p_leftMeasure = NULL;
    }
    if (p_rightMeasure) {
      delete p_rightMeasure;
      p_rightMeasure = NULL;
    }
#endif
    if (p_groundGmap) {
      delete p_groundGmap;
      p_groundGmap = NULL;
    }
  }



  /** 
   * create the main window for editing control points
   *
   * @param parent Pointer to parent QWidget
   * @internal
   *   @history 2008-11-24  Jeannie Walldren - Added "Goodness of Fit" to right
   *                           and left measure info.
   *   @history 2008-11-26  Jeannie Walldren - Added "Number of Measures" to
   *                           QnetTool point information. Moved setWindowTitle()
   *                           command to updateNet() method. Added connection
   *                           between Ignore checkbox toggle() slot and
   *                           ignoreChanged() signal
   *   @history 2008-12-29 Jeannie Walldren - Disabled ground point check box and
   *                          commented out connection between check box and
   *                          setGroundPoint() method.
   *   @history 2008-12-30 Jeannie Walldren - Added connections to toggle
   *                          measures' Ignore check boxes if ignoreLeftChanged()
   *                          and ignoreRightChanged() are emitted. Replaced
   *                          reference to ignoreChanged() with
   *                          ignorePointChanged().
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                          namespace std"
   */
  void QnetTool::createQnetTool(QWidget *parent) {

    p_qnetTool = new QMainWindow(parent);

    createActions();
    createMenus();
    createToolBars();
    
    // create p_pointEditor first since we need to get its templateFilename
    // later
    p_pointEditor = new Qisis::ControlPointEdit(g_controlNetwork, parent);
    connect(this, SIGNAL(newControlNetwork(Isis::ControlNet *)),
        p_pointEditor, SIGNAL(newControlNetwork(Isis::ControlNet *)));
    connect(this,
        SIGNAL(stretchChipViewport(Isis::Stretch *, Qisis::CubeViewport *)),
        p_pointEditor,
        SIGNAL(stretchChipViewport(Isis::Stretch *, Qisis::CubeViewport *)));
    connect(p_pointEditor, SIGNAL(measureSaved()), this, SLOT(measureSaved()));
    connect(this, SIGNAL(measureChanged()),
            p_pointEditor, SLOT(colorizeSaveButton()));
        
    QPushButton * addMeasure = new QPushButton("Add Measure(s) to Point");
    connect(addMeasure, SIGNAL(clicked()), this, SLOT(addMeasure()));

    p_savePoint = new QPushButton ("Save Point");
    p_saveDefaultPalette = p_savePoint->palette();
    connect (p_savePoint,SIGNAL(clicked()),this,SLOT(savePoint()));

    QHBoxLayout * addMeasureLayout = new QHBoxLayout;
    addMeasureLayout->addWidget(addMeasure);
    addMeasureLayout->addWidget(p_savePoint);
//    addMeasureLayout->addStretch();
    
    p_templateFilenameLabel = new QLabel("Template File: " +
        QString::fromStdString(p_pointEditor->templateFilename()));
    
    p_groundFilenameLabel = new QLabel("Ground Source File: ");
    p_radiusFilenameLabel = new QLabel("Radius Source File: ");

    QVBoxLayout * centralLayout = new QVBoxLayout;
    
    centralLayout->addWidget(p_templateFilenameLabel);
    centralLayout->addWidget(p_groundFilenameLabel);
    centralLayout->addWidget(p_radiusFilenameLabel);
    centralLayout->addWidget(createTopSplitter());
    centralLayout->addStretch();
    centralLayout->addWidget(p_pointEditor);
    centralLayout->addLayout(addMeasureLayout);
    
    QWidget * centralWidget = new QWidget;
    centralWidget->setLayout(centralLayout);
    p_qnetTool->setCentralWidget(centralWidget);
    
    connect(this, SIGNAL(editPointChanged(QString)),
            this, SLOT(paintAllViewports(QString)));
  }
  
  
  //! creates everything above the ControlPointEdit
  QSplitter * QnetTool::createTopSplitter() {
  
    QHBoxLayout * measureLayout = new QHBoxLayout;
    measureLayout->addWidget(createLeftMeasureGroupBox());
    measureLayout->addWidget(createRightMeasureGroupBox());
    
    QVBoxLayout * groupBoxesLayout = new QVBoxLayout;
    groupBoxesLayout->addWidget(createControlPointGroupBox());
    groupBoxesLayout->addStretch();
    groupBoxesLayout->addLayout(measureLayout);
    
    QWidget * groupBoxesWidget = new QWidget;
    groupBoxesWidget->setLayout(groupBoxesLayout);
    
    createTemplateEditorWidget();
    
    QSplitter * topSplitter = new QSplitter;
    topSplitter->addWidget(groupBoxesWidget);
    topSplitter->addWidget(p_templateEditorWidget);
    topSplitter->setStretchFactor(0, 4);
    topSplitter->setStretchFactor(1, 3);
    
    p_templateEditorWidget->hide();
    
    return topSplitter;
  }
  
  
  //! @returns The groupbox labeled "Control Point"
  QGroupBox * QnetTool::createControlPointGroupBox() {
  
    // create left vertical layout
    p_ptIdValue = new QLabel;
    p_pointType = new QComboBox;
    for (int i=0; i<ControlPoint::PointTypeCount; i++) {
      p_pointType->insertItem(i, ControlPoint::PointTypeToString(
                              (ControlPoint::PointType) i));
    }
    QHBoxLayout *pointTypeLayout = new QHBoxLayout;
    QLabel *pointTypeLabel = new QLabel("PointType:");
    pointTypeLayout->addWidget(pointTypeLabel);
    pointTypeLayout->addWidget(p_pointType);
    connect(p_pointType, SIGNAL(activated(int)),
            this, SLOT(setPointType(int)));
    p_numMeasures = new QLabel;
    p_pointAprioriLatitude = new QLabel;
    p_pointAprioriLongitude = new QLabel;
    p_pointAprioriRadius = new QLabel;
    p_pointAprioriLatitudeSigma = new QLabel;
    p_pointAprioriLongitudeSigma = new QLabel;
    p_pointAprioriRadiusSigma = new QLabel;
    QVBoxLayout * leftLayout = new QVBoxLayout;
    leftLayout->addWidget(p_ptIdValue);
    leftLayout->addLayout(pointTypeLayout);
    leftLayout->addWidget(p_numMeasures);
    leftLayout->addWidget(p_pointAprioriLatitude);
    leftLayout->addWidget(p_pointAprioriLongitude);
    leftLayout->addWidget(p_pointAprioriRadius);
    leftLayout->addWidget(p_pointAprioriLatitudeSigma);
    leftLayout->addWidget(p_pointAprioriLongitudeSigma);
    leftLayout->addWidget(p_pointAprioriRadiusSigma);
    
    // create right vertical layout's top layout
    p_lockPoint = new QCheckBox("Edit Lock Point");
    connect(p_lockPoint, SIGNAL(clicked(bool)), this, SLOT(setLockPoint(bool)));
    p_ignorePoint = new QCheckBox("Ignore Point");
    connect(p_ignorePoint, SIGNAL(clicked(bool)),
            this, SLOT(setIgnorePoint(bool)));
    connect(this, SIGNAL(ignorePointChanged()), p_ignorePoint, SLOT(toggle()));
    p_pointLatitude = new QLabel;
    p_pointLongitude = new QLabel;
    p_pointRadius = new QLabel;

    QVBoxLayout * rightLayout = new QVBoxLayout;
    rightLayout->addWidget(p_lockPoint);
    rightLayout->addWidget(p_ignorePoint);
    rightLayout->addWidget(p_pointLatitude);
    rightLayout->addWidget(p_pointLongitude);
    rightLayout->addWidget(p_pointRadius);
    
    
    QHBoxLayout * mainLayout = new QHBoxLayout;
    mainLayout->addLayout(leftLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(rightLayout);
    
    // create the groupbox
    QGroupBox * groupBox = new QGroupBox("Control Point");
    groupBox->setLayout(mainLayout);
    
    return groupBox;
  }
  
  
  //! @returns The groupbox labeled "Left Measure"
  QGroupBox * QnetTool::createLeftMeasureGroupBox() {
  
    p_leftCombo = new QComboBox;
    p_leftCombo->view()->installEventFilter(this);
    connect(p_leftCombo, SIGNAL(activated(int)),
            this, SLOT(selectLeftMeasure(int)));
    p_lockLeftMeasure = new QCheckBox("Edit Lock Measure");
    connect(p_lockLeftMeasure, SIGNAL(clicked(bool)),
            this, SLOT(setLockLeftMeasure(bool)));
    p_ignoreLeftMeasure = new QCheckBox("Ignore Measure");
    connect(p_ignoreLeftMeasure, SIGNAL(clicked(bool)),
            this, SLOT(setIgnoreLeftMeasure(bool)));
    connect(this, SIGNAL(ignoreLeftChanged()),
            p_ignoreLeftMeasure, SLOT(toggle()));
    p_leftReference = new QLabel();
    p_leftMeasureType = new QLabel();
    p_leftSampError = new QLabel();
    p_leftLineError = new QLabel();
    p_leftGoodness = new QLabel();
    QVBoxLayout * leftLayout = new QVBoxLayout;
    leftLayout->addWidget(p_leftCombo);
    leftLayout->addWidget(p_lockLeftMeasure);
    leftLayout->addWidget(p_ignoreLeftMeasure);
    leftLayout->addWidget(p_leftReference);
    leftLayout->addWidget(p_leftMeasureType);
    leftLayout->addWidget(p_leftSampError);
    leftLayout->addWidget(p_leftLineError);
    leftLayout->addWidget(p_leftGoodness);

    QGroupBox * leftGroupBox = new QGroupBox("Left Measure");
    leftGroupBox->setLayout(leftLayout);
    
    return leftGroupBox;
  }
  
  
  //! @returns The groupbox labeled "Right Measure"
  QGroupBox * QnetTool::createRightMeasureGroupBox() {
  
    // create widgets for the right groupbox
    p_rightCombo = new QComboBox;
    p_rightCombo->view()->installEventFilter(this);
    connect(p_rightCombo, SIGNAL(activated(int)),
            this, SLOT(selectRightMeasure(int)));
    p_lockRightMeasure = new QCheckBox("Edit Lock Measure");
    connect(p_lockRightMeasure, SIGNAL(clicked(bool)),
            this, SLOT(setLockRightMeasure(bool)));
    p_ignoreRightMeasure = new QCheckBox("Ignore Measure");
    connect(p_ignoreRightMeasure, SIGNAL(clicked(bool)),
            this, SLOT(setIgnoreRightMeasure(bool)));
    connect(this, SIGNAL(ignoreRightChanged()),
            p_ignoreRightMeasure, SLOT(toggle()));
    p_rightReference = new QLabel();
    p_rightMeasureType = new QLabel();
    p_rightSampError = new QLabel();
    p_rightLineError = new QLabel();
    p_rightGoodness = new QLabel();
    
    // create right groupbox
    QVBoxLayout * rightLayout = new QVBoxLayout;
    rightLayout->addWidget(p_rightCombo);
    rightLayout->addWidget(p_lockRightMeasure);
    rightLayout->addWidget(p_ignoreRightMeasure);
    rightLayout->addWidget(p_rightReference);
    rightLayout->addWidget(p_rightMeasureType);
    rightLayout->addWidget(p_rightSampError);
    rightLayout->addWidget(p_rightLineError);
    rightLayout->addWidget(p_rightGoodness);
    
    QGroupBox * rightGroupBox = new QGroupBox("Right Measure");
    rightGroupBox->setLayout(rightLayout);
    
    return rightGroupBox;
  }
  
  
  //! Creates the Widget which contains the template editor and its toolbar
  void QnetTool::createTemplateEditorWidget() {

    QToolBar *toolBar = new QToolBar("Template Editor ToolBar");
    toolBar->addAction(p_openTemplateFile);
    toolBar->addSeparator();
    toolBar->addAction(p_saveTemplateFile);
    toolBar->addAction(p_saveTemplateFileAs);

    p_templateEditor = new QTextEdit;
    connect(p_templateEditor, SIGNAL(textChanged()), this,
        SLOT(setTemplateModified()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(toolBar);
    mainLayout->addWidget(p_templateEditor);

    p_templateEditorWidget = new QWidget;
    p_templateEditorWidget->setLayout(mainLayout);
  }



  void QnetTool::createActions() {
    p_openGround = new QAction(p_qnetTool);
    p_openGround->setText("Open &Ground Source");
    p_openGround->setStatusTip("Open a ground source for choosing fixed points");
    QString whatsThis =
      "<b>Function:</b> Open and display a ground source for choosing fixed points."
      "This can be level1, level2 or dem cube.";
    p_openGround->setWhatsThis(whatsThis);
    connect (p_openGround,SIGNAL(activated()),this,SLOT(openGround()));

    p_openDem = new QAction(p_qnetTool);
    p_openDem->setText("Open &Radius Source");
    whatsThis =
      "<b>Function:</b> Open a DEM for determining the radius when "
      "choosing fixed points.  This is not the file that will be displayed "
      "to be used for visually picking points.  This is strictly used to "
      "determine the radius value.";
    p_openDem->setWhatsThis(whatsThis);
    connect (p_openDem,SIGNAL(activated()),this,SLOT(openDem()));

    p_saveNet = new QAction(QIcon(":saveAs"), "Save Control Network &As...",
        p_qnetTool);
    p_saveNet->setStatusTip("Save current control network to chosen file");
    whatsThis = "<b>Function:</b> Saves the current <i>"
        "control network</i> under chosen filename";
    p_saveNet->setWhatsThis(whatsThis);
    connect(p_saveNet, SIGNAL(activated()), this, SLOT(saveNet()));

    p_closeQnetTool = new QAction(QIcon(":close"), "&Close", p_qnetTool);
    p_closeQnetTool->setStatusTip("Close this window");
    p_closeQnetTool->setShortcut(Qt::ALT + Qt::Key_F4);
    whatsThis = "<b>Function:</b> Closes the Qnet Tool window for this point "
        "<p><b>Shortcut:</b> Alt+F4 </p>";
    p_closeQnetTool->setWhatsThis(whatsThis);
    connect(p_closeQnetTool, SIGNAL(activated()), p_qnetTool, SLOT(close()));

    p_showHideTemplateEditor = new QAction(QIcon(":view_edit"),
        "&View/edit registration template", p_qnetTool);
    p_showHideTemplateEditor->setCheckable(true);
    p_showHideTemplateEditor->setStatusTip("View and/or edit the registration template");
    whatsThis = "<b>Function:</b> Displays the curent registration template.  "
       "The user may edit and save changes under a chosen filename.";
    p_showHideTemplateEditor->setWhatsThis(whatsThis);
    connect(p_showHideTemplateEditor, SIGNAL(activated()), this,
        SLOT(showHideTemplateEditor()));

    p_saveChips = new QAction(QIcon(":window_new"), "Save registration chips",
        p_qnetTool);
    p_saveChips->setStatusTip("Save registration chips");
    whatsThis = "<b>Function:</b> Save registration chips to file.  "
       "Each chip: pattern, search, fit will be saved to a separate file.";
    p_saveChips->setWhatsThis(whatsThis);
    connect(p_saveChips, SIGNAL(activated()), this, SLOT(saveChips()));

    p_openTemplateFile = new QAction(QIcon(":open"), "&Open registration "
        "template", p_qnetTool);
    p_openTemplateFile->setStatusTip("Set registration template");
    whatsThis = "<b>Function:</b> Allows user to select a new file to set as "
        "the registration template";
    p_openTemplateFile->setWhatsThis(whatsThis);
    connect(p_openTemplateFile, SIGNAL(activated()), this, SLOT(openTemplateFile()));

    p_saveTemplateFile = new QAction(QIcon(":save"), "&Save template file",
        p_qnetTool);
    p_saveTemplateFile->setStatusTip("Save the template file");
    p_saveTemplateFile->setWhatsThis("Save the registration template file");
    connect(p_saveTemplateFile, SIGNAL(triggered()), this,
        SLOT(saveTemplateFile()));

    p_saveTemplateFileAs = new QAction(QIcon(":saveAs"), "&Save template as...",
        p_qnetTool);
    p_saveTemplateFileAs->setStatusTip("Save the template file");
    p_saveTemplateFileAs->setWhatsThis("Save the registration template file");
    connect(p_saveTemplateFileAs, SIGNAL(triggered()), this,
        SLOT(saveTemplateFileAs()));
  }



  /** 
   * Customize dropdown menus below title bar. 
   *  
   * @internal
   *   @history 2008-11-18 Jeannie Walldren - Added "Close" action to the file
   *                          menu on the qnet tool window.
   *   @history 2008-12-10 Jeannie Walldren - Added "What's this?" function to
   *                          qnet tool actions.
   */
  void QnetTool::createMenus () {

    QMenu *fileMenu = p_qnetTool->menuBar()->addMenu("&File");
    fileMenu->addAction(p_openGround);
    fileMenu->addAction(p_openDem);
    fileMenu->addAction(p_saveNet);
    fileMenu->addAction(p_closeQnetTool);

    QMenu * regMenu = p_qnetTool->menuBar()->addMenu("&Registration");
    regMenu->addAction(p_openTemplateFile);
    regMenu->addAction(p_showHideTemplateEditor);
    regMenu->addAction(p_saveChips);
  }


  void QnetTool::createToolBars() {

    QToolBar * toolBar = new QToolBar;
    toolBar->setFloatable(false);
    toolBar->addAction(p_saveNet);
    toolBar->addSeparator();
    toolBar->addAction(p_showHideTemplateEditor);
    toolBar->addAction(p_saveChips);

    p_qnetTool->addToolBar(Qt::TopToolBarArea, toolBar);
  }



  /** 
   * This method is connected with the measureSaved() signal from 
   * ControlPointEdit. 
   *  
   * @internal 
   *   @history 2008-11-26 Jeannie Walldren - Added message box to warn the user
   *                          that they are saving an "Ignore" point and ask
   *                          whether they would like to set Ignore=false. This
   *                          emits an ignoreChanged() signal so the "Ignore" box
   *                          in the window is unchecked.
   *   @history 2008-12-30 Jeannie Walldren - Modified to set measures in
   *                          viewports to Ignore=False if when saving, the user
   *                          chooses to set a point's Ignore=False. Replaced
   *                          reference to ignoreChanged() with
   *                          ignorePointChanged().
   *   @history 2008-12-31 Jeannie Walldren - Added question box to ask user
   *                          whether the current reference measure should be
   *                          replaced with the measure in the left viewport.
   *   @history 2010-01-27 Jeannie Walldren - Added question box to warn the user
   *                          that they are saving an "Ignore" measure and ask
   *                          whether they would like to set Ignore=False. This
   *                          emits an ignoreRightChanged() signal so the "Ignore"
   *                          box in the window is unchecked. Modified Ignore
   *                          Point message for clarity.
   *   @history 2010-11-19 Tracie Sucharski - Renamed from pointSaved.
   *   @history 2011-03-03 Tracie Sucharski - Do not save left measure unless
   *                          the ignore flag was changed, that is the only
   *                          change allowed on the left measure.
   *   @history 2011-04-20 Tracie Sucharski - If left measure equals right
   *                          measure, copy right into left.  Also if EditLock
   *                          true and user does not want to change, then
   *                          do not save measure.  Remove signals
   *                          EditPointChanged and netChanged, since these
   *                          should only happen when the point is saved.
   *   @history 2011-07-01 Tracie Sucharski - Fixed bug where the edit measure
   *                          EditLocked=True, but the original measure was
   *                          False, and we woouldn't allow the measure to be
   *                          saved.
   */
  void QnetTool::measureSaved() {

    // Read original measures from the network.
    ControlMeasure *leftMeasure =
                p_editPoint->GetMeasure(p_leftMeasure->GetCubeSerialNumber());
    ControlMeasure *rightMeasure =
                p_editPoint->GetMeasure(p_rightMeasure->GetCubeSerialNumber());

    //  Check original measure from network, so that a measure that has just
    //  had EditLocked set to True can be saved.
    if (IsMeasureLocked(rightMeasure->GetCubeSerialNumber()) &&
        IsMeasureLocked(p_rightMeasure->GetCubeSerialNumber())) {
      QString message = "You are saving changes to a measure that is locked ";
      message += "for editing.  Do you want to set EditLock = False for this ";
      message += "measure?";
      switch (QMessageBox::question((QWidget *)parent(),
                "Qnet Tool Save Measure", message,
                "&Yes", "&No", 0, 0)) {
        // Yes was clicked or Enter was pressed, set EditLock=false for the right
        // measure
        case 0: 
          p_rightMeasure->SetEditLock(false);
          p_lockRightMeasure->setChecked(false);
        // No was clicked, keep EditLock=true and do NOT save measure
        case 1: 
          return;
      }
    }

    if (p_editPoint->IsIgnored()) {
      QString message = "You are saving changes to a measure on an ignored ";
      message += "point.  Do you want to set Ignore = False on the point and ";
      message += "both measures?";
      switch (QMessageBox::question((QWidget *)parent(),
                "Qnet Tool Save Measure", message,
                "&Yes", "&No", 0, 0)) {
        // Yes was clicked or Enter was pressed, set Ignore=false for the point
        // and measures and save point
        case 0: 
          p_editPoint->SetIgnored(false);
          emit ignorePointChanged();
          if (p_leftMeasure->IsIgnored()) {
            p_leftMeasure->SetIgnored(false);
            emit ignoreLeftChanged();
          }
          if (p_rightMeasure->IsIgnored()) {
            p_rightMeasure->SetIgnored(false);
            emit ignoreRightChanged();
          }
        // No was clicked, keep Ignore=true and save measure
        case 1: 
          break;

      }
    }
    if (rightMeasure->IsIgnored()) {
      QString message = "You are saving changes to an ignored measure.  ";
      message += "Do you want to set Ignore = False on the right measure?";
      switch(QMessageBox::question((QWidget *)parent(),
                                "Qnet Tool Save Measure", message,
                                "&Yes", "&No", 0, 0)){
        // Yes was clicked, set Ignore=false for the right measure and save point
        case 0: 
            p_rightMeasure->SetIgnored(false);
            emit ignoreRightChanged();
        // No was clicked, keep Ignore=true and save point
        case 1: 
          break;
      }
    }
    // Check if ControlPoint has reference measure, if reference Measure is
    // not the same measure that is on the left chip viewport, set left
    // measure as reference.
    if (p_editPoint->IsReferenceExplicit()) {
      ControlMeasure *refMeasure = p_editPoint->GetRefMeasure();
      // Reference Measure not on left.  Ask user if they want to change
      // the reference measure, but only if it is not the ground source on the left
      if (refMeasure->GetCubeSerialNumber() != 
        p_leftMeasure->GetCubeSerialNumber() &&
        p_leftMeasure->GetCubeSerialNumber() != p_groundSN) {
        QString message = "This point already contains a reference measure.  ";
        message += "Would you like to replace it with the measure on the left?";
        switch(QMessageBox::question((QWidget *)parent(),
                                  "Qnet Tool Save Measure", message,
                                  "&Yes", "&No", 0, 0)){
          case 0: // Yes was clicked or Enter was pressed, replace reference
            p_editPoint->SetRefMeasure(p_leftMeasure->GetCubeSerialNumber());
            // ??? Need to set rest of measures to Candiate and add more warning. ???//
          case 1: // No was clicked, keep original reference
            break;
        }
      }
      // If the right measure is the reference, make sure they really want
      // to move the reference.
      if (refMeasure->GetCubeSerialNumber() == p_rightMeasure->GetCubeSerialNumber()) {
        QString message = "You are making a change to the reference measure.  You ";
        message += "may need to move all of the other measures to match the new ";
        message += " coordinate of the reference measure.  Do you really want to ";
        message += " change the reference measure? ";
        switch(QMessageBox::question((QWidget *)parent(),
                                  "Qnet Tool Save Measure", message,
                                  "&Yes", "&No", 0, 0)){
          case 0: // Yes was clicked or Enter was pressed, save the measure
            break;
          case 1: // No was clicked, keep original reference, return without saving
            loadPoint();
            return;
        }
      }
    }
    else {
      p_editPoint->SetRefMeasure(p_leftMeasure->GetCubeSerialNumber());
    }

    // If this is a fixed or constrained point, if either measure (left or
    // right) is the ground source, update the lat,lon,radius.
    if (p_editPoint->GetType() != ControlPoint::Free &&
        (p_leftMeasure->GetCubeSerialNumber() == p_groundSN ||
         p_rightMeasure->GetCubeSerialNumber() == p_groundSN)) {

      //  Point is editLocked.  Print warning if the point already exists in the
      //  network.  If it is a new point with the editLock keyword set, do not
      //  print warning.
      if (p_editPoint->IsEditLocked() && g_controlNetwork->ContainsPoint(
                                          p_editPoint->GetId())) {
        QString message = "This point is locked for editing.  Do want to set ";
        message += "EditLock = False?";
        switch (QMessageBox::question((QWidget *)parent(),
                  "Qnet Tool Save Measure", message,
                  "&Yes", "&No", 0, 0)) {
          // Yes was clicked or Enter was pressed, set EditLock=false for the
          // point and save point
          case 0: 
            p_editPoint->SetEditLock(false);
            p_lockPoint->setChecked(false);
          // No was clicked, keep EditLock=true and return
          case 1: 
            loadPoint();
            return;

        }
      }
      //  TODO:  If groundSource file opened does not match the SurfacePoint Source
      //  file, print warning.

      //  Find temporary ground measure

      //  Do not know why this code was necessary. Commented out on 7/18/2011
      //  TODO:  If not needed, delete!
//    ControlMeasure *groundMeasure = p_editPoint->GetMeasure(p_groundSN);
//    if (!p_groundGmap->SetImage(groundMeasure->GetSample(),
//                                groundMeasure->GetLine())) {
//      //  TODO  error??? This should never happen
//    }
      
      // Is ground on right or left?  Use ground measure to update apriori
      //  surface point.
      if (p_leftMeasure->GetCubeSerialNumber() == p_groundSN) {
        if (!p_groundGmap->SetImage(p_leftMeasure->GetSample(),
                                    p_leftMeasure->GetLine())) {
          // TODO :  should never happen, either add error check or
         //        get rid of
        }
      }
      else {
        if (!p_groundGmap->SetImage(p_rightMeasure->GetSample(),
                                    p_rightMeasure->GetLine())) {
          // TODO :  should never happen, either add error check or
           //      get rid of
        }
      }

      double lat = p_groundGmap->UniversalLatitude();
      double lon = p_groundGmap->UniversalLongitude();
      double radius;
      //  Update radius
      if (p_demOpen) {
        Brick pixel(1,1,1,p_demCube->getPixelType());
        int intSamp = (int) (p_rightMeasure->GetSample() + 0.5);
        int intLine = (int) (p_rightMeasure->GetLine() + 0.5);
        pixel.SetBasePosition(intSamp,intLine,1);
        p_demCube->read(pixel);
        radius = pixel[0];
      }
      else {
        radius = p_groundGmap->Projection()->LocalRadius();
      }
      try {
        p_editPoint->SetAprioriSurfacePoint(SurfacePoint(
                                            Latitude(lat, Angle::Degrees),
                                            Longitude(lon, Angle::Degrees),
                                            Distance(radius, Distance::Meters)));
      }
      catch (iException &e) {
        QString message = "Unable to set Apriori Surface Point.\n";
        message += "Latitude = " + QString::number(lat);
        message += "  Longitude = " + QString::number(lon);
        message += "  Radius = " + QString::number(radius) + "\n";
        message += e.Errors().c_str();
        QMessageBox::critical((QWidget *)parent(),"Error",message);
        e.Clear();
      }
      p_editPoint->SetAprioriSurfacePointSource(p_groundSurfacePointSource);
      p_editPoint->SetAprioriSurfacePointSourceFile(p_groundSourceFile);
      p_editPoint->SetAprioriRadiusSource(p_groundRadiusSource);
      p_editPoint->SetAprioriRadiusSourceFile(p_radiusSourceFile);

      updateSurfacePointInfo ();
    }

    // Save the right measure and left (if ignore or edit lock flag changed) to
    // the editPoint The Ignore flag is the only thing that can change on the left
    // measure. First find measure, then replace with the edit Tool measure
    p_rightMeasure->SetChooserName(Application::UserName());
    *rightMeasure = *p_rightMeasure;

    //  The ignore flag on the left measure has changed from the net file
    if (p_leftMeasure->IsIgnored() != leftMeasure->IsIgnored() ||
        IsMeasureLocked(p_leftMeasure->GetCubeSerialNumber()) !=
        IsMeasureLocked(leftMeasure->GetCubeSerialNumber())) {
      p_leftMeasure->SetChooserName(Application::UserName());
      *leftMeasure = *p_leftMeasure;
    }

    // If left measure == right measure, update left
    if (p_leftMeasure->GetCubeSerialNumber() == p_rightMeasure->GetCubeSerialNumber()) {
      *p_leftMeasure = *p_rightMeasure;
    }

    //  Change Save Point button text to red
    colorizeSaveButton();

    // Update measure info
    updateLeftMeasureInfo();
    updateRightMeasureInfo();
    loadMeasureTable();

  }


  /**
   * Save edit point to the Control Network.  Up to this point the point is 
   * simply a copy and does not exist in the network. 
   *  
   * @author 2010-11-19 Tracie Sucharski 
   *  
   * @internal 
   * @history 2011-04-20 Tracie Sucharski - If EditLock set, prompt for changing 
   *                        and do not save point if editLock not changed.
   * @history 2011-07-05 Tracie Sucharski - Move point EditLock error checking 
   *                        to individual point parameter setting methods, ie.
   *                        SetPointType, SetIgnorePoint.
   *      
   */
  void QnetTool::savePoint () {


    //  Make a copy of edit point for updating the control net since the edit
    //  point is still loaded in the point editor.
    ControlPoint *updatePoint = new ControlPoint;
    *updatePoint = *p_editPoint;

    //  If this is a fixed or constrained point, see if there is a temporary
    //  measure holding the coordinate information from the ground source. 
    //  If so, delete this measure before saving point.  Clear out the
    //  fixed Measure variable (memory deleted in ControlPoint::Delete).
    if (updatePoint->GetType() != ControlPoint::Free) {
      for (int i=0; i<updatePoint->GetNumMeasures(); i++) {
        if ((*updatePoint)[i]->GetCubeSerialNumber() == p_groundSN) {
          updatePoint->Delete(i);
          break;
        }
      }
    }

    //  If edit point exists in the network, save the updated point.  If it
    //  does not exist, add it.
    if (g_controlNetwork->ContainsPoint(updatePoint->GetId())) {
      ControlPoint *p;
      p = g_controlNetwork->GetPoint(QString(updatePoint->GetId()));
      *p = *updatePoint;
    }
    else {
      g_controlNetwork->AddPoint(updatePoint);
    }

    //  Change Save Measure button text back to default
    p_savePoint->setPalette(p_saveDefaultPalette);

    loadPoint();
    // emit signal so the nav tool refreshes the list
    emit refreshNavList();
    // emit signal so the nav tool can update edit point
    emit editPointChanged(p_editPoint->GetId());
    // emit a signal to alert user to save when exiting 
    emit netChanged();
  }




  /**
   * Set the point type
   * @param pointType int Index from point type combo box 
   *  
   * @author 2011-07-05 Tracie Sucharski 
   *  
   * @internal 
   */
  void QnetTool::setPointType (int pointType) {
    if (p_editPoint == NULL) return;

    ControlPoint::Status status = 
         p_editPoint->SetType((ControlPoint::PointType) pointType);
    if (status == ControlPoint::PointLocked) {
      p_pointType->setCurrentIndex((int) p_editPoint->GetType());
      QString message = "Unable to change the point type.  Set EditLock ";
      message += " to False.";
      QMessageBox::critical((QWidget *)parent(), "Error", message);
      return;
    }

    //  If ground loaded, readd temporary ground measure to the point
    if (p_groundOpen) loadPoint();


    colorizeSaveButton();
  }



  /**
   * Set point's "EditLock" keyword to the value of the input parameter. 
   * @param ignore Boolean value that determines the EditLock value for this 
   *               point.
   *  
   * @author 2011-03-07 Tracie Sucharski 
   *  
   * @internal 
   */
  void QnetTool::setLockPoint (bool lock) {
    if (p_editPoint == NULL) return;

    p_editPoint->SetEditLock(lock);
    colorizeSaveButton();
  }



  /**
   * Set point's "Ignore" keyword to the value of the input
   * parameter.
   * @param ignore Boolean value that determines the Ignore value for this point. 
   *  
   * @internal 
   * @history 2010-12-15 Tracie Sucharski - Remove netChanged, the point is 
   *                        not changed in the net unless "Save Point" is
   *                        selected.
   */
  void QnetTool::setIgnorePoint (bool ignore) {
    if (p_editPoint == NULL) return;

    ControlPoint::Status status = p_editPoint->SetIgnored(ignore);
    if (status == ControlPoint::PointLocked) {
      p_ignorePoint->setChecked(p_editPoint->IsIgnored());
      QString message = "Unable to change Ignored on point.  Set EditLock ";
      message += " to False.";
      QMessageBox::critical((QWidget *)parent(), "Error", message);
      return;
    }
    colorizeSaveButton();
  }




  /**
   * Set the "EditLock" keyword of the measure shown in the left viewport to the
   * value of the input parameter. 
   * 
   * @param ignore Boolean value that determines the EditLock value for the left
   *               measure.
   *  
   * @author 2011-03-07 Tracie Sucharski 
   *  
   * @internal 
   * @history 2011-06-27 Tracie Sucharski - emit signal indicating a measure 
   *                        parameter has changed. 
   */
  void QnetTool::setLockLeftMeasure (bool lock) {
    if (p_leftMeasure != NULL) p_leftMeasure->SetEditLock(lock);

    //  If the right chip is the same as the left chip , update the right editLock
    //  box.
    if (p_rightMeasure != NULL) {
      if (p_rightMeasure->GetCubeSerialNumber() == p_leftMeasure->GetCubeSerialNumber()) {
        p_rightMeasure->SetEditLock(lock);
        p_lockRightMeasure->setChecked(lock);
      }
    }
    emit measureChanged();
  }


  /**
   * Set the "Ignore" keyword of the measure shown in the left
   * viewport to the value of the input parameter.
   * 
   * @param ignore Boolean value that determines the Ignore value for the left 
   *               measure.
   * @internal
   * @history 2010-01-27 Jeannie Walldren - Fixed bug that resulted in segfault.
   *                          Moved the check whether p_rightMeasure is null
   *                          before the check whether p_rightMeasure equals
   *                          p_leftMeasure.
   * @history 2010-12-15 Tracie Sucharski - Remove netChanged, the point is 
   *                        not changed in the net unless "Save Point" is
   *                        selected.
   * @history 2011-06-27 Tracie Sucharski - emit signal indicating a measure 
   *                        parameter has changed. 
   */
  void QnetTool::setIgnoreLeftMeasure (bool ignore) {
    if (p_leftMeasure != NULL) p_leftMeasure->SetIgnored(ignore);

    //  If the right chip is the same as the left chip , update the right
    //  ignore box.
    if (p_rightMeasure != NULL) {
      if (p_rightMeasure->GetCubeSerialNumber() == p_leftMeasure->GetCubeSerialNumber()) {
        p_rightMeasure->SetIgnored(ignore);
        p_ignoreRightMeasure->setChecked(ignore);
      }
    }
    emit measureChanged();
  }


  /**
   * Set the "EditLock" keyword of the measure shown in the right viewport to 
   * the value of the input parameter. 
   * 
   * @param ignore Boolean value that determines the EditLock value for the 
   *               right measure.
   *  
   * @author 2011-03-07 Tracie Sucharski 
   *  
   * @internal
   * @history 2011-06-27 Tracie Sucharski - emit signal indicating a measure 
   *                        parameter has changed. 
   */
  void QnetTool::setLockRightMeasure (bool lock) {
    if (p_rightMeasure != NULL) p_rightMeasure->SetEditLock(lock);

    //  If the left chip is the same as the right chip , update the left editLock box.
    if (p_leftMeasure != NULL) {
      if (p_leftMeasure->GetCubeSerialNumber() == p_rightMeasure->GetCubeSerialNumber()) {
        p_leftMeasure->SetEditLock(lock);
        p_lockLeftMeasure->setChecked(lock);
      }
    }
    emit measureChanged();
  }


  /**
   * Set the "Ignore" keyword of the measure shown in the right
   * viewport to the value of the input parameter. 
   * 
   * @param ignore Boolean value that determines the Ignore value for the right 
   *               measure.
   * @internal
   * @history 2010-01-27 Jeannie Walldren - Fixed bug that resulted in segfault.
   *                          Moved the check whether p_leftMeasure is null before
   *                          the check whether p_rightMeasure equals
   *                          p_leftMeasure.
   * @history 2010-12-15 Tracie Sucharski - Remove netChanged, the point is 
   *                        not changed in the net unless "Save Point" is
   *                        selected.
   * @history 2011-06-27 Tracie Sucharski - emit signal indicating a measure 
   *                        parameter has changed. 
   */
  void QnetTool::setIgnoreRightMeasure (bool ignore) {
    if (p_rightMeasure != NULL) p_rightMeasure->SetIgnored(ignore);

    //  If the right chip is the same as the left chip , update the right
    //  ignore blox.
    if (p_leftMeasure != NULL) {
      if (p_rightMeasure->GetCubeSerialNumber() == p_leftMeasure->GetCubeSerialNumber()) {
        p_leftMeasure->SetIgnored(ignore);
        p_ignoreLeftMeasure->setChecked(ignore);
      }
    }
    emit measureChanged();
  }


  /**
   * Signal to save the control net.
   */
  void QnetTool::saveNet () {
    emit qnetToolSave();
  }




  /**
   * 
   */
  void QnetTool::updateList() {

    //p_pointEditor->setSerialList (*g_serialNumberList);

  }


  /**
   * Updates the Control Network displayed in the Qnet Tool title 
   * bar. This slot is connected to QnetFileTool's 
   * controlNetworkUpdated(QString cNetFilename) signal. 
   * 
   * @param cNetFilename Filename of the most recently selected 
   *                     control network.
   * @see QnetFileTool 
   * @internal 
   *   @history Tracie Sucharski - Original version
   *   @history 2008-11-26 Jeannie Walldren - Added cNetFilename input parameter
   *                          in order to show the file path in the window's title
   *                          bar.
   */
  void QnetTool::updateNet(QString cNetFilename) {
    p_qnetTool->setWindowTitle("Qnet Tool - Control Network File: " + cNetFilename);
    //p_pointEditor->setControlNet(*g_controlNetwork);

  }



  /** 
   * Adds the Tie tool action to the tool pad.  When the Tie tool is selected, the
   * Navigation Tool will automatically open. 
   *  
   * @param pad Tool pad
   * @return @b QAction* Pointer to Tie tool action 
   *  
   * @internal
   *   @history 2010-07-01 Jeannie Walldren - Added connection between qnet's
   *                          TieTool button and the showNavWindow() method
   */
  QAction *QnetTool::toolPadAction(ToolPad *pad) {
    QAction *action = new QAction(pad);
    action->setIcon(QPixmap(toolIconDir()+"/stock_draw-connector-with-arrows.png"));
    action->setToolTip("Control Point Editor (T)");
    action->setShortcut(Qt::Key_T);
    QObject::connect(action,SIGNAL(triggered(bool)),this,SLOT(showNavWindow(bool)));
    return action;
  }



  /**
   * Handle mouse events on CubeViewport
   * 
   * @param p[in]   (QPoint)            Point under cursor in cubeviewport
   * @param s[in]   (Qt::MouseButton)   Which mouse button was pressed
   * 
   * @internal
   * @history  2007-06-12 Tracie Sucharski - Swapped left and right mouse
   *                           button actions.
   * @history  2009-06-08 Tracie Sucharski - Add error checking for editing 
   *                           or deleting points when no point exists.
   * @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                          namespace std"
   * @history 2010-11-19 Tracie Sucharski - Make a copy of the point to be 
   *                          edited or added.  Do not add to control
   *                          network until user selects
   *                          "Save Point To Control Network".
   *  
   */
  void QnetTool::mouseButtonRelease(QPoint p, Qt::MouseButton s) {
    MdiCubeViewport *cvp = cubeViewport();
    if (cvp  == NULL) return;
    if (cvp->cursorInside()) QPoint p = cvp->cursorPosition();

    iString file = cvp->cube()->getFilename();
    iString sn = g_serialNumberList->SerialNumber(file);

    double samp,line;
    cvp->viewportToCube(p.x(),p.y(),samp,line);


    if (s == Qt::LeftButton) {
      if (p_groundOpen && file == p_groundCube->getFilename()) {
        QString message = "Cannot select point for editing on ground source.  Select ";
        message += "point using un-projected images or the Navigator Window.";
        QMessageBox::critical((QWidget *)parent(),"Error",message);
        return;
      }
      //  Find closest control point in network
      iString sn = g_serialNumberList->SerialNumber(file);
      ControlPoint *point = g_controlNetwork->FindClosest(sn, samp, line);

      if (point == NULL) {
        QString message = "No points exist for editing.  Create points ";
        message += "using the right mouse button.";
        QMessageBox::warning((QWidget *)parent(),"Warning",message);
        return;
      }
      p_leftFile = file;

      modifyPoint(point);
    }
    else if (s == Qt::MidButton) {
      if (p_groundOpen && file == p_groundCube->getFilename()) {
        QString message = "Cannot select point for deleting on ground source.  Select ";
        message += "point using un-projected images or the Navigator Window.";
        QMessageBox::critical((QWidget *)parent(),"Error",message);
        return;
      }
      //  Find closest control point in network
      ControlPoint *point = g_controlNetwork->FindClosest(sn, samp, line);

      if (point == NULL) {
        QString message = "No points exist for deleting.  Create points ";
        message += "using the right mouse button.";
        QMessageBox::warning((QWidget *)parent(),"Warning",message);
        return;
      }

      deletePoint(point);
    }
    else if (s == Qt::RightButton) {
      p_leftFile = file;
      UniversalGroundMap *gmap = cvp->universalGroundMap();
      gmap->SetImage(samp,line);
      double lat = gmap->UniversalLatitude();
      double lon = gmap->UniversalLongitude();
      if (p_groundOpen && file == p_groundCube->getFilename()) {
        createFixedPoint (lat,lon);
      }
      else {
        createPoint(lat,lon);
      }
    }
  }



  /**
   *   Create new control point
   *  
   * @param lat Latitude value of control point to be created. 
   * @param lon Longitude value of control point to be created. 
   *  
   * @internal
   *   @history 2008-11-20 Jeannie Walldren - Added message box if pointID value
   *                          entered already exists for another ControlPoint.
   *                          Previously this resulted in a PROGRAMMER ERROR from
   *                          ControlPoint. Now, the user will be alerted in a
   *                          message box and prompted to enter a new value for
   *                          pointID.
   *   @history 2008-12-03  Tracie Sucharski - Add error message and cancel
   *                          create new point if the point falls on a single
   *                          image.
   *   @history 2008-12-15  Jeannie Walldren - Throw and catch error before
   *                           creating QMessageBox
   *   @history 2009-03-09  Jeannie Walldren - Clear error message stack after it
   *                           is displayed to user in message box.
   *   @history 2009-04-20  Tracie Sucharski - Set camera for each measure.
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                          namespace std"
   *   @history 2010-07-21  Tracie Sucharski - Modified for new keywords
   *                           associated with implementation of binary
   *                           control networks.
   *   @history 2010-11-19  Tracie Sucharski - Changed p_controlPoint to
   *                           p_editPoint which is a copy rather than a pointer
   *                           directly to the network.
   *   @history 2010-12-15 Tracie Sucharski - Remove netChanged, the point is 
   *                           not changed in the net unless "Save Point" is
   *                           selected.
   *   @history 2011-03-31 Tracie Sucharski - Remove check for point only
   *                           existing on a single image.  This will be
   *                           shown on new point dialog and user can always
   *                           hit "Cancel".
   *   @history 2011-04-08 Tracie Sucharski - Added check for NULL pointer
   *                           before deleting p_editPOint if parent is NULL.
   *  
   */
  void QnetTool::createPoint(double lat,double lon) {

    //  TODO:   ADD AUTOSEED OPTION (CHECKBOX?)

    //  Create list box of all files highlighting those that
    //  contain the point.
    QStringList pointFiles;

    Camera *cam;
    for(int i = 0; i < g_serialNumberList->Size(); i++) {
      if (g_serialNumberList->SerialNumber(i) == p_groundSN) continue;
      cam = g_controlNetwork->Camera(i);
      if(cam->SetUniversalGround(lat, lon)) {
        //  Make sure point is within image boundary
        double samp = cam->Sample();
        double line = cam->Line();
        if (samp >= 1 && samp <= cam->Samples() &&
            line >= 1 && line <= cam->Lines()) {
          pointFiles<<g_serialNumberList->Filename(i).c_str();
        }
      }
    }

    QnetNewPointDialog *newPointDialog = new QnetNewPointDialog();
    newPointDialog->SetFiles(pointFiles);
    if (newPointDialog->exec()) {

      ControlPoint *newPoint =
        new ControlPoint(newPointDialog->ptIdValue->text().toStdString());
      SurfacePoint surfPt = newPoint->GetAprioriSurfacePoint();
      vector<Distance> targRadii = g_controlNetwork->GetTargetRadii();
      surfPt.SetRadii(targRadii[0],targRadii[1],targRadii[2]);
      newPoint->SetAprioriSurfacePoint(surfPt);

      // If this ControlPointId already exists, message box pops up and user is 
      // asked to enter a new value.
      if (g_controlNetwork->ContainsPoint(newPoint->GetId())) {
        iString message = "A ControlPoint with Point Id = [" + newPoint->GetId();
        message += "] already exists.  Re-enter Point Id for this ControlPoint.";
        QMessageBox::warning((QWidget *)parent(), "New Point Id", message.c_str());
        pointFiles.clear();
        delete newPoint;
        newPoint = NULL;
        createPoint(lat, lon);
        return;
      }

      newPoint->SetId(newPointDialog->ptIdValue->text().toStdString());
      newPoint->SetChooserName(Application::UserName());

      for (int i=0; i<newPointDialog->fileList->count(); i++) {
        QListWidgetItem *item = newPointDialog->fileList->item(i);
        if (!newPointDialog->fileList->isItemSelected(item)) continue;
        //  Create measure for any file selected
        ControlMeasure *m = new ControlMeasure;
        //  Find serial number for this file
        iString sn =
                  g_serialNumberList->SerialNumber(item->text().toStdString());
        m->SetCubeSerialNumber(sn);
        int camIndex =
              g_serialNumberList->FilenameIndex(item->text().toStdString());
        cam = g_controlNetwork->Camera(camIndex);
        cam->SetUniversalGround(lat,lon);
        m->SetCoordinate(cam->Sample(),cam->Line());
        m->SetAprioriSample(cam->Sample());
        m->SetAprioriLine(cam->Line());
        m->SetType(ControlMeasure::Manual);
        m->SetChooserName(Application::UserName());
        m->SetCamera(cam);
        newPoint->Add(m);
      }
      if (p_editPoint != NULL && p_editPoint->Parent() == NULL) delete p_editPoint;
      p_editPoint = newPoint;

      //  Load new point in QnetTool
      loadPoint();
      p_qnetTool->setShown(true);
      p_qnetTool->raise();
      
      loadTemplateFile(QString::fromStdString(
          p_pointEditor->templateFilename()));

    
      // emit signal so the nave tool refreshes the list
      emit refreshNavList();
      // emit signal so the nav tool can update edit point
      emit editPointChanged(p_editPoint->GetId());
      colorizeSaveButton();

    }
  }






  /**
   *   Create new Fixed control point
   *  
   * @param lat Latitude value of control point to be created. 
   * @param lon Longitude value of control point to be created. 
   *  
   * @author 2010-11-09 Tracie Sucharski 
   *  
   * @internal
   *  
   */
  void QnetTool::createFixedPoint(double lat,double lon) {

    //  TODO:   ADD AUTOSEED OPTION (CHECKBOX?)

    //  Create list of list box of all files highlighting those that
    //  contain the point.
    QStringList pointFiles;

    Camera *cam;
    for (int i=0; i<g_serialNumberList->Size(); i++) {
      if (g_serialNumberList->SerialNumber(i) == p_groundSN) continue;
      cam = g_controlNetwork->Camera(i);
      if (cam->SetUniversalGround(lat,lon)) {
        //  Make sure point is within image boundary
        double samp = cam->Sample();
        double line = cam->Line();
        if (samp >= 1 && samp <= cam->Samples() &&
            line >= 1 && line <= cam->Lines()) {
          pointFiles<<g_serialNumberList->Filename(i).c_str();
        }
      }
    }

    QnetFixedPointDialog *fixedPointDialog = new QnetFixedPointDialog();
    fixedPointDialog->SetFiles(pointFiles);
    if (fixedPointDialog->exec()) {
      ControlPoint *fixedPoint = 
      new ControlPoint(fixedPointDialog->ptIdValue->text().toStdString()); 

      fixedPoint->SetType(ControlPoint::Fixed);

      //  ??????       What radius , check for dem or shape model
      double radius = p_groundGmap->Projection()->LocalRadius();

      fixedPoint->SetAprioriSurfacePoint(SurfacePoint(
                                          Latitude(lat, Angle::Degrees),
                                          Longitude(lon, Angle::Degrees),
                                          Distance(radius, Distance::Meters)));

      // If this ControlPointId already exists, message box pops up and user is 
      // asked to enter a new value.
      if (g_controlNetwork->ContainsPoint(fixedPoint->GetId())) {
        string message = "A ControlPoint with Point Id = [" + fixedPoint->GetId();
        message += "] already exists.  Re-enter Point Id for this ControlPoint.";
        QMessageBox::warning((QWidget *)parent(),"New Point Id",message.c_str());
        pointFiles.clear();
        delete fixedPoint;
        fixedPoint = NULL;
        createFixedPoint(lat,lon);
        return;
      }

      fixedPoint->SetChooserName(Application::UserName());

      for (int i=0; i<fixedPointDialog->fileList->count(); i++) {
        QListWidgetItem *item = fixedPointDialog->fileList->item(i);
        if (!fixedPointDialog->fileList->isItemSelected(item)) continue;
        //  Create measure for any file selected
        ControlMeasure *m = new ControlMeasure;
        //  Find serial number for this file
        string sn =
        g_serialNumberList->SerialNumber(item->text().toStdString());
        m->SetCubeSerialNumber(sn);
        int camIndex =
        g_serialNumberList->FilenameIndex(item->text().toStdString());
        cam = g_controlNetwork->Camera(camIndex);
        cam->SetUniversalGround(lat,lon);
        m->SetCoordinate(cam->Sample(),cam->Line());
        m->SetType(ControlMeasure::Manual);
        m->SetChooserName(Application::UserName());
        m->SetCamera(cam);
        fixedPoint->Add(m);
      }
      if (p_editPoint != NULL && p_editPoint->Parent() == NULL) {
        delete p_editPoint;
        p_editPoint = NULL;
      }
      p_editPoint = fixedPoint;

      //  Load new point in QnetTool
      loadPoint();
      p_qnetTool->setShown(true);
      p_qnetTool->raise();

      // emit signal so the nave tool refreshes the list
      emit refreshNavList();
      // emit signal so the nav tool can update edit point
      emit editPointChanged(p_editPoint->GetId());
      colorizeSaveButton();
    }
  }



  /**
   * Delete control point
   * 
   * @param point Pointer to control point (net memory) to be deleted.
   * 
   * @internal
   * @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                           namespace std"
   * @history 2010-07-12 Jeannie Walldren - Fixed bug by setting control point
   *                          to NULL if removed from the control net and check
   *                          for NULL points before emitting editPointChanged
   * @history 2011-04-04 Tracie Sucharski - Move code that was after the exec 
   *                          block within, so that if the Cancel button is
   *                          selected, nothing else happens.
   * @history 2011-07-15 Tracie Sucharski - Print info about deleting editLock 
   *                          points and reference measures. 
   *  
   */
  void QnetTool::deletePoint(ControlPoint *point) {

    // Make a copy
    if (p_editPoint != NULL && p_editPoint->Parent() == NULL) {
      delete p_editPoint;
      p_editPoint = NULL;
    }
    p_editPoint = new ControlPoint;
    *p_editPoint = *point;
    loadPoint();

    //  Change point in viewport to red so user can see what point they are 
    //  about to delete.
    // the nav tool will update edit point
    emit editPointChanged(p_editPoint->GetId());

    QnetDeletePointDialog *deletePointDialog = new QnetDeletePointDialog;
    iString CPId = p_editPoint->GetId();
    deletePointDialog->pointIdValue->setText(CPId.c_str());
    //  Need all files for this point
    for (int i=0; i<p_editPoint->GetNumMeasures(); i++) {
      ControlMeasure &m = *(*p_editPoint)[i];
      iString file = g_serialNumberList->Filename(m.GetCubeSerialNumber());
      deletePointDialog->fileList->addItem(file.c_str());
    }

    if (deletePointDialog->exec()) {

      //  Delete entire control point
      if (deletePointDialog->deleteAllCheckBox->isChecked()) {
        //  First get rid of deleted point from g_filteredPoints list
        //  need index in control net for pt
        //int i = g_controlNetwork->
        //g_filteredPoints.
        p_qnetTool->setShown(false);
        // remove this point from the control network
        if (g_controlNetwork->DeletePoint(p_editPoint->GetId()) ==
                                          ControlPoint::PointLocked) {
          QMessageBox::information((QWidget *)parent(),"EditLocked Point",
              "This point is EditLocked and cannot be deleted.");
          return;
        }
        if (p_editPoint != NULL && p_editPoint->Parent() == NULL) {
          delete p_editPoint;
          p_editPoint = NULL;
        }
        //  emit signal so the nav tool refreshes the list
        emit refreshNavList();
      }

      //  Delete specific measures from control point
      else {
        //  Keep track of editLocked measures for reporting
        int lockedMeasures = 0;
        for (int i=0; i<deletePointDialog->fileList->count(); i++) {
          QListWidgetItem *item = deletePointDialog->fileList->item(i);
          if (!deletePointDialog->fileList->isItemSelected(item)) continue;

          //  Do not delete reference without asking user
          if (p_editPoint->IsReferenceExplicit() &&
                (p_editPoint->GetRefMeasure()->GetCubeSerialNumber() ==
                (*p_editPoint)[i]->GetCubeSerialNumber())) {
            QString message = "You are trying to delete the Reference measure."
                "  Do you really want to delete the Reference measure?";
            switch (QMessageBox::question((QWidget *)parent(),
                    "Delete Reference measure?", message, "&Yes", "&No", 0, 0)) {
              //  Yes or Enter pressed, skip to end of switch todelete the measure
              case 0:
                break;
              //  No, continue to next measure in the loop
              case 1:
                continue;
            }
          }

          if (p_editPoint->Delete(i) == ControlMeasure::MeasureLocked) {
            lockedMeasures++;
          }
        }

        if (lockedMeasures > 0) {
          QMessageBox::information((QWidget *)parent(),"EditLocked Measures",
                QString::number(lockedMeasures) + " / "
                + QString::number(deletePointDialog->fileList->selectedItems().size())
                + " measures are EditLocked and were not deleted.");
        }

        p_leftFile = "";
        loadPoint();
        p_qnetTool->setShown(true);
        p_qnetTool->raise();

        loadTemplateFile(QString::fromStdString(
            p_pointEditor->templateFilename()));
      }

      // emit a signal to alert user to save when exiting 
      emit netChanged();
  
      // emit signal so the nav tool can update edit point
      if (p_editPoint != NULL) {
        emit editPointChanged(p_editPoint->GetId());
      }
      else {
        // if the entire point is deleted, update with point Id = ""
        // this signal is connected to QnetTool::paintAllViewports
        // and QnetNavTool::updateEditPoint
        emit editPointChanged("");
      }
    }
  }



  /**
   * Modify control point
   * 
   * @param point Pointer to control point to be modified. 
   *
   * @history 2009-09-15 Tracie Sucharski - Add error check for points
   *                       with no measures.
   */
  void QnetTool::modifyPoint(ControlPoint *point) {

    //  If no measures, print info and return
    if (point->GetNumMeasures() == 0) {
      QString message = "This point has no measures.";
      QMessageBox::warning((QWidget *)parent(),"Warning",message);
      // update nav list to re-highlight old point
      if (p_editPoint != NULL) {
        // emit signal so the nav tool can update edit point
        emit editPointChanged(p_editPoint->GetId());
      }
      else {
        // this signal is connected to QnetTool::paintAllViewports 
        // and QnetNavTool::updateEditPoint
        emit editPointChanged("");
      }
      return;
    }
    //  Make a copy of point for editing, first make sure memory not already
    //  allocated
    if (p_editPoint != NULL && p_editPoint->Parent() == NULL) {
      delete p_editPoint;
      p_editPoint = NULL;
    }
    p_editPoint = new ControlPoint;
    *p_editPoint = *point;

    //  If navTool modfify button pressed, p_leftFile needs to be reset
    //  TODO: better way - have 2 slots
    if (sender() != this) p_leftFile = "";
    loadPoint();
    p_qnetTool->setShown(true);
    p_qnetTool->raise();
    loadTemplateFile(QString::fromStdString(
        p_pointEditor->templateFilename()));

    // emit signal so the nav tool can update edit point
    emit editPointChanged(p_editPoint->GetId());

    // New point loaded, make sure Save Measure Button text is default
    p_savePoint->setPalette(p_saveDefaultPalette);
  }

  /**
   * @brief Load point into QnetTool. 
   * @internal 
   *   @history 2008-11-26  Jeannie Walldren - Added "Number of Measures" to
   *                           QnetTool point information.
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                          namespace std"
   *   @history 2010-10-29 Tracie Sucharski - Changed pointfiles to QStringList
   *   @history 2011-04-20 Tracie Sucharski - Was not setting EditLock check box
   *   @history 2011-07-18 Tracie Sucharski - Fixed bug with loading
   *                          ground measure-use AprioriSurface point, not lat,lon
   *                          of reference measure unless there is no apriori
   *                          surface point.
   *
   */
  void QnetTool::loadPoint () {

    //  Write pointId
    iString CPId = p_editPoint->GetId();
    QString ptId("Point ID:  ");
    ptId += (QString) CPId;
    p_ptIdValue->setText(ptId);

    //  Write point type
//    QString ptType("Point Type:  ");
//    ptType += (QString) p_editPoint->GetPointTypeString();
//    p_pointType->setText(ptType);
    p_pointType->setCurrentIndex((int) p_editPoint->GetType());

    //  Write number of measures
    QString ptsize = "Number of Measures:  " +
                   QString::number(p_editPoint->GetNumMeasures());
    p_numMeasures->setText(ptsize);

    //  Set EditLock box correctly
    p_lockPoint->setChecked(p_editPoint->IsEditLocked());

    //  Set ignore box correctly
    p_ignorePoint->setChecked(p_editPoint->IsIgnored());

    // Clear combo boxes
    p_leftCombo->clear();
    p_rightCombo->clear();
    p_pointFiles.clear();
    
    // Find in point and delete, it will be re-created with current
    // ground source if this is a fixed point
    if (p_editPoint->HasSerialNumber(p_groundSN)) {
        p_editPoint->Delete(p_groundSN);
    }

    //  If fixed, add ground source file to combos, create a measure for
    //  the ground source, load reference on left, ground source on right
    if (p_groundOpen && (p_editPoint->GetType() != ControlPoint::Free)) {

      // TODO:  Does open ground source match point ground source


      // Use apriori surface point to find location on ground source.  If
      // apriori surface point does not exist use reference measure
      double lat = 0.;
      double lon = 0.;
      if (p_editPoint->HasAprioriCoordinates()) {
        SurfacePoint sPt = p_editPoint->GetAprioriSurfacePoint();
        lat = sPt.GetLatitude().GetDegrees();
        lon = sPt.GetLongitude().GetDegrees();
      }
      else {
        ControlMeasure m = *(p_editPoint->GetRefMeasure());
        int camIndex = g_serialNumberList->SerialNumberIndex(m.GetCubeSerialNumber());
        Camera *cam;
        cam = g_controlNetwork->Camera(camIndex);
        cam->SetImage(m.GetSample(),m.GetLine());
        lat = cam->UniversalLatitude();
        lon = cam->UniversalLongitude();
      }

      //  Try to locate point position on current ground source,
      //  TODO ???if doesn't exist,???
      if (!p_groundGmap->SetUniversalGround(lat,lon)) {
        QString message = "This point does not exist on the ground source.\n";
        message += "Latitude = " + QString::number(lat);
        message += "  Longitude = " + QString::number(lon);
        message += "\n A ground measure will not be created.";
        QMessageBox::warning((QWidget *)parent(),"Warning",message);
      }
      else {
        // Create a temporary measure to hold the ground point info for ground source
        // This measure will be deleted when the ControlPoint is saved to the
        // ControlNet.
        Isis::ControlMeasure *groundMeasure = new ControlMeasure;
        groundMeasure->SetCubeSerialNumber(p_groundSN);
        groundMeasure->SetType(ControlMeasure::Candidate);
        groundMeasure->SetCoordinate(p_groundGmap->Sample(),p_groundGmap->Line());
        p_editPoint->Add(groundMeasure);
      }
    }


    //  Need all files for this point
    for (int i=0; i<p_editPoint->GetNumMeasures(); i++) {
      ControlMeasure &m = *(*p_editPoint)[i];
      iString file = g_serialNumberList->Filename(m.GetCubeSerialNumber());
      p_pointFiles<<file;
      QString tempFilename = Filename(file).Name().c_str();
      p_leftCombo->addItem(tempFilename);
      p_rightCombo->addItem(tempFilename);
      if (p_editPoint->IsReferenceExplicit() &&
          (QString)m.GetCubeSerialNumber() == p_editPoint->GetReferenceSN()) {
          p_leftCombo->setItemData(i,QFont("DejaVu Sans", 12, QFont::Bold), Qt::FontRole);
          p_rightCombo->setItemData(i,QFont("DejaVu Sans", 12, QFont::Bold), Qt::FontRole);
      }
    }


    //  TODO:  WHAT HAPPENS IF THERE IS ONLY ONE MEASURE IN THIS CONTROLPOINT??
    // Assuming combo loaded in same order as measures in the control point-is
    // this a safe assumption???
    // 
    //  Find the file from the cubeViewport that was originally used to select 
    //  the point, this will be displayed on the left ChipViewport, unless the
    //  point was selected on the ground source image.  In this case, simply
    //  load the first measure on the left.
    int leftIndex = 0;
    int rightIndex = 0;
    //  Check for reference
    if (p_editPoint->IsReferenceExplicit()) {
      leftIndex = p_editPoint->IndexOfRefMeasure();
    }
    else {
      if (p_editPoint->GetType() == ControlPoint::Free &&
          (p_leftFile.length() != 0)) {
        iString tempFilename = Filename(p_leftFile).Name();
        leftIndex = p_leftCombo->findText(tempFilename);
      }
    }

    if (p_groundOpen && (p_editPoint->GetType() != ControlPoint::Free))  {
      rightIndex = p_rightCombo->findText((QString)p_groundSN);
    }
    else {
      if (leftIndex == 0) {
        rightIndex = 1;
      }
      else {
        rightIndex = 0;
      }

    }

    //  Handle pts with a single measure, for now simply put measure on left/right
    //  Evenutally put on left with black on right??
    if (rightIndex > p_editPoint->GetNumMeasures()-1) rightIndex = 0;
    p_rightCombo->setCurrentIndex(rightIndex);
    p_leftCombo->setCurrentIndex(leftIndex);
    //  Initialize pointEditor with measures
    selectLeftMeasure(leftIndex);
    selectRightMeasure(rightIndex);

    updateSurfacePointInfo ();

    loadMeasureTable();
  }




  void QnetTool::loadMeasureTable () {
    if (p_measureTable != NULL) delete p_measureTable;
    p_measureTable = new QTableWidget();
    p_measureTable->setRowCount(p_editPoint->GetNumMeasures());
    p_measureTable->setColumnCount(NUMCOLUMNS);

    QStringList labels;
    for (int i=0; i<NUMCOLUMNS; i++) {
      labels<<measureColumnToString((MeasureColumns)i);
    }
    p_measureTable->setHorizontalHeaderLabels(labels);

    //  Fill in values
    for (int row=0; row<p_editPoint->GetNumMeasures(); row++) {
      int column = 0;
      ControlMeasure &m = *(*p_editPoint)[row];

      QString file = QString::fromStdString(
                       g_serialNumberList->Filename(m.GetCubeSerialNumber()));
      QTableWidgetItem *tableItem = new QTableWidgetItem(QString(file));
      p_measureTable->setItem(row,column++,tableItem);

      tableItem = new QTableWidgetItem(QString(m.GetCubeSerialNumber()));
      p_measureTable->setItem(row,column++,tableItem);

      tableItem = new QTableWidgetItem();
      tableItem->setData(0,m.GetSample());
      p_measureTable->setItem(row,column++,tableItem);

      tableItem = new QTableWidgetItem();
      tableItem->setData(0,m.GetLine());
      p_measureTable->setItem(row,column++,tableItem);

      if (m.GetSampleResidual() == Isis::Null) {  
        tableItem = new QTableWidgetItem(QString("Null"));
      }
      else {
        tableItem = new QTableWidgetItem();
        tableItem->setData(0,m.GetSampleResidual());
      }
      p_measureTable->setItem(row,column++,tableItem);

      if (m.GetLineResidual() == Isis::Null) {  
        tableItem = new QTableWidgetItem(QString("Null"));
      }
      else {
        tableItem = new QTableWidgetItem();
        tableItem->setData(0,m.GetLineResidual());
      }
      p_measureTable->setItem(row,column++,tableItem);

      if (m.GetResidualMagnitude() == Isis::Null) {
        tableItem = new QTableWidgetItem(QString("Null"));
      }
      else {
        tableItem = new QTableWidgetItem();
        tableItem->setData(0,m.GetResidualMagnitude());
      }
      p_measureTable->setItem(row,column++,tableItem);

      double sampleShift = m.GetSampleShift();
      if (sampleShift == Isis::Null) {
        tableItem = new QTableWidgetItem(QString("Null"));
      }
      else {
        tableItem = new QTableWidgetItem();
        tableItem->setData(0,sampleShift);
      }
      p_measureTable->setItem(row,column++,tableItem);

      double lineShift = m.GetLineShift();
      if (lineShift == Isis::Null) {
        tableItem = new QTableWidgetItem(QString("Null"));
      }
      else {
        tableItem = new QTableWidgetItem();
        tableItem->setData(0,lineShift);
      }
      p_measureTable->setItem(row,column++,tableItem);

      double pixelShift = m.GetPixelShift();
      if (pixelShift == Isis::Null) {
        tableItem = new QTableWidgetItem(QString("Null"));
      }
      else {
        tableItem = new QTableWidgetItem();
        tableItem->setData(0,pixelShift);
      }
      p_measureTable->setItem(row,column++,tableItem);

      double goodnessOfFit = m.GetLogData(
                      ControlMeasureLogData::GoodnessOfFit).GetNumericalValue();
      if (goodnessOfFit == Isis::Null) {
        tableItem = new QTableWidgetItem(QString("Null"));
      }
      else {
        tableItem = new QTableWidgetItem();
        tableItem->setData(0,goodnessOfFit);
      }
      p_measureTable->setItem(row,column++,tableItem);

      if (m.IsIgnored()) tableItem = new QTableWidgetItem("True");
      if (!m.IsIgnored()) tableItem = new QTableWidgetItem("False");
      p_measureTable->setItem(row,column++,tableItem);

      if (IsMeasureLocked(m.GetCubeSerialNumber()))
        tableItem = new QTableWidgetItem("True");
      if (!IsMeasureLocked(m.GetCubeSerialNumber()))
        tableItem = new QTableWidgetItem("False");
      p_measureTable->setItem(row,column++,tableItem);

      tableItem = new QTableWidgetItem(QString::fromStdString(
                  ControlMeasure::MeasureTypeToString(m.GetType())));
      p_measureTable->setItem(row,column++,tableItem);

      if (m.GetAprioriSample() == Isis::Null) {
        tableItem = new QTableWidgetItem("Null");
      }
      else {
        tableItem = new QTableWidgetItem();
        tableItem->setData(0,m.GetAprioriSample());
      }
      p_measureTable->setItem(row,column++,tableItem);

      if (m.GetAprioriLine() == Isis::Null) {
        tableItem = new QTableWidgetItem("Null");
      }
      else {
        tableItem = new QTableWidgetItem();
        tableItem->setData(0,m.GetAprioriLine());
      }
      p_measureTable->setItem(row,column,tableItem);

      //  If reference measure set font on this row to bold
      if (p_editPoint->IsReferenceExplicit() &&
          (QString)m.GetCubeSerialNumber() == p_editPoint->GetReferenceSN()) {
        QFont font;
        font.setBold(true);

        for (int col=0; col<p_measureTable->columnCount(); col++)
          p_measureTable->item(row, col)->setFont(font);
      }

    }

    p_measureTable->resizeColumnsToContents();
    p_measureTable->resizeRowsToContents();
    p_measureTable->setMinimumWidth(1600);
    p_measureTable->setAlternatingRowColors(true);
    p_measureTable->setSortingEnabled(true);
    p_measureTable->show();
  }



  QString QnetTool::measureColumnToString(QnetTool::MeasureColumns column) {
    switch (column) {
      case FILENAME:
        return "Filename";
      case CUBESN:
        return "Serial #";
      case SAMPLE:
        return "Sample";
      case LINE:
        return "Line";
      case SAMPLERESIDUAL:
        return "Sample Residual";
      case LINERESIDUAL:
        return "Line Residual";
      case RESIDUALMAGNITUDE:
        return "Residual Magnitude";
      case SAMPLESHIFT:
        return "Sample Shift";
      case LINESHIFT:
        return "Line Shift";
      case PIXELSHIFT:
        return "Pixel Shift";
      case GOODNESSOFFIT:
        return "Goodness of Fit";
      case IGNORED:
        return "Ignored";
      case EDITLOCK:
        return "Edit Lock";
      case TYPE:
        return "Measure Type";
      case APRIORISAMPLE:
        return "Apriori Sample";
      case APRIORILINE:
        return "Apriori Line";
    }
    throw iException::Message(iException::Programmer,
        "Invalid measure column passed to measureColumnToString", _FILEINFO_);
  }



  /** 
   * Update the Surface Point Information in the QnetTool window
   *  
   * @author 2011-03-01 Tracie Sucharski 
   *  
   * @internal 
   * @history 2011-05-12 Tracie Sucharski - Type printing Apriori Values 
   * @history 2011-05-24 Tracie Sucharski - Set target radii on apriori 
   *                        surface point, so that sigmas can be converted to
   *                        meters.
   */
  void QnetTool::updateSurfacePointInfo () {

    QString s;
  
    SurfacePoint aprioriPoint = p_editPoint->GetAprioriSurfacePoint();
    if (aprioriPoint.GetLatitude().GetDegrees() == Isis::Null) {
      s = "AprioriLatitude:  Null";
    }
    else {
      s = "Apriori Latitude:  " +
          QString::number(aprioriPoint.GetLatitude().GetDegrees());
    }
    p_pointAprioriLatitude->setText(s);
    if (aprioriPoint.GetLongitude().GetDegrees() == Isis::Null) {
      s = "Apriori Longitude:  Null";
    }
    else {
      s = "Apriori Longitude:  " +
          QString::number(aprioriPoint.GetLongitude().GetDegrees());
    }
    p_pointAprioriLongitude->setText(s);
    if (aprioriPoint.GetLocalRadius().GetMeters() == Isis::Null) {
      s = "Apriori Radius:  Null";
    }
    else {
      s = "Apriori Radius:  " +
          QString::number(aprioriPoint.GetLocalRadius().GetMeters(),'f',2) +
          " <meters>";
    }
    p_pointAprioriRadius->setText(s);
  
    if (aprioriPoint.Valid()) {
      vector<Distance> targRadii = g_controlNetwork->GetTargetRadii();
      aprioriPoint.SetRadii(targRadii[0],targRadii[1],targRadii[2]);

      if (aprioriPoint.GetLatSigmaDistance().GetMeters() == Isis::Null) {
        s = "Apriori Latitude Sigma:  Null";
      }
      else {
        s = "Apriori Latitude Sigma:  " +
            QString::number(aprioriPoint.GetLatSigmaDistance().GetMeters()) +
            " <meters>";
      }
      p_pointAprioriLatitudeSigma->setText(s);
      if (aprioriPoint.GetLonSigmaDistance().GetMeters() == Isis::Null) {
        s = "Apriori Longitude Sigma:  Null";
      }
      else {
        s = "Apriori Longitude Sigma:  " +
            QString::number(aprioriPoint.GetLonSigmaDistance().GetMeters()) +
            " <meters>";
      }
      p_pointAprioriLongitudeSigma->setText(s);
      if (aprioriPoint.GetLocalRadiusSigma().GetMeters() == Isis::Null) {
        s = "Apriori Radius Sigma:  Null";
      }
      else {
        s = "Apriori Radius Sigma:  " +
            QString::number(aprioriPoint.GetLocalRadiusSigma().GetMeters()) +
            " <meters>";
      }
      p_pointAprioriRadiusSigma->setText(s);
    }
    else {
      s = "Apriori Latitude Sigma:  Null";
      p_pointAprioriLatitudeSigma->setText(s);
      s = "Apriori Longitude Sigma:  Null";
      p_pointAprioriLongitudeSigma->setText(s);
      s = "Apriori Radius Sigma:  Null";
      p_pointAprioriRadiusSigma->setText(s);
    }
      
          
    SurfacePoint point = p_editPoint->GetAdjustedSurfacePoint();
    if (point.GetLatitude().GetDegrees() == Isis::Null) {
      s = "Adjusted Latitude:  Null";
    }
    else {
      s = "Adjusted Latitude:  " + QString::number(point.GetLatitude().GetDegrees());
    }
    p_pointLatitude->setText(s);
    if (point.GetLongitude().GetDegrees() == Isis::Null) {
      s = "Adjusted Longitude:  Null";
    }
    else {
      s = "Adjusted Longitude:  " + QString::number(point.GetLongitude().GetDegrees());
    }
    p_pointLongitude->setText(s);
    if (point.GetLocalRadius().GetMeters() == Isis::Null) {
      s = "Adjusted Radius:  Null";
    }
    else {
      s = "Adjusted Radius:  " +
          QString::number(point.GetLocalRadius().GetMeters(),'f',2) + " <meters>";
    }
    p_pointRadius->setText(s);



  }




  /** 
   * Select left measure 
   *  
   * @param index Index of file from the point files vector 
   *  
   * @internal
   * @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                          namespace std"
   * @history 2011-07-06 Tracie Sucharski - If point is Locked, and measure is 
   *                          reference, lock the measure. 
   */
  void QnetTool::selectLeftMeasure(int index) {
    iString file = p_pointFiles[index];

    iString serial = g_serialNumberList->SerialNumber(file);

    // Make sure to clear out leftMeasure before making a copy of the selected
    // measure.
    if (p_leftMeasure != NULL) {
      delete p_leftMeasure;
      p_leftMeasure = NULL;
    }
    p_leftMeasure = new ControlMeasure();
    //  Find measure for each file
    *p_leftMeasure = *((*p_editPoint)[serial]);

    //  If p_leftCube is not null, delete before creating new one
    if (p_leftCube != NULL) delete p_leftCube;
    p_leftCube = new Cube();
    p_leftCube->open(file);

    //  Update left measure of pointEditor
    p_pointEditor->setLeftMeasure (p_leftMeasure, p_leftCube,
                                   p_editPoint->GetId());
    updateLeftMeasureInfo ();
      
  }


  /** 
   * Select right measure 
   *  
   * @param index  Index of file from the point files vector 
   *  
   * @internal
   * @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                          namespace std"
   */
  void QnetTool::selectRightMeasure(int index) {

    iString file = p_pointFiles[index];

    iString serial = g_serialNumberList->SerialNumber(file);

    // Make sure to clear out rightMeasure before making a copy of the selected
    // measure.
    if (p_rightMeasure != NULL) {
      delete p_rightMeasure;
      p_rightMeasure = NULL;
    }
    p_rightMeasure = new ControlMeasure();
    //  Find measure for each file
    *p_rightMeasure = *((*p_editPoint)[serial]);

    //  If p_leftCube is not null, delete before creating new one
    if (p_rightCube != NULL) delete p_rightCube;
    p_rightCube = new Cube();
    p_rightCube->open(file);

    //  Update left measure of pointEditor
    p_pointEditor->setRightMeasure (p_rightMeasure,p_rightCube,
                                    p_editPoint->GetId());
    updateRightMeasureInfo ();

  }




  /**
   * @internal
   * @history 2008-11-24  Jeannie Walldren - Added "Goodness of Fit" to left 
   *                         measure info.
   * @history 2010-07-22  Tracie Sucharski - Updated new measure types  
   *                           associated with implementation of binary
   *                           control networks.
   * @history 2010-12-27  Tracie Sucharski - Write textual Null instead of 
   *                           the numeric Null for sample & line residuals.
   * @history 2011-04-20  Tracie Sucharski - Set EditLock check box correctly 
   * @history 2011-05-20  Tracie Sucharski - Added Reference output 
   * 
   */
  void QnetTool::updateLeftMeasureInfo () {

    //  Set editLock measure box correctly
    p_lockLeftMeasure->setChecked(IsMeasureLocked(
                                       p_leftMeasure->GetCubeSerialNumber()));
    //  Set ignore measure box correctly
    p_ignoreLeftMeasure->setChecked(p_leftMeasure->IsIgnored());

    QString s = "Reference: ";
    if (p_editPoint->IsReferenceExplicit() &&
        (QString(p_leftMeasure->GetCubeSerialNumber()) == p_editPoint->GetReferenceSN())) {
      s += "True";
    }
    else {
      s += "False";
    }
    p_leftReference->setText(s);

    s = "Measure Type: ";
    if (p_leftMeasure->GetType() == ControlMeasure::Candidate) s+= "Candidate";
    if (p_leftMeasure->GetType() == ControlMeasure::Manual) s+= "Manual";
    if (p_leftMeasure->GetType() == ControlMeasure::RegisteredPixel) s+= "RegisteredPixel";
    if (p_leftMeasure->GetType() == ControlMeasure::RegisteredSubPixel) s+= "RegisteredSubPixel";
    p_leftMeasureType->setText(s);

    if (p_leftMeasure->GetSampleResidual() == Isis::Null) {
      s = "Sample Residual: Null";
    }
    else {
      s = "Sample Residual: " + QString::number(p_leftMeasure->GetSampleResidual());
    }
    p_leftSampError->setText(s);
    if (p_leftMeasure->GetLineResidual() == Isis::Null) {
      s = "Line Residual: Null";
    }
    else {
      s = "Line Residual: " + QString::number(p_leftMeasure->GetLineResidual());
    }
    p_leftLineError->setText(s);

    double goodnessOfFit = p_leftMeasure->GetLogData(
                    ControlMeasureLogData::GoodnessOfFit).GetNumericalValue();
    if (goodnessOfFit == Isis::Null) {
      s = "Goodness of Fit: Null";
    }
    else {
      s = "Goodness of Fit: " + QString::number(goodnessOfFit);
    }
    p_leftGoodness->setText(s);

  }



  /**
   * @internal
   * @history 2008-11-24  Jeannie Walldren - Added "Goodness of Fit" to right
   *                           measure info.
   * @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                           namespace std"
   * @history 2010-07-22  Tracie Sucharski - Updated new measure types  
   *                           associated with implementation of binary
   *                           control networks.
   * @history 2010-12-27  Tracie Sucharski - Write textual Null instead of 
   *                           the numeric Null for sample & line residuals. 
   * @history 2011-04-20  Tracie Sucharski - Set EditLock check box correctly 
   * @history 2011-05-20  Tracie Sucharski - Added Reference output 
   * 
   */

  void QnetTool::updateRightMeasureInfo () {

  //  Set editLock measure box correctly
    p_lockRightMeasure->setChecked(IsMeasureLocked(
                                        p_rightMeasure->GetCubeSerialNumber()));
      //  Set ignore measure box correctly
    p_ignoreRightMeasure->setChecked(p_rightMeasure->IsIgnored());

    QString s = "Reference: ";
    if (p_editPoint->IsReferenceExplicit() &&
        (QString(p_rightMeasure->GetCubeSerialNumber()) == p_editPoint->GetReferenceSN())) {
      s += "True";
    }
    else {
      s += "False";
    }

    p_rightReference->setText(s);

    s = "Measure Type: ";
    if (p_rightMeasure->GetType() == ControlMeasure::Candidate) s+= "Candidate";
    if (p_rightMeasure->GetType() == ControlMeasure::Manual) s+= "Manual";
    if (p_rightMeasure->GetType() == ControlMeasure::RegisteredPixel) s+= "RegisteredPixel";
    if (p_rightMeasure->GetType() == ControlMeasure::RegisteredSubPixel) s+= "RegisteredSubPixel";
    p_rightMeasureType->setText(s);

    if (p_rightMeasure->GetSampleResidual() == Isis::Null) {
      s = "Sample Residual: Null";
    }
    else {
      s = "Sample Residual: " + QString::number(p_rightMeasure->GetSampleResidual());
    }
    p_rightSampError->setText(s);
    if (p_rightMeasure->GetLineResidual() == Isis::Null) {
      s = "Line Residual: Null";
    }
    else {
      s = "Line Residual: " + QString::number(p_rightMeasure->GetLineResidual());
    }
    p_rightLineError->setText(s);
    double goodnessOfFit = p_rightMeasure->GetLogData(
                    ControlMeasureLogData::GoodnessOfFit).GetNumericalValue();
    if (goodnessOfFit == Isis::Null) {
      s = "Goodness of Fit: Null";
    }
    else {
      s = "Goodness of Fit: " + QString::number(goodnessOfFit);
    }
    p_rightGoodness->setText(s);

  }


  /**
   * Add measure to point 
   *  
   * @internal
   * @history 2010-07-22  Tracie Sucharski - MeasureType of Estimated is now 
   *                           Reference.  This change associated with
   *                           implementation of binary control networks.
   * @history 2011-04-06  Tracie Sucharski - If not a fixed point, use the 
   *                           Reference measure to get lat,lon. 
   * 
   */
  void QnetTool::addMeasure() {

    //  Create list of list box of all files highlighting those that
    //  contain the point, but that do not already have a measure.
    QStringList pointFiles;

    //  Initialize camera for all images in control network,  
    //  TODO::   Needs to be moved to QnetFileTool.cpp
    Camera *cam;

    // If no apriori or adjusted lat/lon for this point, use lat/lon of first measure
    double lat = p_editPoint->GetBestSurfacePoint().GetLatitude().GetDegrees();
    double lon = p_editPoint->GetBestSurfacePoint().GetLongitude().GetDegrees();
    if (lat == Null || lon == Null) {
      ControlMeasure m = *(p_editPoint->GetRefMeasure());
      int camIndex = g_serialNumberList->SerialNumberIndex(m.GetCubeSerialNumber());
      cam = g_controlNetwork->Camera(camIndex);
      //cam = m.Camera();
      cam->SetImage(m.GetSample(),m.GetLine());
      lat = cam->UniversalLatitude();
      lon = cam->UniversalLongitude();
    }

    for (int i=0; i<g_serialNumberList->Size(); i++) {
      cam = g_controlNetwork->Camera(i);
      if (cam->SetUniversalGround(lat,lon)) {
        //  Make sure point is within image boundary
        double samp = cam->Sample();
        double line = cam->Line();
        if (samp >= 1 && samp <= cam->Samples() &&
            line >= 1 && line <= cam->Lines()) {
          pointFiles<<g_serialNumberList->Filename(i).c_str();
        }
      }
    }

    QnetNewMeasureDialog *newMeasureDialog = new QnetNewMeasureDialog();
    newMeasureDialog->SetFiles(*p_editPoint,pointFiles);
    if (newMeasureDialog->exec()) {
      for (int i=0; i<newMeasureDialog->fileList->count(); i++) {
        QListWidgetItem *item = newMeasureDialog->fileList->item(i);
        if (!newMeasureDialog->fileList->isItemSelected(item)) continue;
        //  Create measure for any file selected
        ControlMeasure *m = new ControlMeasure;
        //  Find serial number for this file
        iString sn = g_serialNumberList->SerialNumber((iString) item->text());
        m->SetCubeSerialNumber(sn);
        int camIndex =
              g_serialNumberList->FilenameIndex(item->text().toStdString());
        cam = g_controlNetwork->Camera(camIndex);
        cam->SetUniversalGround(lat,lon);
        m->SetCoordinate(cam->Sample(),cam->Line());
        m->SetAprioriSample(cam->Sample());
        m->SetAprioriLine(cam->Line());
        m->SetType(ControlMeasure::Manual);
        m->SetChooserName(Application::UserName());
        p_editPoint->Add(m);
      }
      loadPoint();
      p_qnetTool->setShown(true);
      p_qnetTool->raise();

      loadTemplateFile(QString::fromStdString(
          p_pointEditor->templateFilename()));


      // emit signal so the nav tool can update edit point
      emit editPointChanged(p_editPoint->GetId());
      colorizeSaveButton();
    }
  }


  /**
   * Event filter for QnetTool.  Determines whether to update left or right 
   * measure info. 
   * 
   * @param o Pointer to QObject
   * @param e Pointer to QEvent
   * 
   * @return @b bool Indicates whether the event type is "Leave".
   * 
   */
  bool QnetTool::eventFilter(QObject *o, QEvent *e) {
    if(e->type() != QEvent::Leave) return false;
    if(o == p_leftCombo->view()) {
      updateLeftMeasureInfo();
      p_leftCombo->hidePopup();
    }
    if (o == p_rightCombo->view()) {
      updateRightMeasureInfo ();
      p_rightCombo->hidePopup();
    }
    return true;
  }


  /**
   * Take care of drawing things on a viewPort.
   * This is overiding the parents paintViewport member. 
   *  
   * @param vp Pointer to Viewport to be painted
   * @param painter 
   */
  void QnetTool::paintViewport(MdiCubeViewport *vp, QPainter *painter) {
    drawAllMeasurments (vp,painter);

  }


  /** 
   * This method will repaint the given Point ID in each viewport 
   * Note: The pointId parameter is here even though it's not used because 
   * the signal (QnetTool::editPointChanged connected to this slot is also 
   * connected to another slot (QnetNavTool::updateEditPoint which needs the 
   * point Id.  TODO:  Clean this up, use 2 different signals? 
   *  
   * @param pointId 
   *  
   * @internal
   * @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                          namespace std"
   */    
  void QnetTool::paintAllViewports(QString pointId) {
    
    // Take care of drawing things on all viewPorts.
    // Calling update will cause the Tool class to call all registered tools 
    // if point has been deleted, this will remove it from the main window
    MdiCubeViewport *vp;
    for (int i=0; i<(int)cubeViewportList()->size(); i++) {
      vp = (*(cubeViewportList()))[i];
       vp->viewport()->update();
    }
  }

  /**
   * Draw all measurments which are on this viewPort
   * @param vp Viewport whose measurements will be drawn
   * @param painter
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                          namespace std"
   *   @history 2010-06-08 Jeannie Walldren - Fixed bug that was causing ignored
   *                          measures not be drawn as yellow unless QnetTool was
   *                          open
   *   @history 2010-07-01 Jeannie Walldren - Modified to draw points selected in
   *                          QnetTool last so they lay on top of all other points
   *                          in the image.
   *   @history 2011-04-15 Tracie Sucharski - Fixed bug which was causing all
   *                          measures to be drawn on all cubes.  Also removed
   *                          loop through measures, instead just get measure
   *                          for given serial number.
   */
  void QnetTool::drawAllMeasurments(MdiCubeViewport *vp, QPainter *painter) {
    
    // Without a controlnetwork there are no points
    if(g_controlNetwork == 0) return;

    // Don't show the measurments on cubes not in the serial number list
    // TODO: Should we show them anyway
    // TODO: Should we add the SN to the viewPort
    string serialNumber = Isis::SerialNumber::Compose(*vp->cube());

    if(!g_serialNumberList->HasSerialNumber(serialNumber)) return;

    // loop through all points in the control net
    for (int i = 0; i < g_controlNetwork->GetNumPoints(); i++) {
      Isis::ControlPoint &p = *((*g_controlNetwork)[i]);
      // check whether this point is contained in the image
      if (p.HasSerialNumber(serialNumber)) {
        // Find the measurments on the viewport
        double samp = p[serialNumber]->GetSample();
        double line = p[serialNumber]->GetLine();
        int x, y;
        vp->cubeToViewport(samp, line, x, y);
        // if the point is ignored,
        if (p.IsIgnored()) {
          painter->setPen(QColor(255, 255, 0)); // set point marker yellow
        }
        // point is not ignored, but measure matching this image is ignored,
        else if (p[serialNumber]->IsIgnored()) {
          painter->setPen(QColor(255, 255, 0)); // set point marker yellow
        }
        // Neither point nor measure is not ignored and the measure is fixed,
        else if (p.GetType() != ControlPoint::Free) {
          painter->setPen(Qt::magenta);// set point marker magenta
        }
        else {
          painter->setPen(Qt::green); // set all other point markers green
        }
        // draw points
        painter->drawLine(x - 5, y, x + 5, y);
        painter->drawLine(x, y - 5, x, y + 5);
      }
    }
    // if QnetTool is open,
    if (p_editPoint != NULL) {
      // and the selected point is in the image,
      if(p_editPoint->HasSerialNumber(serialNumber)) {
        // find the measurement
        double samp = (*p_editPoint)[serialNumber]->GetSample();
        double line = (*p_editPoint)[serialNumber]->GetLine();
        int x, y;
        vp->cubeToViewport(samp, line, x, y);
        // set point marker red
        QBrush brush(Qt::red);
        // set point marker bold - line width 2
        QPen pen(brush, 2);
        // draw the selected point in each image last so it's on top of the rest of the points
        painter->setPen(pen);
        painter->drawLine(x - 5, y, x + 5, y);
        painter->drawLine(x, y - 5, x, y + 5);
      }
    }
  }


#if 0
  /** 
   * Draw all measurments which are on this viewPort
   * @param vp Viewport whose measurements will be drawn
   * @param painter
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                          namespace std"
   *   @history 2010-06-08 Jeannie Walldren - Fixed bug that was causing ignored
   *                          measures not be drawn as yellow unless QnetTool was
   *                          open
   *   @history 2010-07-01 Jeannie Walldren - Modified to draw points selected in
   *                          QnetTool last so they lay on top of all other points
   *                          in the image.
   *   @history 2010-10-28 Tracie Sucharski - Cleaned up code.
   *   @history 2010-11-02 Tracie Sucharski - Added ground point cabability,
   */
  void QnetTool::drawAllMeasurments (MdiCubeViewport *vp,QPainter *painter) {
    // Without a controlnetwork there are no points
    if (g_controlNetwork == 0) return;

    bool isGround = false;
    if (vp->cube() == p_groundCube) isGround = true;
    // Don't show the measurments on cubes not in the serial number list
    // TODO: Should we show them anyway
    // TODO: Should we add the SN to the viewPort
    iString serialNumber = SerialNumber::Compose(*vp->cube(),true);
    if (!g_serialNumberList->HasSerialNumber(serialNumber) && !isGround) return;
    // loop through all points in the control net
    for (int i=0; i<g_controlNetwork->GetNumPoints(); i++) {
      drawMeasures (vp, painter, *((*g_controlNetwork)[i]));
    }
    //drawMeasures (vp, painter, *p_editPoint);

    // If ground cube, return since there are probably not a dense # of ground
    // pts and don't need to redraw current edit point.
    if (isGround) return;

    // if QnetTool is open, redraw selected point so it is on top.  This is
    // done because the selected point was hidden under a mass of crosshairs
    // for dense networks.
    if (p_editPoint != NULL) {
      // and the selected point is in the image, 
      if (p_editPoint->HasSerialNumber(serialNumber)) {
        // find the measurement
        double samp = (*p_editPoint)[serialNumber]->GetSample();
        double line = (*p_editPoint)[serialNumber]->GetLine();
        int x,y;
        vp->cubeToViewport(samp,line,x,y);
        // set point marker red
        QBrush brush(Qt::red);
        // set point marker bold - line width 2
        QPen pen(brush, 2);
        // draw the selected point in each image last so it's on top of the rest of the points
        painter->setPen(pen);
        painter->drawLine(x-5,y,x+5,y);
        painter->drawLine(x,y-5,x,y+5);
      }
    }
  }


  void QnetTool::drawMeasures (MdiCubeViewport *vp,QPainter *painter,ControlPoint &point) {

    //  IF cubes are opened before a point has been loaded into editor, this
    //  point could be NULL.
    if (&point == NULL) return;

    // Don't show the measurments on cubes not in the serial number list
    // TODO: Should we show them anyway
    // TODO: Should we add the SN to the viewPort
    string serialNumber = SerialNumber::Compose(*vp->cube(),true);

    double samp, line;

    // Does cube exist in ControlPoint
    if (!point.HasSerialNumber(serialNumber)) return;

    samp = point[serialNumber]->GetSample();
    line = point[serialNumber]->GetLine();
    int x,y;
    vp->cubeToViewport(samp,line,x,y);

    // Determine pen color
    // if the point or measure is ignored    
    if (point.IsIgnored() || (!point.IsIgnored() && point[serialNumber]->IsIgnored())) {
      painter->setPen(QColor(255,255,0)); // set point marker yellow
    }
    // Point & Measure are valid (not ignored) and it's a ground point
    else if (point.GetType() == ControlPoint::Ground) {
      painter->setPen(Qt::magenta);// set point marker magenta
    }
    else {
      painter->setPen(Qt::green); // set all other point markers green
    }

    // draw points
    painter->drawLine(x-5,y,x+5,y);
    painter->drawLine(x,y-5,x,y+5);
  }
#endif



  /**
   * Allows user to set a new template file.
   * @author 2008-12-10 Jeannie Walldren 
   * @internal 
   *   @history 2008-12-10 Jeannie Walldren - Original Version
   *

  void QnetTool::setTemplateFile() {
    p_pointEditor->setTemplateFile();
  }*/


  bool QnetTool::okToContinue() {

    if (p_templateModified) {
      int r = QMessageBox::warning(p_qnetTool, tr("OK to continue?"),
          tr("The currently opened registration template has been modified.\n"
          "Save changes?"),
          QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
          QMessageBox::Yes);
          
      if (r == QMessageBox::Yes)
        saveTemplateFileAs();
      else if (r == QMessageBox::Cancel)
        return false;
    }
    
    return true;
  }
  
  
  /**
   * prompt user for a registration template file to open.  Once the file is
   * selected, loadTemplateFile is called to update the current template file
   * being used.
   */
  void QnetTool::openTemplateFile() {
  
    if (!okToContinue())
      return;
    
    QString filename = QFileDialog::getOpenFileName(p_qnetTool,
        "Select a registration template", ".",
        "Registration template files (*.def *.pvl);;All files (*)");
        
    if (filename.isEmpty())
      return;
      
    if (p_pointEditor->setTemplateFile(filename)) {
      loadTemplateFile(filename);
    }
  }
  
  
  /**
   * Updates the current template file being used.
   *
   * @param fn The file path of the new template file
   */
  void QnetTool::loadTemplateFile(QString fn) {
  
    QFile file(QString::fromStdString(Filename((iString) fn).Expanded()));
    if (!file.open(QIODevice::ReadOnly)) {
      QString msg = "Failed to open template file \"" + fn + "\"";
      QMessageBox::warning(p_qnetTool, "IO Error", msg);
      return;
    }
    
    QTextStream stream(&file);
    p_templateEditor->setText(stream.readAll());
    file.close();
    
    QScrollBar * sb = p_templateEditor->verticalScrollBar();
    sb->setValue(sb->minimum());
    
    p_templateModified = false;
    p_saveTemplateFile->setEnabled(false);
    p_templateFilenameLabel->setText("Template File: " + fn);
  }
  
  
  //! called when the template file is modified by the template editor
  void QnetTool::setTemplateModified() {
    p_templateModified = true;
    p_saveTemplateFile->setEnabled(true);
  }
  
  
  //! save the file opened in the template editor
  void QnetTool::saveTemplateFile() {
  
    if (!p_templateModified)
      return;
    
    QString filename = QString::fromStdString(
        p_pointEditor->templateFilename());
        
    writeTemplateFile(filename);
  }
  
  
  //! save the contents of template editor to a file chosen by the user
  void QnetTool::saveTemplateFileAs() {
  
    QString filename = QFileDialog::getSaveFileName(p_qnetTool,
        "Save registration template", ".",
        "Registration template files (*.def *.pvl);;All files (*)");
        
    if (filename.isEmpty())
      return;
  
    writeTemplateFile(filename);
  }
  
  
  /**
   * write the contents of the template editor to the file provided.
   *
   * @param fn The filename to write to
   */
  void QnetTool::writeTemplateFile(QString fn) {
  
    QString contents = p_templateEditor->toPlainText();
    
    // catch errors in Pvl format when populating pvl object
    stringstream ss;
    ss << contents.toStdString();
    try {
      Pvl pvl;
      ss >> pvl;
    }
    catch(iException &e) {
      QString message = e.Errors().c_str();
      e.Clear();
      QMessageBox::warning(p_qnetTool, "Error", message);
      return;
    }
    
    QString expandedFilename(QString::fromStdString(
        Filename((iString) fn).Expanded()));
    
    QFile file(expandedFilename);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
      QString msg = "Failed to save template file to \"" + fn + "\"\nDo you "
          "have permission?";
      QMessageBox::warning(p_qnetTool, "IO Error", msg);
      return;
    }
    
    // now save contents
    QTextStream stream(&file);
    stream << contents;
    file.close();
    
    if (p_pointEditor->setTemplateFile(fn)) {
      p_templateModified = false;
      p_saveTemplateFile->setEnabled(false);
      p_templateFilenameLabel->setText("Template File: " + fn);
    }
  }


  /**
   * Allows the user to view the template file that is currently 
   * set. 
   *  
   * @author 2008-12-10 Jeannie Walldren 
   * @internal 
   *   @history 2008-12-10 Jeannie Walldren - Original Version
   *   @history 2008-12-10 Jeannie Walldren - Added "Isis::" namespace to
   *                          PvlEditDialog reference and changed
   *                          registrationDialog from pointer to object
   *   @history 2008-12-15 Jeannie Walldren - Added QMessageBox warning in case
   *                          Template File cannot be read.
   */
  void QnetTool::viewTemplateFile() {
    try{
      // Get the template file from the ControlPointEditor object
      Pvl templatePvl(p_pointEditor->templateFilename());
      // Create registration dialog window using PvlEditDialog class
      // to view and/or edit the template
      PvlEditDialog registrationDialog(templatePvl);
      registrationDialog.setWindowTitle("View or Edit Template File: " 
                                         + QString::fromStdString(templatePvl.Filename()));
      registrationDialog.resize(550,360);
      registrationDialog.exec();
    }
    catch (iException &e) {
      QString message = e.Errors().c_str();
      e.Clear ();
      QMessageBox::information((QWidget *)parent(),"Error",message);
    }
  }



  /**
   * Slot which calls ControlPointEditor slot to save chips 
   *  
   * @author 2009-03-17 Tracie Sucharski
   */

  void QnetTool::saveChips() {
    p_pointEditor->saveChips();
  }


  void QnetTool::showHideTemplateEditor() {
  
    if (!p_templateEditorWidget)
      return;
    
    p_templateEditorWidget->setVisible(!p_templateEditorWidget->isVisible());
  }



  /**
   * Update the current editPoint information in the Point Editor labels
   * 
   * @author 2011-05-05 Tracie Sucharski
   *  
   * @TODO  Instead of a single method, should slots be separate for each 
   *        updated point parameter, ie. ignore, editLock, apriori, etc.
   *        This is not robust, if other point attributes are changed outside
   *        of QnetTool, this method will need to be updated.
   *       *** THIS METHOD SHOULD GO AWAY WHEN CONTROLpOINTEDITOR IS INCLUDED
   *           IN QNET ***
   */
  void QnetTool::updatePointInfo(QString pointId) {
    if (p_editPoint == NULL) return;
    if (pointId != p_editPoint->GetId()) return;
    //  The edit point has been changed by SetApriori, so p_editPoint needs
    //  to possibly update some values.  Need to retain measures from p_editPoint
    //  because they might have been updated, but not yet saved to the network
    //   ("Save Point").
    ControlPoint *updatedPoint = g_controlNetwork->GetPoint(pointId);
    p_editPoint->SetEditLock(updatedPoint->IsEditLocked());
    p_editPoint->SetIgnored(updatedPoint->IsIgnored());
    p_editPoint->SetAprioriSurfacePoint(updatedPoint->GetAprioriSurfacePoint());

    //  Set EditLock box correctly
    p_lockPoint->setChecked(p_editPoint->IsEditLocked());

    //  Set ignore box correctly
    p_ignorePoint->setChecked(p_editPoint->IsIgnored());

    updateSurfacePointInfo();

  }




  /**
   * Refresh all necessary widgets in QnetTool including the PointEditor and 
   * CubeViewports. 
   *  
   * @author 2008-12-09 Tracie Sucharski 
   *  
   * @internal 
   * @history 2010-12-15 Tracie Sucharski - Before setting p_editPoint to NULL, 
   *                        release memory.  TODO: Why is the first if statement
   *                        being done???
   *  
   */
  void QnetTool::refresh() {
    

    //  Check point being edited, make sure it still exists, if not ???
    //  Update ignored checkbox??
    if (p_editPoint != NULL) {
      try {
        QString id = p_ptIdValue->text().remove("Point ID:  ");
        ControlPoint *pt = g_controlNetwork->GetPoint(id);
        pt = 0;
      }
      catch (iException &e) {
        delete p_editPoint;
        p_editPoint = NULL;
        p_qnetTool->setShown(false);
      }
    }

    if (p_editPoint == NULL) {
      paintAllViewports("");
    }
    else {
      paintAllViewports(p_editPoint->GetId());
    }
  }


  /**
   * Emits a signal to displays the Navigation window.  This signal is connected 
   * to QnetNavTool. 
   *  
   * @internal 
   *   @history 2010-07-01 Jeannie Walldren - Original version 
   */
  void QnetTool::showNavWindow(bool checked){
    emit showNavTool();
  }

  /**
   * This method creates the widgets for the tool bar.  A "Show Nav Tool" button
   * is created so that the navigation tool may be reopened if it has been closed.
   *
   * @param parent The parent QStackedWidget
   * @return @b QWidget*
   *
   * @internal
   * @todo Find a way to enable Show Nav Button even when there are no images open
   *       in the main window.
   *  
   * @history 2010-07-01 Jeannie Walldren - Original version. 
   */
  QWidget *QnetTool::createToolBarWidget (QStackedWidget *parent) {
    QWidget *hbox = new QWidget(parent);

    QToolButton *showNavToolButton = new QToolButton();
    showNavToolButton->setText("Show Nav Tool");
    showNavToolButton->setToolTip("Shows the Navigation Tool Window");
    QString text =
    "<b>Function:</b> This button will bring up the Navigation Tool window that allows \
     the user to view, modify, ignore, delete, or filter points and cubes.";
    showNavToolButton->setWhatsThis(text);
    connect(showNavToolButton,SIGNAL(clicked(bool)),this,SLOT(showNavWindow(bool)));

    QHBoxLayout *layout = new QHBoxLayout(hbox);
    layout->setMargin(0);
    layout->addWidget(showNavToolButton);
    layout->addStretch(1);
    hbox->setLayout(layout);
    return hbox;
  }




  /**
   * Open a ground source for selecting fixed points
   * 
   * @author  2009-07-20 Tracie Sucharski
   * 
   * @internal 
   * @history 2011-06-03 Tracie Sucharski - Make sure edit point valid before 
   *                        loading. 
   */
  void QnetTool::openGround() {

    QString filter = "Isis cubes (*.cub *.cub.*);;";
    filter += "Detached labels (*.lbl);;";
    filter += "All (*)";
    QString ground = QFileDialog::getOpenFileName((QWidget*)parent(),
                                                  "Open ground source",
                                                  ".",
                                                  filter);
    if (ground.isEmpty()) return;

    //  If a ground source is already opened, check if new is same as old.  If so,
    //  simply set as active window.  If new ground source, close old,
    //  open new and add new to serial number list if not already in list.
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (p_groundOpen) {
      //  See if ground source is already opened in a cubeviewport.  If so, simply
      //  activate the viewport and return.
      MdiCubeViewport *vp;
      for (int i=0; i<(int)cubeViewportList()->size(); i++) {
        vp = (*(cubeViewportList()))[i];
        if (vp->cube()->getFilename() == ground.toStdString()) {
          g_vpMainWindow->workspace()->setActiveSubWindow((QMdiSubWindow *)vp->parentWidget());
          QApplication::restoreOverrideCursor();
          return;
        }
      }
      //  ....otherwise if new ground source, close old.  We only want a single
      //  ground source opened at once.  Delete old ground source from left/right
      //  combo if it's there, and delete from serial number list.
      clearGroundSource ();

    }

    p_groundSourceFile = ground;
    p_groundCube = new Cube();
    try {
      p_groundCube->open(ground.toStdString());
      p_groundGmap = new UniversalGroundMap(*p_groundCube);
      p_groundFile = Filename(p_groundCube->getFilename()).Name().c_str();
    }
    catch (iException &e) {
      QString message = e.Errors().c_str();
      QMessageBox::critical((QWidget *)parent(),"Error",message);
      e.Clear();
      if (p_groundCube) {
        delete p_groundCube;
        p_groundCube = NULL;      }
      if (p_groundGmap) {
        delete p_groundGmap;
        p_groundGmap = NULL;
      }
      QApplication::restoreOverrideCursor();
      // Re-load point w/o ground source
      loadPoint();
      return;
    }
    p_groundOpen = true;

    g_vpMainWindow->workspace()->addCubeViewport(p_groundCube);

    // If ground source cube not in serial number list, add
    string serialNumber = SerialNumber::Compose(*p_groundCube,true);
    p_groundSN = serialNumber;
    if (!g_serialNumberList->HasSerialNumber(serialNumber)) {
      g_serialNumberList->Add(ground.toStdString(),true);
    }

    //  Determine file type of ground for setting AprioriSurfacePointSource
    //  and AprioriRadiusSource.
    if (p_groundCube->hasTable("ShapeModelStatistics")) {
      p_groundSurfacePointSource = ControlPoint::SurfacePointSource::Basemap;
      if (!p_demOpen) {
        p_groundRadiusSource = ControlPoint::RadiusSource::DEM;
        p_radiusSourceFile = ground;
      }
    }
    // Is this a level 1 or level 2?
    else {
      Camera *camera = NULL;
      Projection *projection = NULL;
      try {
        projection = ProjectionFactory::CreateFromCube(*p_groundCube);
        p_groundSurfacePointSource = ControlPoint::SurfacePointSource::Basemap;
        // TODO  Add Basemap to ControlPoint::RadiusSource
        if (!p_demOpen) {
          // TODO p_groundRadiusSource = ControlPoint::RadiusSource::Basemap;
          p_groundRadiusSource = ControlPoint::RadiusSource::Ellipsoid;
          PvlGroup mapping = p_groundCube->getGroup("Mapping");
          p_demFile = QString::fromStdString(mapping ["EquatorialRadius"])
                         + ", " + QString::fromStdString(mapping ["PolarRadius"]);
          //  
          p_radiusSourceFile = "";
        }
      }
      catch (iException &e) {
        e.Clear();
        try {
          camera = CameraFactory::Create(*(p_groundCube->getLabel()));
          p_groundSurfacePointSource = ControlPoint::SurfacePointSource::Reference;
          if (!p_demOpen) {
            //  If level 1, determine the shape model
            PvlGroup kernels = p_groundCube->getGroup("Kernels");
            QString shapeFile = QString::fromStdString(kernels ["ShapeModel"]);
            if (shapeFile.contains("dem")) {
              p_groundRadiusSource = ControlPoint::RadiusSource::DEM;
              p_radiusSourceFile = shapeFile;
              //  Open shape file for reading radius later
              initDem(shapeFile);
            }
            else {
              p_groundRadiusSource = ControlPoint::RadiusSource::Ellipsoid;
              p_demFile = "Ellipsoid";
              //  Find pck file from Kernels group
              p_radiusSourceFile = (string) kernels["TargetAttitudeShape"];
            }
          }
        }
        catch (iException &e) {
          QString message = "Cannot create either Camera or Projections ";
          message += "for the ground source file.  Check the validity of the ";
          message += " cube labels.  The cube must either be projected or ";
          message += " run through spiceinit.";
          QMessageBox::critical((QWidget *)parent(),"Error",message);
          //  Clear out everything relating to ground source
          clearGroundSource ();
          QApplication::restoreOverrideCursor();
          return;
        }
      }
    }

    if (p_editPoint != NULL &&
        (p_editPoint->GetType() != ControlPoint::Free)) loadPoint();
    p_groundFilenameLabel->setText("Ground Source File:  " + p_groundFile);
    p_radiusFilenameLabel->setText("Radius Source File:  " + p_demFile);

    QApplication::restoreOverrideCursor();
  }


  /**
   * Open a DEM for ground source radii
   * 
   * @author  2011-01-18 Tracie Sucharski
   * 
   * 
   */
  void QnetTool::openDem() {

      QString filter = "Isis cubes (*.cub *.cub.*);;";
      filter += "Detached labels (*.lbl);;";
      filter += "All (*)";
      QString dem = QFileDialog::getOpenFileName((QWidget*)parent(),
                                                  "Open DEM",
                                                  ".",
                                                  filter);
      if (dem.isEmpty()) return;

      initDem(dem);

  }


  void QnetTool::initDem (QString demFile) {

      //  If a DEM is already opened, check if new is same as old. If new,
      //  close old, open new.
      QApplication::setOverrideCursor(Qt::WaitCursor);
      if (p_demOpen) {
        if (p_demFile == demFile) {
          QApplication::restoreOverrideCursor();
          return;
        }

        p_demCube->close();
        p_demOpen = false;
        delete p_demCube;
        p_demCube = NULL;
        p_demFile.clear();

      }

      p_demCube = new Cube();

      try {
        p_demCube->open(demFile.toStdString());
        p_demFile = Filename(p_demCube->getFilename()).Name().c_str();
      } catch (iException &e) {
        QString message = e.Errors().c_str();
        QMessageBox::critical((QWidget *)parent(),"Error",message);
        e.Clear();
        if (p_demCube) {
          delete p_demCube;
          p_demCube = NULL;
        }
        QApplication::restoreOverrideCursor();
        return;
      }
      p_demOpen = true;

      //  Make sure this is a dem
      if (!p_demCube->hasTable("ShapeModelStatistics")) {
        QString message = p_demFile + " is not a DEM.";
        QMessageBox::critical((QWidget *)parent(),"Error",message);
        p_demCube->close();
        p_demOpen = false;
        delete p_demCube;
        p_demCube = NULL;
        p_demFile.clear();
        QApplication::restoreOverrideCursor();
        return;
      }
      p_groundRadiusSource = ControlPoint::RadiusSource::DEM;
      p_groundFilenameLabel->setText("Ground Source File:  " + p_groundFile);
      p_radiusFilenameLabel->setText("Radius Source File:  " + p_demFile);
      p_radiusSourceFile = demFile;

      QApplication::restoreOverrideCursor();
  }



  void QnetTool::clearGroundSource () {

    //  If the loaded point is a fixed point, see if there is a temporary measure
    //  holding the coordinate information for the currentground source. If so,
    //  delete this measure.
    if (p_editPoint->GetType() != ControlPoint::Free &&
        p_editPoint->HasSerialNumber(p_groundSN)) {
      p_editPoint->Delete(p_groundSN);
    }

    p_leftCombo->removeItem(p_leftCombo->findText(p_groundFile));
    p_rightCombo->removeItem(p_rightCombo->findText(p_groundFile));

    //  Close viewport containing ground source
    MdiCubeViewport *vp;
    for (int i=0; i<(int)cubeViewportList()->size(); i++) {
      vp = (*(cubeViewportList()))[i];
      if (vp->cube() == p_groundCube) {
        vp->parentWidget()->parentWidget()->close();
        QApplication::processEvents();
        break;
      }
    }
    //  If we could not find the ground source in the open viewports, user might
    //  have closed the viewport , reset ground source variables and re-open.
    p_groundOpen = false;
//    p_groundCube->close();
//    delete p_groundCube;
    p_groundCube = NULL;
    p_groundFile.clear();

    if (p_groundGmap) {
      delete p_groundGmap;
      p_groundGmap = NULL;
    }

    // Remove from serial number list
    g_serialNumberList->Delete(p_groundSN);
  }




  /**
   * Turn "Save Point" button text to red
   *
   * @author 2011-06-14 Tracie Sucharski
   */
  void QnetTool::colorizeSaveButton() {

    QColor qc = Qt::red;
    QPalette p = p_savePoint->palette();
    p.setColor(QPalette::ButtonText,qc);
    p_savePoint->setPalette(p);

  }



  /**
   * Check for implicitly locked measure in p_editPoint.  If point is Locked, 
   * and this measure is the reference, it is implicity Locked.
   * Because measure is a copy, the ControlPoint::IsEditLocked() which checks 
   * for implicit Lock on Reference measures does not work because there is 
   * not a parent point. 
   *  
   * @param[in] serialNumber (QString)   Serial number of measure to be checked 
   *  
   *  
   * @author 2011-07-06 Tracie Sucharski 
   */
  bool QnetTool::IsMeasureLocked (iString serialNumber) {

    if (p_editPoint == NULL) return false;

    // Reference implicitly editLocked
    if (p_editPoint->IsEditLocked() && p_editPoint->IsReferenceExplicit() &&
        (p_editPoint->GetReferenceSN() == serialNumber)) {
      return true;
    }
    // Return measures explicit editLocked value
    else {
      return p_editPoint->GetMeasure(serialNumber)->IsEditLocked();
    }

  }

}

