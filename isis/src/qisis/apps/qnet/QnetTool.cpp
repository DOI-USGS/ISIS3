#include "QnetTool.h"

#include <sstream>
#include <vector>

#include <QAction>
#include <QBrush>
#include <QCheckBox>
#include <QComboBox>
#include <QFile>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QObject>
#include <QPainter>
#include <QPoint>
#include <QPushButton>
#include <QSplitter>
#include <QStackedWidget>
#include <QTextStream>
#include <QString>
#include <QWidget>

//#include "AutoReg.h"
//#include "AutoRegFactory.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "ControlPointEdit.h"
#include "Filename.h"
#include "iString.h"
#include "iException.h"
#include "MdiCubeViewport.h"
#include "Pvl.h"
#include "PvlEditDialog.h"
#include "QnetDeletePointDialog.h"
#include "QnetHoldPointDialog.h"
#include "QnetNewMeasureDialog.h"
#include "QnetNewPointDialog.h"
#include "SerialNumber.h"
#include "ToolPad.h"
#include "qnet.h"

using namespace Qisis::Qnet;
using namespace Isis;
using namespace std;


namespace Qisis {
  
  const int CHIPVIEWPORT_WIDTH = 310;


  /**
   * Consructs the Qnet Tool window
   *
   * @param parent Pointer to the parent widget for the Qnet tool
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers to null.
   *
   */
  QnetTool::QnetTool(QWidget *parent) : Qisis::Tool(parent) {

#if 0
    p_createPoint = new QAction(parent);
    p_createPoint->setShortcut(Qt::Key_C);
    connect(p_createPoint, SIGNAL(triggered()), this, SLOT(createPoint()));

    p_modifyPoint = new QAction(parent);
    p_modifyPoint->setShortcut(Qt::Key_M);
//    connect(p_modifyPoint,SIGNAL(triggered()),this,SLOT(modifyPoint()));

    p_deletePoint = new QAction(parent);
    p_deletePoint->setShortcut(Qt::Key_D);
    connect(p_deletePoint, SIGNAL(triggered()), this, SLOT(deletePoint()));
#endif
    p_leftCube = NULL;
    p_rightCube = NULL;
    p_controlPoint = NULL;
    p_createPoint = NULL;
    p_modifyPoint = NULL;
    p_deletePoint = NULL;
    p_mw = NULL;
    p_pointEditor = NULL;
    p_ptIdValue = NULL;
    p_numMeasures = NULL;
    p_ignorePoint = NULL;
    p_holdPoint = NULL;
    p_groundPoint = NULL;
    p_leftMeasureType = NULL;
    p_leftSampError = NULL;
    p_leftLineError = NULL;
    p_leftGoodness = NULL;
    p_rightMeasureType = NULL;
    p_rightSampError = NULL;
    p_rightLineError = NULL;
    p_rightGoodness = NULL;
    p_ignoreLeftMeasure = NULL;
    p_ignoreRightMeasure = NULL;
    p_leftCombo = NULL;
    p_rightCombo = NULL;
    p_leftMeasure = NULL;
    p_rightMeasure = NULL;
    p_holdPointDialog = NULL;
    
    p_templateModified = false;
    
    createQnetTool(parent);
  }

  
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
    connect(p_pointEditor, SIGNAL(pointSaved()), this, SLOT(pointSaved()));
    
    QPushButton * addMeasure = new QPushButton("Add Measure(s) to Point");
    connect(addMeasure, SIGNAL(clicked()), this, SLOT(addMeasure()));
    QHBoxLayout * addMeasureLayout = new QHBoxLayout;
    addMeasureLayout->addWidget(addMeasure);
    addMeasureLayout->addStretch();
    
    QVBoxLayout * centralLayout = new QVBoxLayout;
    centralLayout->addWidget(createTopSplitter());
    centralLayout->addStretch();
    centralLayout->addWidget(p_pointEditor);
    centralLayout->addLayout(addMeasureLayout);
    
    QWidget * centralWidget = new QWidget;
    centralWidget->setLayout(centralLayout);
    p_qnetTool->setCentralWidget(centralWidget);
    
    connect(this, SIGNAL(editPointChanged(string)),
            this, SLOT(paintAllViewports(string)));
  }
  
  
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
  
  
  QGroupBox * QnetTool::createControlPointGroupBox() {
  
    // create left vertical layout
    p_ptIdValue = new QLabel;
    p_numMeasures = new QLabel;
    QVBoxLayout * leftLayout = new QVBoxLayout;
    leftLayout->addWidget(p_ptIdValue);
    leftLayout->addWidget(p_numMeasures);
    
    // create right vertical layout's top layout
    p_ignorePoint = new QCheckBox("Ignore Point");
    connect(p_ignorePoint, SIGNAL(toggled(bool)),
            this, SLOT(setIgnorePoint(bool)));
    connect(this, SIGNAL(ignorePointChanged()),
            p_ignorePoint, SLOT(toggle()));
    p_holdPoint = new QCheckBox("Hold Point");
    connect(p_holdPoint, SIGNAL(toggled(bool)), this, SLOT(setHoldPoint(bool)));
    QHBoxLayout * rightTopInnerLayout = new QHBoxLayout;
    rightTopInnerLayout->addWidget(p_ignorePoint);
    rightTopInnerLayout->addWidget(p_holdPoint);
    
    // create right vertical layout's bottom layout
    p_groundPoint = new QCheckBox("Ground Point");
    p_groundPoint->setEnabled(false);
    //????  connect(p_groundPoint,SIGNAL(toggled(bool)),this,SLOT(setGroundPoint(bool)));
    QHBoxLayout * rightBottomInnerLayout = new QHBoxLayout;
    rightBottomInnerLayout->addWidget(p_groundPoint);
    rightBottomInnerLayout->addStretch();
    
    // create right vertical layout
    QVBoxLayout * rightLayout = new QVBoxLayout;
    rightLayout->addLayout(rightTopInnerLayout);
    rightLayout->addLayout(rightBottomInnerLayout);
    
    QHBoxLayout * topLayout = new QHBoxLayout;
    topLayout->addLayout(leftLayout);
    topLayout->addStretch();
    topLayout->addLayout(rightLayout);
    
    p_templateFilenameLabel = new QLabel("Template File: " +
        QString::fromStdString(p_pointEditor->templateFilename()));
    
    QVBoxLayout * mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(p_templateFilenameLabel);
    
    // create the groupbox
    QGroupBox * groupBox = new QGroupBox("Control Point");
    groupBox->setLayout(mainLayout);
    
    return groupBox;
  }
  
  
  QGroupBox * QnetTool::createLeftMeasureGroupBox() {
  
    p_leftCombo = new QComboBox;
    p_leftCombo->view()->installEventFilter(this);
    connect(p_leftCombo, SIGNAL(activated(int)),
            this, SLOT(selectLeftMeasure(int)));
    p_ignoreLeftMeasure = new QCheckBox("Ignore Measure");
    connect(p_ignoreLeftMeasure, SIGNAL(toggled(bool)),
            this, SLOT(setIgnoreLeftMeasure(bool)));
    connect(this, SIGNAL(ignoreLeftChanged()),
            p_ignoreLeftMeasure, SLOT(toggle()));
    p_leftMeasureType = new QLabel;
    p_leftSampError = new QLabel();
    p_leftLineError = new QLabel();
    p_leftGoodness = new QLabel();
    
    QVBoxLayout * leftLayout = new QVBoxLayout;
    leftLayout->addWidget(p_leftCombo);
    leftLayout->addWidget(p_ignoreLeftMeasure);
    leftLayout->addWidget(p_leftMeasureType);
    leftLayout->addWidget(p_leftSampError);
    leftLayout->addWidget(p_leftLineError);
    leftLayout->addWidget(p_leftGoodness);
    
    QGroupBox * leftGroupBox = new QGroupBox("Left Measure");
    leftGroupBox->setLayout(leftLayout);
    
    return leftGroupBox;
  }
  
  
  QGroupBox * QnetTool::createRightMeasureGroupBox() {
  
    // create widgets for the right groupbox
    p_rightCombo = new QComboBox;
    p_rightCombo->view()->installEventFilter(this);
    connect(p_rightCombo, SIGNAL(activated(int)),
            this, SLOT(selectRightMeasure(int)));
    p_ignoreRightMeasure = new QCheckBox("Ignore Measure");
    connect(p_ignoreRightMeasure, SIGNAL(toggled(bool)),
            this, SLOT(setIgnoreRightMeasure(bool)));
    connect(this, SIGNAL(ignoreRightChanged()),
            p_ignoreRightMeasure, SLOT(toggle()));
    p_rightMeasureType = new QLabel;
    p_rightSampError = new QLabel();
    p_rightLineError = new QLabel();
    p_rightGoodness = new QLabel();
    
    // create right groupbox
    QVBoxLayout * rightLayout = new QVBoxLayout;
    rightLayout->addWidget(p_rightCombo);
    rightLayout->addWidget(p_ignoreRightMeasure);
    rightLayout->addWidget(p_rightMeasureType);
    rightLayout->addWidget(p_rightSampError);
    rightLayout->addWidget(p_rightLineError);
    rightLayout->addWidget(p_rightGoodness);
    
    QGroupBox * rightGroupBox = new QGroupBox("Right Measure");
    rightGroupBox->setLayout(rightLayout);
    
    return rightGroupBox;
  }
  
  
  void QnetTool::createTemplateEditorWidget() {
    
    QToolBar * toolBar = new QToolBar("Template Editor ToolBar");
    toolBar->addAction(p_openTemplateFile);
    toolBar->addSeparator();
    toolBar->addAction(p_saveTemplateFile);
    toolBar->addAction(p_saveTemplateFileAs);
  
    p_templateEditor = new QTextEdit;
    connect(p_templateEditor, SIGNAL(textChanged()), this,
        SLOT(setTemplateModified()));
  
    QVBoxLayout * mainLayout = new QVBoxLayout;
    mainLayout->addWidget(toolBar);
    mainLayout->addWidget(p_templateEditor);
  
    p_templateEditorWidget = new QWidget;
    p_templateEditorWidget->setLayout(mainLayout);
  }
  


  /**
   * This method is connected with the pointSaved() signal from
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
   */
  void QnetTool::pointSaved() {
    if(p_controlPoint->Ignore()) {
      switch(QMessageBox::question((QWidget *)parent(),
                                   "Qnet Tool Save Point",
                                   "You are saving changes to an ignored point.  Do you want to set Ignore = False on the point and both measures?",
                                   "&Yes", "&No", 0, 0)) {
        case 0: // Yes was clicked or Enter was pressed, set Ignore=false for the point and measures and save point
          p_controlPoint->SetIgnore(false);
          emit ignorePointChanged();
          if(p_leftMeasure->Ignore()) {
            p_leftMeasure->SetIgnore(false);
            emit ignoreLeftChanged();
          }
          if(p_rightMeasure->Ignore()) {
            p_rightMeasure->SetIgnore(false);
            emit ignoreRightChanged();
          }
        case 1: // No was clicked, keep Ignore=true and save point
          break;

      }
    }
    if(p_rightMeasure->Ignore()) {
      switch(QMessageBox::question((QWidget *)parent(),
                                   "Qnet Tool Save Point",
                                   "You are saving changes to an ignored measure.  Do you want to set Ignore = False on the right measure?",
                                   "&Yes", "&No", 0, 0)) {
        case 0: // Yes was clicked, set Ignore=false for the right measure and save point
          p_rightMeasure->SetIgnore(false);
          emit ignoreRightChanged();
        case 1: // No was clicked, keep Ignore=true and save point
          break;

      }
    }
    // Check if ControlPoint has reference measure, if reference Measure is
    // not the same measure that is on the left chip viewport, set left
    // measure as reference.
    if(p_controlPoint->HasReference()) {
      Isis::ControlMeasure *refMeasure =
        &(*p_controlPoint)[p_controlPoint->ReferenceIndex()];
      if(refMeasure != p_leftMeasure) {
        switch(QMessageBox::question((QWidget *)parent(),
                                     "Qnet Tool Save Point",
                                     "This point already contains a reference measure.  Would you like to replace it with the measure on the left?",
                                     "&Yes", "&No", 0, 0)) {
          case 0: // Yes was clicked or Enter was pressed, replace reference
            refMeasure->SetReference(false);
            p_leftMeasure->SetReference(true);
          case 1: // No was clicked, keep original reference
            break;

        }
      }
    }
    else {
      p_leftMeasure->SetReference(true);
    }

    // emit signal so the nav tool can update edit point
    emit editPointChanged(p_controlPoint->Id());
    // emit a signal to alert user to save when exiting 
    emit netChanged();
  }


  void QnetTool::createActions() {
    p_saveNet = new QAction(QIcon(":saveAs"), "Save Control Network &As...",
        p_qnetTool);
    p_saveNet->setStatusTip("Save current control network to chosen file");
    QString whatsThis = "<b>Function:</b> Saves the current <i>"
        "control network</i> under chosen filename";
    p_saveNet->setWhatsThis(whatsThis);
    connect(p_saveNet, SIGNAL(activated()), this, SLOT(saveNet()));

    p_closeQnetTool = new QAction(QIcon(":close"), "&Close", p_qnetTool);
    p_closeQnetTool->setStatusTip("Close this window");
    p_closeQnetTool->setShortcut(Qt::ALT + Qt::Key_F4);
    whatsThis ="<b>Function:</b> Closes the Qnet Tool window for this point "
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
  void QnetTool::createMenus() {

    QMenu * fileMenu = p_qnetTool->menuBar()->addMenu("&File");
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
   * Set point's "Ignore" keyword to the value of the input
   * parameter.
   * @param ignore Boolean value that determines the Ignore value for this point.
   */
  void QnetTool::setIgnorePoint(bool ignore) {
    if(p_controlPoint != NULL) {
      p_controlPoint->SetIgnore(ignore);
    }
    // emit a signal to alert user to save when exiting 
    emit netChanged();
  }


  /**
   * Sets the "Held" keyword of the control point to the boolean
   * value of the input parameter.
   *
   * @param hold Boolean value to which "Held" keyword will be
   *             set.
   * @internal
   *   @history 2008-12-29 Jeannie Walldren - Added dialog box to allow user to
   *                          determine the lat/lon/radius for this point if set
   *                          to Held=True.
   */
  void QnetTool::setHoldPoint(bool hold) {
    // if control point is already set to value of hold, return
    //    (currently this is unnecessary since this method is
    //     only called if the checkbox is toggled)
    if(hold == p_controlPoint->Held()) {
      return;
    }
    if(!hold) {
      p_controlPoint->SetHeld(false);
    }
    else {
      p_holdPointDialog = new QnetHoldPointDialog;
      p_holdPointDialog->setModal(true);
      p_holdPointDialog->setPoint(*p_controlPoint);
      connect(p_holdPointDialog, SIGNAL(holdPoint(Isis::ControlPoint &)),
              this, SLOT(newHoldPoint(Isis::ControlPoint &)));
      connect(p_holdPointDialog, SIGNAL(holdCancelled()),
              this, SLOT(cancelHoldPoint()));
      p_holdPointDialog->exec();
    }
    // emit a signal to alert user to save when exiting 
    emit netChanged();
  }


  /**
   *
   * Makes a new control point identical to input with Held=True.
   *
   * @param point Control point to be copied
   * @author 2008-12-29 Jeannie Walldren
   * @internal
   *   @history 2008-12-29 Jeannie Walldren - Original Version
   *   @history 2008-12-30 Jeannie Walldren - Replaced reference to
   *                          ignoreChanged() with ignorePointChanged().
   *   @history 2008-12-30 Jeannie Walldren - Removed call to close the Hold
   *                          Point Dialog that caused the reject() command to
   *                          be called.
   */
  void QnetTool::newHoldPoint(Isis::ControlPoint &point) {
    //  If setting as hold or ground point ,make sure point isn't ignored, ground
    if(p_controlPoint->Ignore()) {
      p_controlPoint->SetIgnore(false);
      emit ignorePointChanged();
    }
    p_controlPoint = &point;

    // this line causes hold point dialog to call "reject()"
    // not sure why it was orginally included
    //p_holdPointDialog->close();

  }


  /**
   *  This method sets Held=False and unchecks hold CheckBox if
   *  the "Cancel" button is clicked in the Hold Point Dialog.
   *
   *
   * @internal
   *   @history 2010-06-02 Jeannie Walldren - Original version
   */
  void QnetTool::cancelHoldPoint() {
    p_controlPoint->SetHeld(false);
    p_holdPoint->setChecked(false);
    return;
  }


  /**
   * Sets the "PointType" keyword of the control point.  If ground
   * is true the point type will be set to "Ground".  If ground is
   * false, it will be set to "Tie".
   * @param ground Boolean value that determines whether the PointType will be set
   *               to ground.  If false, PointType will be set to Tie.
   * @author
   * @internal
   */
  void QnetTool::setGroundPoint(bool ground) {

    //  If this slot is called when loading a new point which happens to already be
    //  a ground point, simply return.  I haven't figured out how to set the
    //  state when loading a new point without emitting the toggled signal.
    //  Maybe the ground should not be a checkBox???
    if(ground && p_controlPoint->Type() == Isis::ControlPoint::Ground) return;

    //  if false, turn back Tie
    if(!ground) {
      p_controlPoint->SetType(Isis::ControlPoint::Tie);
    }
    else {
      p_controlPoint->SetType(Isis::ControlPoint::Ground);
    }

  }


  /**
   * Create a new ground point using input point.
   * @param point The new ground control point
   */
  void QnetTool::newGroundPoint(Isis::ControlPoint &point) {

    p_controlPoint = &point;
    p_controlPoint->SetType(Isis::ControlPoint::Ground);

  }


  /**
   * Set the "Ignore" keyword of the measure shown in the left
   * viewport to the value of the input parameter.
   * @param ignore Boolean value that determines the Ignore value for the left 
   *               measure.
   * @internal
   *   @history 2010-01-27 Jeannie Walldren - Fixed bug that resulted in segfault.
   *                          Moved the check whether p_rightMeasure is null
   *                          before the check whether p_rightMeasure equals
   *                          p_leftMeasure.
   */
  void QnetTool::setIgnoreLeftMeasure(bool ignore) {
    if(p_leftMeasure != NULL) p_leftMeasure->SetIgnore(ignore);
    // emit a signal to alert user to save when exiting 
    emit netChanged();

    //  If the right chip is the same as the left chip , update the right
    //  ignore box.
    if(p_rightMeasure != NULL) {
      if(p_rightMeasure->CubeSerialNumber() == p_leftMeasure->CubeSerialNumber()) {
        p_rightMeasure->SetIgnore(ignore);
        p_ignoreRightMeasure->setChecked(ignore);
      }
    }
  }


  /**
   * Set the "Ignore" keyword of the measure shown in the right
   * viewport to the value of the input parameter. 
   * @param ignore Boolean value that determines the Ignore value for the right 
   *               measure.
   * @internal
   *   @history 2010-01-27 Jeannie Walldren - Fixed bug that resulted in segfault.
   *                          Moved the check whether p_leftMeasure is null before
   *                          the check whether p_rightMeasure equals
   *                          p_leftMeasure.
   */
  void QnetTool::setIgnoreRightMeasure(bool ignore) {
    if(p_rightMeasure != NULL) p_rightMeasure->SetIgnore(ignore);
    // emit a signal to alert user to save when exiting 
    emit netChanged();

    //  If the right chip is the same as the left chip , update the right
    //  ignore blox.
    if(p_leftMeasure != NULL) {
      if(p_rightMeasure->CubeSerialNumber() == p_leftMeasure->CubeSerialNumber()) {
        p_leftMeasure->SetIgnore(ignore);
        p_ignoreLeftMeasure->setChecked(ignore);
      }
    }
  }


  /**
   * Signal to save the control net.
   */
  void QnetTool::saveNet() {
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
   * @param pad Tool pad
   * @return @b QAction* Pointer to Tie tool action 
   * @internal
   *   @history 2010-07-01 Jeannie Walldren - Added connection between qnet's
   *                          TieTool button and the showNavWindow() method
   */
  QAction *QnetTool::toolPadAction(ToolPad *pad) {
    QAction *action = new QAction(pad);
    action->setIcon(QPixmap(toolIconDir() + "/stock_draw-connector-with-arrows.png"));
    action->setToolTip("Tie (T)");
    action->setShortcut(Qt::Key_T);
    QObject::connect(action, SIGNAL(activated()), this, SLOT(showNavWindow()));
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
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                          namespace std"
   *
   */
  void QnetTool::mouseButtonRelease(QPoint p, Qt::MouseButton s) {
    MdiCubeViewport *cvp = cubeViewport();
    if(cvp  == NULL) return;
    if(cvp->cursorInside()) QPoint p = cvp->cursorPosition();

    string file = cvp->cube()->Filename();
    string sn = g_serialNumberList->SerialNumber(file);

    double samp, line;
    cvp->viewportToCube(p.x(), p.y(), samp, line);


    if(s == Qt::LeftButton) {
      p_leftFile = file;
      //  Find closest control point in network
      Isis::ControlPoint *point =
        g_controlNetwork->FindClosest(sn, samp, line);
      //  TODO:  test for errors and reality
      if(point == NULL) {
        QString message = "No points exist for editing.  Create points ";
        message += "using the right mouse button.";
        QMessageBox::warning((QWidget *)parent(), "Warning", message);
        return;
      }
      modifyPoint(point);
    }
    else if(s == Qt::MidButton) {
      //  Find closest control point in network
      Isis::ControlPoint *point =
        g_controlNetwork->FindClosest(sn, samp, line);
      //  TODO:  test for errors and reality
      if(point == NULL) {
        QString message = "No points exist for deleting.  Create points ";
        message += "using the right mouse button.";
        QMessageBox::warning((QWidget *)parent(), "Warning", message);
        return;
      }
      deletePoint(point);
    }
    else if(s == Qt::RightButton) {
      p_leftFile = file;
      Isis::Camera *cam = cvp->camera();
      cam->SetImage(samp, line);
      double lat = cam->UniversalLatitude();
      double lon = cam->UniversalLongitude();
      createPoint(lat, lon);
    }
  }


  /** 
   * Finds point files 
   * @param lat Latitude used to find sample/line values
   * @param lon Longitude used to find sample/line values
   * @return @b vector @b <string> Point files found.
   *  
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                          namespace std"
   *
   */
  vector<string> QnetTool::findPointFiles(double lat, double lon) {


    //  Create list of list box of all files highlighting those that
    //  contain the point.
    vector<string> pointFiles;

    //  Initialize camera for all images in control network,
    //  TODO::   Needs to be moved to QnetFileTool.cpp
    Isis::Camera *cam;
    for(int i = 0; i < g_serialNumberList->Size(); i++) {
      cam = g_controlNetwork->Camera(i);
      if(cam->SetUniversalGround(lat, lon)) {
        //  Make sure point is within image boundary
        double samp = cam->Sample();
        double line = cam->Line();
        if(samp >= 1 && samp <= cam->Samples() &&
            line >= 1 && line <= cam->Lines()) {
          pointFiles.push_back(g_serialNumberList->Filename(i));
        }
      }
    }
    return pointFiles;
  }



  /**
   *   Create new control point
   * @param lat Latitude value of control point to be created. 
   * @param lon Longitude value of control point to be created. 
   *   @internal
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
   *
   */
  void QnetTool::createPoint(double lat, double lon) {

    //  TODO:   ADD AUTOSEED OPTION (CHECKBOX?)

    //  Create list of list box of all files highlighting those that
    //  contain the point.
    QStringList pointFiles;

    //  Initialize camera for all images in control network,
    //  TODO::   Needs to be moved to QnetFileTool.cpp
    Isis::Camera *cam;
    for(int i = 0; i < g_serialNumberList->Size(); i++) {
      cam = g_controlNetwork->Camera(i);
      if(cam->SetUniversalGround(lat, lon)) {
        //  Make sure point is within image boundary
        double samp = cam->Sample();
        double line = cam->Line();
        if(samp >= 1 && samp <= cam->Samples() &&
            line >= 1 && line <= cam->Lines()) {
          pointFiles << (iString) g_serialNumberList->Filename(i);
        }
      }
    }

    //  If point is on a single file, print error and return, do not create
    //  point.
    //     ????  Need to allow a point to be created w/single measure.  Display
    //     ????  measure on right or left?
    if(pointFiles.size() == 1) {
      try {
        throw Isis::iException::Message(Isis::iException::User,
                                        "Cannot add point, it only exists on 1 image. Point will not be added to control network.",
                                        _FILEINFO_);
      }
      catch(Isis::iException &e) {
        QString message = e.Errors().c_str();
        QMessageBox::warning((QWidget *)parent(), "Warning", message);
        e.Clear();
        return;
      }
    }

    QnetNewPointDialog *newPointDialog = new QnetNewPointDialog();
    newPointDialog->SetFiles(pointFiles);
    if(newPointDialog->exec()) {
      Isis::ControlPoint *newPoint =
        new Isis::ControlPoint(newPointDialog->ptIdValue->text().toStdString());


      // If this ControlPointId already exists, message box pops up and user is asked to enter a new value.
      if(g_controlNetwork->Exists(*newPoint)) {
        string message = "A ControlPoint with Point Id = [" + newPoint->Id();
        message += "] already exists.  Re-enter Point Id for this ControlPoint.";
        QMessageBox::warning((QWidget *)parent(), "New Point Id", message.c_str());
        pointFiles.clear();
        createPoint(lat, lon);
        return;
      }
      for(int i = 0; i < newPointDialog->fileList->count(); i++) {
        QListWidgetItem *item = newPointDialog->fileList->item(i);
        if(!newPointDialog->fileList->isItemSelected(item)) continue;
        //  Create measure for any file selected
        Isis::ControlMeasure *m = new Isis::ControlMeasure;
        //  Find serial number for this file
        string sn =
          g_serialNumberList->SerialNumber(item->text().toStdString());
        m->SetCubeSerialNumber(sn);
        int camIndex =
          g_serialNumberList->FilenameIndex(item->text().toStdString());
        cam = g_controlNetwork->Camera(camIndex);
        cam->SetUniversalGround(lat, lon);
        m->SetCoordinate(cam->Sample(), cam->Line());
        m->SetType(Isis::ControlMeasure::Estimated);
        m->SetDateTime();
        m->SetChooserName();
        m->SetCamera(cam);
        newPoint->Add(*m);
      }
      //  Add new control point to control network
      g_controlNetwork->Add(*newPoint);
      //  Read newly added point
      p_controlPoint =
        g_controlNetwork->Find(newPointDialog->ptIdValue->text().toStdString());
      //  Load new point in QnetTool
      //p_controlPoint = newPoint;
      loadPoint();
      p_qnetTool->setShown(true);
      p_qnetTool->raise();
      loadTemplateFile(QString::fromStdString(
          p_pointEditor->templateFilename()));

    
      // emit a signal to alert user to save when exiting 
      emit netChanged();
      // emit signal so the nave tool refreshes the list
      emit refreshNavList();
      // emit signal so the nav tool can update edit point
      emit editPointChanged(p_controlPoint->Id());
    }
  }



  /**
   * Delete control point
   * @param point Pointer to control point to be deleted. 
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                           namespace std"
   *   @history 2010-07-12 Jeannie Walldren - Fixed bug by setting control point
   *                          to NULL if removed from the control net and check
   *                          for NULL points before emitting editPointChanged
   *
   */
  void QnetTool::deletePoint(Isis::ControlPoint *point) {

    // Change point in viewport to red 
    // so user can see what point they are about to delete.
    // the nav tool will update edit point
    emit editPointChanged(point->Id());

    p_controlPoint = point;
    loadPoint();

    QnetDeletePointDialog *deletePointDialog = new QnetDeletePointDialog;
    string CPId = p_controlPoint->Id();
    deletePointDialog->pointIdValue->setText(CPId.c_str());
    //  Need all files for this point
    for(int i = 0; i < p_controlPoint->Size(); i++) {
      Isis::ControlMeasure &m = (*p_controlPoint)[i];
      string file = g_serialNumberList->Filename(m.CubeSerialNumber());
      deletePointDialog->fileList->addItem(file.c_str());
    }

    if(deletePointDialog->exec()) {
      //  First see if entire point needs to be deleted
      if(deletePointDialog->deleteAllCheckBox->isChecked()) {
        //  First get rid of deleted point from g_filteredPoints list
        //  need index in control net for pt
        //int i = g_controlNetwork->
        //g_filteredPoints.
        p_qnetTool->setShown(false);
        // remove this point from the control network
        g_controlNetwork->Delete(p_controlPoint->Id());
        p_controlPoint = NULL;
        // emit signal so the nav tool refreshes the list
        emit refreshNavList();
      }
      //  Otherwise, delete measures located on images chosen
      else {
        for(int i = 0; i < deletePointDialog->fileList->count(); i++) {
          QListWidgetItem *item = deletePointDialog->fileList->item(i);
          if(! deletePointDialog->fileList->isItemSelected(item)) continue;

          //  Delete measure from ControlPoint
          p_controlPoint->Delete(i);
        }

        p_leftFile = "";
        loadPoint();
        p_qnetTool->setShown(true);
        p_qnetTool->raise();
        loadTemplateFile(QString::fromStdString(
            p_pointEditor->templateFilename()));
      }
    }

    // emit a signal to alert user to save when exiting 
    emit netChanged();

    // emit signal so the nav tool can update edit point
    if(p_controlPoint != NULL) {
      emit editPointChanged(p_controlPoint->Id());
    }
    else {
      // if the entire point is deleted, update with point Id = ""
      // this signal is connected to QnetTool::paintAllViewports
      // and QnetNavTool::updateEditPoint
      emit editPointChanged("");
    }
  }



  /**
   * Modify control point 
   * @param point Pointer to control point to be modified. 
   *
   * @history 2009-09-15 Tracie Sucharski - Add error check for points
   *                       with no measures.
   */
  void QnetTool::modifyPoint(Isis::ControlPoint *point) {

    //  If no measures, print info and return
    if(point->Size() == 0) {
      QString message = "This point has no measures.";
      QMessageBox::warning((QWidget *)parent(), "Warning", message);
      // update nav list to re-highlight old point
      if(p_controlPoint != NULL) {
        // emit signal so the nav tool can update edit point
        emit editPointChanged(p_controlPoint->Id());
      }
      else {
        // this signal is connected to QnetTool::paintAllViewports 
        // and QnetNavTool::updateEditPoint
        emit editPointChanged("");
      }
      return;
    }
    p_controlPoint = point;

    //  If navTool modfify button pressed, p_leftFile needs to be reset
    //  better way - have 2 slots
    if(sender() != this) p_leftFile = "";
    loadPoint();
    p_qnetTool->setShown(true);
    p_qnetTool->raise();
    loadTemplateFile(QString::fromStdString(
        p_pointEditor->templateFilename()));

    // emit signal so the nav tool can update edit point
    emit editPointChanged(p_controlPoint->Id());
  }

  /**
   * @brief Load point into QnetTool.
   * @internal
   *   @history 2008-11-26  Jeannie Walldren - Added "Number of Measures" to
   *                           QnetTool point information.
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                          namespace std"
   *
   */
  void QnetTool::loadPoint() {

    //  If Ground point print error
//    if(p_controlPoint->Type() == Isis::ControlPoint::Ground) {
//      QString message = "Cannot edit ground points with a single measure at this
//    }

    //  Write pointId
    string CPId = p_controlPoint->Id();
    QString ptId = "Point ID:  " +
                   QString::fromStdString(CPId.c_str());
    p_ptIdValue->setText(ptId);

    //  Write number of measures
    QString ptsize = "Number of Measures:  " +
                     QString::number(p_controlPoint->Size());
    p_numMeasures->setText(ptsize);

    //  Set ignore box correctly
    p_ignorePoint->setChecked(p_controlPoint->Ignore());

    //  Set hold box correctly
    p_holdPoint->setChecked(p_controlPoint->Held());

    //  Set ground box correctly
    p_groundPoint->setChecked(
      p_controlPoint->Type() == Isis::ControlPoint::Ground);

    // Clear combo boxes
    p_leftCombo->clear();
    p_rightCombo->clear();
    p_pointFiles.clear();
    //  Need all files for this point
    for(int i = 0; i < p_controlPoint->Size(); i++) {
      Isis::ControlMeasure &m = (*p_controlPoint)[i];
      string file = g_serialNumberList->Filename(m.CubeSerialNumber());
      p_pointFiles.push_back(file);
      string tempFilename = Isis::Filename(file).Name();
      p_leftCombo->addItem(tempFilename.c_str());
      p_rightCombo->addItem(tempFilename.c_str());
    }


    //  TODO:  WHAT HAPPENS IF THERE IS ONLY ONE MEASURE IN THIS CONTROLPOINT??
    //
    //
    //  Find the file from the cubeViewport that was originally used to select
    //  the point, this will be displayed on the left ChipViewport.
    int leftIndex = 0;
    //  Check for reference
    if(p_controlPoint->HasReference()) {
      leftIndex = p_controlPoint->ReferenceIndex();
    }
    else {
      if(p_leftFile.length() != 0) {
        string tempFilename = Isis::Filename(p_leftFile).Name();
        leftIndex = p_leftCombo->findText(QString(tempFilename.c_str()));
      }
    }
    int rightIndex = 0;

    if(leftIndex == 0) {
      rightIndex = 1;
    }
    else {
      rightIndex = 0;
    }
    //  Handle pts with a single measure, for now simply put measure on left/right
    //  Evenutally put on left with black on right??
    if(rightIndex + 1 > p_controlPoint->Size()) rightIndex = 0;
    p_rightCombo->setCurrentIndex(rightIndex);
    p_leftCombo->setCurrentIndex(leftIndex);

    //  Initialize pointEditor with measures
    selectLeftMeasure(leftIndex);
    selectRightMeasure(rightIndex);
  }


  /** 
   * Select left measure 
   * @param index Index of file from the point files vector 
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                          namespace std"
   */
  void QnetTool::selectLeftMeasure(int index) {
    string file = p_pointFiles[index];

    string serial = g_serialNumberList->SerialNumber(file);
    //  Find measure for each file
    p_leftMeasure = &(*p_controlPoint)[serial];

    //  If p_leftCube is not null, delete before creating new one
    if(p_leftCube != NULL) delete p_leftCube;
    p_leftCube = new Isis::Cube();
    p_leftCube->Open(file);

    //  Update left measure of pointEditor
    p_pointEditor->setLeftMeasure(p_leftMeasure, p_leftCube, p_controlPoint->Id());
    updateLeftMeasureInfo();

  }


  /** 
   * Select right measure 
   * @param index  Index of file from the point files vector
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                          namespace std"
   */
  void QnetTool::selectRightMeasure(int index) {

    string file = p_pointFiles[index];

    string serial = g_serialNumberList->SerialNumber(file);
    //  Find measure for each file
    p_rightMeasure = &(*p_controlPoint)[serial];

    //  If p_leftCube is not null, delete before creating new one
    if(p_rightCube != NULL) delete p_rightCube;
    p_rightCube = new Isis::Cube();
    p_rightCube->Open(file);

    //  Update left measure of pointEditor
    p_pointEditor->setRightMeasure(p_rightMeasure, p_rightCube, p_controlPoint->Id());
    updateRightMeasureInfo();

  }




  /**
   * @internal
   * @history 2008-11-24  Jeannie Walldren - Added "Goodness of Fit" to left
   *                         measure info.
   *
   */
  void QnetTool::updateLeftMeasureInfo() {

    //  Set ignore measure box correctly
    p_ignoreLeftMeasure->setChecked(p_leftMeasure->Ignore());

    QString s = "Measure Type: ";
    if(p_leftMeasure->Type() == Isis::ControlMeasure::Unmeasured) s += "Unmeasured";
    if(p_leftMeasure->Type() == Isis::ControlMeasure::Manual) s += "Manual";
    if(p_leftMeasure->Type() == Isis::ControlMeasure::Estimated) s += "Estimated";
    if(p_leftMeasure->Type() == Isis::ControlMeasure::Automatic) s += "Automatic";
    if(p_leftMeasure->Type() == Isis::ControlMeasure::ValidatedManual) s += "ValidatedManual";
    if(p_leftMeasure->Type() == Isis::ControlMeasure::ValidatedAutomatic) s += "ValidatedAutomatic";
    p_leftMeasureType->setText(s);
    s = "Sample Error: " + QString::number(p_leftMeasure->SampleError());
    p_leftSampError->setText(s);
    s = "Line Error: " + QString::number(p_leftMeasure->LineError());
    p_leftLineError->setText(s);
    if(p_leftMeasure->GoodnessOfFit() == Isis::Null) {
      s = "Goodness of Fit: Null";
    }
    else s = "Goodness of Fit: " + QString::number(p_leftMeasure->GoodnessOfFit());
    p_leftGoodness->setText(s);

  }



  /**
   * @internal
   *   @history 2008-11-24  Jeannie Walldren - Added "Goodness of Fit" to right
   *                           measure info.
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                          namespace std"
   *
   */

  void QnetTool::updateRightMeasureInfo() {

    //  Set ignore measure box correctly
    p_ignoreRightMeasure->setChecked(p_rightMeasure->Ignore());

    QString s = "Measure Type: ";
    if(p_rightMeasure->Type() == Isis::ControlMeasure::Unmeasured) s += "Unmeasured";
    if(p_rightMeasure->Type() == Isis::ControlMeasure::Manual) s += "Manual";
    if(p_rightMeasure->Type() == Isis::ControlMeasure::Estimated) s += "Estimated";
    if(p_rightMeasure->Type() == Isis::ControlMeasure::Automatic) s += "Automatic";
    if(p_rightMeasure->Type() == Isis::ControlMeasure::ValidatedManual) s += "ValidatedManual";
    if(p_rightMeasure->Type() == Isis::ControlMeasure::ValidatedAutomatic) s += "ValidatedAutomatic";
    p_rightMeasureType->setText(s);
    s = "Sample Error: " + QString::number(p_rightMeasure->SampleError());
    p_rightSampError->setText(s);
    s = "Line Error: " + QString::number(p_rightMeasure->LineError());
    p_rightLineError->setText(s);
    if(p_rightMeasure->GoodnessOfFit() == Isis::Null) {
      s = "Goodness of Fit: Null";
    }
    else s = "Goodness of Fit: " + QString::number(p_rightMeasure->GoodnessOfFit());
    p_rightGoodness->setText(s);
  }


  /**
   * Add measure to point
   */
  void QnetTool::addMeasure() {

    //  Create list of list box of all files highlighting those that
    //  contain the point, but that do not already have a measure.
    QStringList pointFiles;

    //  Initialize camera for all images in control network,
    //  TODO::   Needs to be moved to QnetFileTool.cpp
    Isis::Camera *cam;

    // If no apriori lat/lon for this point, use lat/lon of first measure
    double lat = p_controlPoint->UniversalLatitude();
    double lon = p_controlPoint->UniversalLongitude();
    if(lat == Isis::Null || lon == Isis::Null) {
      Isis::ControlMeasure m = (*p_controlPoint)[0];
      int camIndex = g_serialNumberList->SerialNumberIndex(m.CubeSerialNumber());
      cam = g_controlNetwork->Camera(camIndex);
      //cam = m.Camera();
      cam->SetImage(m.Sample(), m.Line());
      lat = cam->UniversalLatitude();
      lon = cam->UniversalLongitude();
    }

    for(int i = 0; i < g_serialNumberList->Size(); i++) {
      cam = g_controlNetwork->Camera(i);
      if(cam->SetUniversalGround(lat, lon)) {
        //  Make sure point is within image boundary
        double samp = cam->Sample();
        double line = cam->Line();
        if(samp >= 1 && samp <= cam->Samples() &&
            line >= 1 && line <= cam->Lines()) {
          pointFiles << QString::fromStdString(g_serialNumberList->Filename(i));
        }
      }
    }

    QnetNewMeasureDialog *newMeasureDialog = new QnetNewMeasureDialog();
    newMeasureDialog->SetFiles(*p_controlPoint, pointFiles);
    if(newMeasureDialog->exec()) {
      for(int i = 0; i < newMeasureDialog->fileList->count(); i++) {
        QListWidgetItem *item = newMeasureDialog->fileList->item(i);
        if(!newMeasureDialog->fileList->isItemSelected(item)) continue;
        //  Create measure for any file selected
        Isis::ControlMeasure *m = new Isis::ControlMeasure;
        //  Find serial number for this file
        string sn =
          g_serialNumberList->SerialNumber(item->text().toStdString());
        m->SetCubeSerialNumber(sn);
        int camIndex =
          g_serialNumberList->FilenameIndex(item->text().toStdString());
        cam = g_controlNetwork->Camera(camIndex);
        cam->SetUniversalGround(lat, lon);
        m->SetCoordinate(cam->Sample(), cam->Line());
        m->SetType(Isis::ControlMeasure::Estimated);
        m->SetDateTime();
        m->SetChooserName();
        p_controlPoint->Add(*m);
      }
      loadPoint();
      p_qnetTool->setShown(true);
      p_qnetTool->raise();
      loadTemplateFile(QString::fromStdString(
          p_pointEditor->templateFilename()));


      // emit a signal to alert user to save when exiting 
      emit netChanged();
      // emit signal so the nav tool can update edit point
      emit editPointChanged(p_controlPoint->Id());
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
    if(o == p_rightCombo->view()) {
      updateRightMeasureInfo();
      p_rightCombo->hidePopup();
    }
    return true;
  }


  /**
   * Take care of drawing things on a viewPort.
   * This is overiding the parents paintViewport member.
   * @param vp Pointer to Viewport to be painted
   * @param painter 
   */
  void QnetTool::paintViewport(MdiCubeViewport *vp, QPainter *painter) {

    drawAllMeasurments(vp, painter);

  }


  /** 
   * This method will repaint the given Point ID in each viewport. 
   * @param pointId
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                          namespace std"
   */
  void QnetTool::paintAllViewports(string pointId) {
    // Take care of drawing things on all viewPorts.
    // Calling update will cause the Tool class to call all registered tools
    // if point has been deleted, this will remove it from the main window
    MdiCubeViewport *vp;
    for(int i = 0; i < (int)cubeViewportList()->size(); i++) {
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
    for(int i = 0; i < g_controlNetwork->Size(); i++) {
      Isis::ControlPoint &p = (*g_controlNetwork)[i];
      // loop through the measurements
      for(int j = 0; j < p.Size(); j++) {
        // check whether this point is contained in the image
        if(p[j].CubeSerialNumber() == serialNumber) {
          // Find the measurments on the viewport
          double samp = p[j].Sample();
          double line = p[j].Line();
          int x, y;
          vp->cubeToViewport(samp, line, x, y);
          // if the point is ignored,
          if(p.Ignore()) {
            painter->setPen(QColor(255, 255, 0)); // set point marker yellow
          }
          // point is not ignored
          // if the measure matching this image is ignored,
          else if(p[j].Ignore()) {
            painter->setPen(QColor(255, 255, 0)); // set point marker yellow
          }
          // Neither point nor measure is not ignored and the measure is ground,
          else if(p.Type() == Isis::ControlPoint::Ground) {
            painter->setPen(Qt::magenta);// set point marker magenta
          }
          else {
            painter->setPen(Qt::green); // set all other point markers green
          }
          // draw points
          painter->drawLine(x - 5, y, x + 5, y);
          painter->drawLine(x, y - 5, x, y + 5);
        }
        // if point is not in the image, go to next point
        else continue;
      }
    }
    // if QnetTool is open,
    if(p_controlPoint != NULL) {
      // and the selected point is in the image,
      if(p_controlPoint->HasSerialNumber(serialNumber)) {
        // find the measurement
        double samp = (*p_controlPoint)[serialNumber].Sample();
        double line = (*p_controlPoint)[serialNumber].Line();
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


  bool QnetTool::okToContinue() {
  
    if (p_templateModified) {
      int r = QMessageBox::warning(p_qnetTool, tr("OK to continue?"),
          tr("The currently opened registration template has been modified.\n"
          "Save changes?"),
          QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
          QMessageBox::Yes);
          
      if (r == QMessageBox::Yes)
        saveTemplateFileAs();
      else
        if (r == QMessageBox::Cancel)
          return false;
    }
    
    return true;
  }
  
  
  void QnetTool::openTemplateFile() {
  
    if (!okToContinue())
      return;
    
    QString filename = QFileDialog::getOpenFileName(p_qnetTool,
        "Select a registration template", ".",
        "Registration template files (*.def *.pvl);;All files (*)");
        
    if (filename.isEmpty())
      return;
      
    if (p_pointEditor->setTemplateFile(filename)) {
      p_templateFilenameLabel->setText("Template File: " + filename);
      loadTemplateFile(filename);
    }
  }
  
  
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
  }
  
  
  void QnetTool::setTemplateModified() {
    p_templateModified = true;
    p_saveTemplateFile->setEnabled(true);
  }
  
  
  void QnetTool::saveTemplateFile() {
  
    if (!p_templateModified)
      return;
    
    QString filename = QString::fromStdString(
        p_pointEditor->templateFilename());
        
    writeTemplateFile(filename);
  }
  
  
  void QnetTool::saveTemplateFileAs() {
  
    QString filename = QFileDialog::getSaveFileName(p_qnetTool,
        "Save registration template", ".",
        "Registration template files (*.def *.pvl);;All files (*)");
        
    if (filename.isEmpty())
      return;
  
    writeTemplateFile(filename);
    p_pointEditor->setTemplateFile(filename);
  }
  
  
  void QnetTool::writeTemplateFile(QString fn) {
  
    QFile file(QString::fromStdString(Filename((iString) fn).Expanded()));
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
      QString msg = "Failed to save template file to \"" + fn + "\"\nDo you "
          "have permission?";
      QMessageBox::warning(p_qnetTool, "IO Error", msg);
      return;
    }
    
    QString contents = p_templateEditor->toPlainText();
    
    // catch errors in Pvl format when populating pvl object
    stringstream ss;
    ss << contents.toStdString();
    try {
      Pvl pvl;
      ss >> pvl;
    }
    catch(Isis::iException &e) {
      QString message = e.Errors().c_str();
      e.Clear();
      QMessageBox::warning(p_qnetTool, "Error", message);
      return;
    }
    
    // now save contents
    QTextStream stream(&file);
    stream << contents;
    
    file.close();
    if (p_pointEditor->setTemplateFile(fn)) {
      p_templateModified = false;
      p_saveTemplateFile->setEnabled(false);
    }
  }


  /**
   * Allows the user to view the template file that is currently
   * set.
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
    try {
      // Get the template file from the ControlPointEditor object
      Isis::Pvl templatePvl(p_pointEditor->templateFilename());
      // Create registration dialog window using PvlEditDialog class
      // to view and/or edit the template
      Isis::PvlEditDialog registrationDialog(templatePvl);
      registrationDialog.setWindowTitle("View or Edit Template File: "
                                        + QString::fromStdString(templatePvl.Filename()));
      registrationDialog.resize(550, 360);
      registrationDialog.exec();
    }
    catch(Isis::iException &e) {
      QString message = e.Errors().c_str();
      e.Clear();
      QMessageBox::information((QWidget *)parent(), "Error", message);
    }
  }


  /**
   * Slot which calls ControlPointEditor slot to save chips
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
   * Refresh all necessary widgets in QnetTool including the PointEditor and
   * CubeViewports.
   *
   * @author 2008-12-09 Tracie Sucharski
   *
   */
  void QnetTool::refresh() {

    //  Check point being edited, make sure it still exists, if not ???
    //  Update ignored checkbox??
    if(p_controlPoint != NULL) {
      try {
        QString id = p_ptIdValue->text().remove("Point ID:  ");
        Isis::ControlPoint *pt =
          g_controlNetwork->Find(id.toStdString());
        pt = 0;
      }
      catch(Isis::iException &e) {
        p_controlPoint = NULL;
        p_qnetTool->setShown(false);
      }
    }

    if(p_controlPoint == NULL) {
      paintAllViewports("");
    }
    else {
      paintAllViewports(p_controlPoint->Id());
    }
  }


  /**
   * Emits a signal to displays the Navigation window.  This signal is connected
   * to QnetNavTool.
   * @internal
   *   @history 2010-07-01 Jeannie Walldren - Original version
   */
  void QnetTool::showNavWindow() {
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
   *   @history 2010-07-01 Jeannie Walldren - Original version.
   */
  QWidget *QnetTool::createToolBarWidget(QStackedWidget *parent) {
    QWidget *hbox = new QWidget(parent);

    QToolButton *showNavToolButton = new QToolButton();
    showNavToolButton->setText("Show Nav Tool");
    showNavToolButton->setToolTip("Shows the Navigation Tool Window");
    QString text =
      "<b>Function:</b> This button will bring up the Navigation Tool window that allows \
     the user to view, modify, ignore, delete, or filter points and cubes.";
    showNavToolButton->setWhatsThis(text);
    connect(showNavToolButton, SIGNAL(clicked()), this, SLOT(showNavWindow()));

    QHBoxLayout *layout = new QHBoxLayout(hbox);
    layout->setMargin(0);
    layout->addWidget(showNavToolButton);
    layout->addStretch(1);
    hbox->setLayout(layout);
    return hbox;
  }
}

