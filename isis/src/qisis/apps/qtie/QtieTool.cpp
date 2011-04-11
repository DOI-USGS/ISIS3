#include "QtieTool.h"

#include <QApplication>
#include <QDialog>
#include <QMenuBar>
#include <QMenu>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QLineEdit>
#include <QRadioButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QPoint>
#include <QStringList>
#include <QSizePolicy>
#include <QTimer>
#include <QMessageBox>
#include <QFileDialog>
#include <QtGui>

#include <vector>

#include "Application.h"
#include "AutoReg.h"
#include "AutoRegFactory.h"
#include "BundleAdjust.h"
#include "ControlMeasure.h"
#include "Filename.h"
#include "History.h"
#include "iString.h"
#include "iTime.h"
#include "Latitude.h"
#include "Longitude.h"
#include "MainWindow.h"
#include "MdiCubeViewport.h"
#include "PvlEditDialog.h"
#include "PvlObject.h"
#include "QIsisApplication.h"
#include "SerialNumber.h"
#include "ToolPad.h"
#include "UniversalGroundMap.h"

using namespace Isis;
using namespace std;


namespace Qisis {
  // initialize static
  QString QtieTool::lastPtIdValue = "";

  /**
   * Construct the QtieTool
   *
   * @author 2008-11-21 Tracie Sucharski
   *
   * @history  2010-05-11 Tracie Sucharski - Moved the creation of control net
   *                           to the QtieFileTool::open.
   */
  QtieTool::QtieTool(QWidget *parent) : Qisis::Tool(parent) {

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
    p_serialNumberList = new Isis::SerialNumberList(false);
    p_controlNet = NULL;
    p_controlPoint = NULL;

    p_twist = true;
    p_tolerance = 5;
    p_maxIterations = 10;
    p_ptIdIndex = 0;
    createQtieTool(parent);

  }


  /**
   * Design the QtieTool widget
   *
   */
  void QtieTool::createQtieTool(QWidget *parent) {

    // Create dialog with a main window
    p_tieTool = new QMainWindow(parent);
    p_tieTool->setWindowTitle("Tie Point Tool");
    p_tieTool->layout()->setSizeConstraint(QLayout::SetFixedSize);

    createMenus();

    // Place everything in a grid
    QGridLayout *gridLayout = new QGridLayout();
    //gridLayout->setSizeConstraint(QLayout::SetFixedSize);
    //  ???  Very tacky-hardcoded to ChipViewport size of ControlPointEdit + xtra.
    //       Is there a better way to do this?
    gridLayout->setColumnMinimumWidth(0, 310);
    gridLayout->setColumnMinimumWidth(1, 310);
    //  grid row
    int row = 0;

    QCheckBox *twist = new QCheckBox("Twist");
    twist->setChecked(p_twist);
    connect(twist, SIGNAL(toggled(bool)), this, SLOT(setTwist(bool)));
    QLabel *iterationLabel = new QLabel("Maximum Iterations");
    QSpinBox *iteration = new QSpinBox();
    iteration->setRange(1, 100);
    iteration->setValue(p_maxIterations);
    iterationLabel->setBuddy(iteration);
    connect(iteration, SIGNAL(valueChanged(int)), this, SLOT(setIterations(int)));
    QHBoxLayout *itLayout = new QHBoxLayout();
    itLayout->addWidget(iterationLabel);
    itLayout->addWidget(iteration);


    QLabel *tolLabel = new QLabel("Tolerance");
    p_tolValue = new QLineEdit();
    tolLabel->setBuddy(p_tolValue);
    QHBoxLayout *tolLayout = new QHBoxLayout();
    tolLayout->addWidget(tolLabel);
    tolLayout->addWidget(p_tolValue);
//    QDoubleValidator *dval = new QDoubleValidator
    QString tolStr;
    tolStr.setNum(p_tolerance);
    p_tolValue->setText(tolStr);
//    connect(p_tolValue,SIGNAL(valueChanged()),this,SLOT(setTolerance()));

    QHBoxLayout *options = new QHBoxLayout();
    options->addWidget(twist);
    options->addLayout(itLayout);
    options->addLayout(tolLayout);

    gridLayout->addLayout(options, row++, 0);

    p_ptIdValue = new QLabel();
    gridLayout->addWidget(p_ptIdValue, row++, 0);

    p_pointEditor = new Qisis::ControlPointEdit(NULL, parent, true);
    gridLayout->addWidget(p_pointEditor, row++, 0, 1, 3);
    connect(p_pointEditor, SIGNAL(pointSaved()), this, SLOT(pointSaved()));
    p_pointEditor->show();
    connect(this,
            SIGNAL(stretchChipViewport(Isis::Stretch *, Qisis::CubeViewport *)),
            p_pointEditor,
            SIGNAL(stretchChipViewport(Isis::Stretch *, Qisis::CubeViewport *)));


    QPushButton *solve = new QPushButton("Solve");
    connect(solve, SIGNAL(clicked()), this, SLOT(solve()));
    gridLayout->addWidget(solve, row++, 0);

    QWidget *cw = new QWidget();
    cw->setLayout(gridLayout);
    p_tieTool->setCentralWidget(cw);

    connect(this, SIGNAL(editPointChanged()), this, SLOT(drawMeasuresOnViewports()));
    connect(this, SIGNAL(newSolution(Isis::Table *)),
            this, SLOT(writeNewCmatrix(Isis::Table *)));

  }


  /**
   * Create the menus for QtieTool
   *
   *
   */
  void QtieTool::createMenus() {

    QAction *saveNet = new QAction(p_tieTool);
    saveNet->setText("Save Control Network &As...");
    QString whatsThis =
      "<b>Function:</b> Saves the current <i>control network</i> under chosen filename";
    saveNet->setWhatsThis(whatsThis);
    connect(saveNet, SIGNAL(activated()), this, SLOT(saveNet()));

    QAction *closeQtieTool = new QAction(p_tieTool);
    closeQtieTool->setText("&Close");
    closeQtieTool->setShortcut(Qt::ALT + Qt::Key_F4);
    whatsThis =
      "<b>Function:</b> Closes the Qtie Tool window for this point \
       <p><b>Shortcut:</b> Alt+F4 </p>";
    closeQtieTool->setWhatsThis(whatsThis);
    connect(closeQtieTool, SIGNAL(activated()), p_tieTool, SLOT(close()));

    QMenu *fileMenu = p_tieTool->menuBar()->addMenu("&File");
    fileMenu->addAction(saveNet);
    fileMenu->addAction(closeQtieTool);

    QAction *templateFile = new QAction(p_tieTool);
    templateFile->setText("&Set registration template");
    whatsThis =
      "<b>Function:</b> Allows user to select a new file to set as the registration template";
    templateFile->setWhatsThis(whatsThis);
    connect(templateFile, SIGNAL(activated()), this, SLOT(setTemplateFile()));

    QAction *viewTemplate = new QAction(p_tieTool);
    viewTemplate->setText("&View/edit registration template");
    whatsThis =
      "<b>Function:</b> Displays the curent registration template.  \
       The user may edit and save changes under a chosen filename.";
    viewTemplate->setWhatsThis(whatsThis);
    connect(viewTemplate, SIGNAL(activated()), this, SLOT(viewTemplateFile()));


    QMenu *optionMenu = p_tieTool->menuBar()->addMenu("&Options");
    QMenu *regMenu = optionMenu->addMenu("&Registration");

    regMenu->addAction(templateFile);
    regMenu->addAction(viewTemplate);
    //    registrationMenu->addAction(interestOp);

  }


  /**
   * Put the QtieTool icon on the main window Toolpad
   *
   * @param pad   Input  Toolpad for the main window
   *
   */

  QAction *QtieTool::toolPadAction(ToolPad *pad) {
    QAction *action = new QAction(pad);
    action->setIcon(QPixmap(toolIconDir() + "/stock_draw-connector-with-arrows.png"));
    action->setToolTip("Tie (T)");
    action->setShortcut(Qt::Key_T);
    return action;
  }


  /**
   * Setup the base map and match cube
   *
   * @param baseCube   Input  Base map cube
   * @param matchCube  Input  Match cube
   *
   */
  void QtieTool::setFiles(Isis::Cube &baseCube, Isis::Cube &matchCube, Isis::ControlNet &cnet) {
    //  Save off base map cube, but add matchCube to serial number list
    p_baseCube = &baseCube;
    p_matchCube = &matchCube;
    p_controlNet = &cnet;
    p_baseSN = Isis::SerialNumber::Compose(*p_baseCube, true);
    p_matchSN = Isis::SerialNumber::Compose(*p_matchCube);

    p_serialNumberList->Add(matchCube.Filename());

    //  Save off universal ground maps
    try {
      p_baseGM = new Isis::UniversalGroundMap(*p_baseCube);
    }
    catch (Isis::iException &e) {
      QString message = "Cannot initialize universal ground map for basemap.\n";
      string errors = e.Errors();
      message += errors.c_str();
      e.Clear();
      QMessageBox::critical((QWidget *)parent(), "Error", message);
      return;
    }
    try {
      p_matchGM = new Isis::UniversalGroundMap(matchCube);
    }
    catch (Isis::iException &e) {
      QString message = "Cannot initialize universal ground map for match cube.\n";
      string errors = e.Errors();
      message += errors.c_str();
      e.Clear();
      QMessageBox::critical((QWidget *)parent(), "Error", message);
      return;
    }

    //  If Control Net has points, set the basemap measures, since they are not
    //  saved in the net file.
    if (cnet.GetNumPoints() != 0) {
      for (int i = 0; i < cnet.GetNumPoints(); i++) {
        Isis::ControlPoint &p = *cnet[i];
        double baseSamp, baseLine;


        if (p_baseGM->SetGround(p.GetBestSurfacePoint())) {
          //  Make sure point on base cube
          baseSamp = p_baseGM->Sample();
          baseLine = p_baseGM->Line();
          if (baseSamp < 1 || baseSamp > p_baseCube->Samples() ||
              baseLine < 1 || baseLine > p_baseCube->Lines()) {
            // throw error? point not on base
            QString message = "Error parsing input control net.  Lat/Lon for Point Id: " +
                              QString::fromStdString(p.GetId()) + " computes to a sample/line off " +
                              "the edge of the basemap cube.";
            QMessageBox::critical((QWidget *)parent(), "Control Net Error", message);
            return;
          }
        }
        else {
          //  throw error?  point not on base cube
          QString message = "Error parsing input control net.  Point Id: " +
                            QString::fromStdString(p.GetId()) + " does not exist on basemap.";
          QMessageBox::critical((QWidget *)parent(), "Control Net Error", message);
          return;
        }

        Isis::ControlMeasure *mB = new Isis::ControlMeasure;
        mB->SetCubeSerialNumber(p_baseSN);
        mB->SetCoordinate(baseSamp, baseLine);
//        mB->SetType(Isis::ControlMeasure::Estimated);
        mB->SetDateTime();
        mB->SetChooserName();
        mB->SetIgnored(true);

        p.Add(mB);
      }
    }

  }


  /**
   * New files selected, clean up old file info
   *
   * @internal
   * @history  2007-06-12 Tracie Sucharski - Added method to allow user to
   *                           run on new files.
   * @history  2010-05-11 Tracie Sucharski - Moved the creation of control net
   *                           to the QtieFileTool::open.
   */
  void QtieTool::clearFiles() {
    p_tieTool->setShown(false);
    //  delete p_baseCube;
    //  delete p_matchCube;

    delete p_serialNumberList;
    p_serialNumberList = new Isis::SerialNumberList(false);

    delete p_controlNet;

    delete p_baseGM;
    delete p_matchGM;

  }


  /**
   * Save control point under crosshairs of ChipViewports
   *
   */
  void QtieTool::pointSaved() {
    //  Get sample/line from base map and find lat/lon
    double samp = p_controlPoint->GetMeasure(Base)->GetSample();
    double line = p_controlPoint->GetMeasure(Base)->GetLine();

    p_baseGM->SetImage(samp, line);
    Latitude lat(p_baseGM->UniversalLatitude(), Angle::Degrees);
    Longitude lon(p_baseGM->UniversalLongitude(), Angle::Degrees);
    Distance radius(p_baseGM->Projection()->LocalRadius(), Distance::Meters);

    p_controlPoint->SetAprioriSurfacePoint(SurfacePoint(lat, lon, radius));

    emit editPointChanged();
  }


  /**
   * Handle mouse events on match CubeViewport
   *
   * @param p[in]   (QPoint)            Point under cursor in cubeviewport
   * @param s[in]   (Qt::MouseButton)   Which mouse button was pressed
   *
   * @internal
   * @history  2007-06-12 Tracie Sucharski - Swapped left and right mouse
   *                           button actions.
   * @history  2008-11-19 Tracie Sucharski - Only allow mouse events on
   *                           match cube.
   *
   */
  void QtieTool::mouseButtonRelease(QPoint p, Qt::MouseButton s) {
    MdiCubeViewport *cvp = cubeViewport();
    if (cvp  == NULL)
      return;
    if (cubeViewportList()->size() != 2) {
      QString message = "You must have a basemap and a match cube open.";
      QMessageBox::critical((QWidget *)parent(), "Error", message);
      return;
    }
    if (cvp->cube() == p_baseCube) {
      QString message = "Select points on match Cube only.";
      QMessageBox::information((QWidget *)parent(), "Warning", message);
      return;
    }
    if (cvp->cursorInside())
      QPoint p = cvp->cursorPosition();

    // ???  do we only allow mouse clicks on level1???
    //    If we allow on both, need to find samp,line on level1 if
    //    they clicked on basemap.
    std::string file = cvp->cube()->Filename();
    std::string sn = p_serialNumberList->SerialNumber(file);

    double samp, line;
    cvp->viewportToCube(p.x(), p.y(), samp, line);

    if (s == Qt::LeftButton) {
      //  Find closest control point in network
      Isis::ControlPoint *point =
        p_controlNet->FindClosest(sn, samp, line);
      //  TODO:  test for errors and reality
      if (point == NULL) {
        QString message = "No points exist for editing.  Create points ";
        message += "using the right mouse button.";
        QMessageBox::information((QWidget *)parent(), "Warning", message);
        return;
      }
      modifyPoint(point);
    }
    else if (s == Qt::MidButton) {
      //  Find closest control point in network
      Isis::ControlPoint *point =
        p_controlNet->FindClosest(sn, samp, line);
      //  TODO:  test for errors and reality
      if (point == NULL) {
        QString message = "No points exist for deleting.  Create points ";
        message += "using the right mouse button.";
        QMessageBox::information((QWidget *)parent(), "Warning", message);
        return;
      }
      deletePoint(point);
    }
    else if (s == Qt::RightButton) {
      p_matchGM->SetImage(samp, line);
      double lat = p_matchGM->UniversalLatitude();
      double lon = p_matchGM->UniversalLongitude();

      createPoint(lat, lon);
    }
  }


  /**
   * Create control point at given lat,lon
   *
   * @param lat   Input  Latitude of new point
   * @param lon   Input  Longitude of new point
   *
   * @internal
   * @history  2008-12-06 Tracie Sucharski - Set point type to Ground
   * @history  2010-05-18 Jeannie Walldren - Modified Point ID
   *                          QInputDialog to return if "Cancel"
   *                          is clicked.
   */
  void QtieTool::createPoint(double lat, double lon) {

    //  TODO:   ADD AUTOSEED OPTION (CHECKBOX?)

    double baseSamp = 0, baseLine = 0;
    double matchSamp, matchLine;

    //  if clicked in match, get samp,line
    p_matchGM->SetUniversalGround(lat, lon);
    matchSamp = p_matchGM->Sample();
    matchLine = p_matchGM->Line();

    //  Make sure point is on base
    if (p_baseGM->SetUniversalGround(lat, lon)) {
      //  Make sure point on base cube
      baseSamp = p_baseGM->Sample();
      baseLine = p_baseGM->Line();
      if (baseSamp < 1 || baseSamp > p_baseCube->Samples() ||
          baseLine < 1 || baseLine > p_baseCube->Lines()) {
        // throw error? point not on base
        QString message = "Point does not exist on base map.";
        QMessageBox::warning((QWidget *)parent(), "Warning", message);
      }
    }
    else {
      //  throw error?  point not on base cube
      // throw error? point not on base
      QString message = "Point does not exist on base map.";
      QMessageBox::warning((QWidget *)parent(), "Warning", message);
    }

    //  Get radius
//    double radius = p_baseGM->Projection()->LocalRadius();

    //  Point is on both base and match, create new control point
    Isis::ControlPoint *newPoint = NULL;
    // First prompt for pointId
    bool goodId = false;
    while (!goodId) {
      bool ok = false;
      QString id = QInputDialog::getText((QWidget *)parent(),
                                         "Point ID", "Enter Point ID:",
                                         QLineEdit::Normal, lastPtIdValue,
                                         &ok);
      if (!ok)
        // user clicked "Cancel"
      {
        return;
      }
      if (ok && id.isEmpty())
        // user clicked "Ok" but did not enter a point ID
      {
        QString message = "You must enter a point Id.";
        QMessageBox::warning((QWidget *)parent(), "Warning", message);
      }
      else {
        // Make sure Id doesn't already exist
        newPoint = new Isis::ControlPoint(id.toStdString());
        if (p_controlNet->ContainsPoint(newPoint->GetId())) {
          iString message = "A ControlPoint with Point Id = [" +
                            newPoint->GetId() +
                            "] already exists.  Re-enter unique Point Id.";
          QMessageBox::warning((QWidget *)parent(), "Unique Point Id", message);
        }
        else {
          goodId = true;
          lastPtIdValue = id;
        }
      }
    }


//    newPoint->SetUniversalGround(lat, lon, radius);
//    newPoint->SetType(Isis::ControlPoint::Ground);

    // Set first measure to match
    ControlMeasure *mM = new ControlMeasure;
    mM->SetCubeSerialNumber(p_matchSN);
    mM->SetCoordinate(matchSamp, matchLine);
//    mM->SetType(Isis::ControlMeasure::Estimated);
    mM->SetDateTime();
    mM->SetChooserName();
    newPoint->Add(mM);
    //  Second measure is base measure, set to Ignore=yes
    ControlMeasure *mB = new ControlMeasure;
    mB->SetCubeSerialNumber(p_baseSN);
    mB->SetCoordinate(baseSamp, baseLine);
//    mB->SetType(ControlMeasure::Estimated);
    mB->SetDateTime();
    mB->SetChooserName();
    mB->SetIgnored(true);
    newPoint->Add(mB);

    //  Add new control point to control network
    p_controlNet->AddPoint(newPoint);
    //  Read newly added point
    p_controlPoint = p_controlNet->GetPoint((QString) newPoint->GetId());
    //  Load new point in QtieTool
    loadPoint();
    p_tieTool->setShown(true);
    p_tieTool->raise();

    emit editPointChanged();
  }


  /**
   * Delete given control point
   *
   * @param point Input  Control Point to delete
   *
   * @history 2010-05-19 Tracie Sucharski - Fixed bug which was causing a seg
   *                        fault.  Set p_controlPoint to NULL, also no sense
   *                        loading point to be deleted.  Should this be
   *                        smartened up to load another Point?
   *
   */
  void QtieTool::deletePoint(Isis::ControlPoint *point) {

    p_controlPoint = point;
    //  Change point in viewport to red so user can see what point they are
    //  about to delete.
    emit editPointChanged();

    //loadPoint();

    p_controlNet->DeletePoint(p_controlPoint->GetId());
    p_tieTool->setShown(false);
    p_controlPoint = NULL;

    emit editPointChanged();
  }


  /**
   * Modify given control point
   *
   * @param point Input  Control Point to modify
   *
   */
  void QtieTool::modifyPoint(Isis::ControlPoint *point) {

    p_controlPoint = point;
    loadPoint();
    p_tieTool->setShown(true);
    p_tieTool->raise();
    emit editPointChanged();
  }


  /**
   * Load control point into the ControlPointEdit widget
   *
   * @history 2010-05-18  Tracie Sucharski - Added pointId to the dialog.
   */
  void QtieTool::loadPoint() {

    //  Initialize pointEditor with measures
    p_pointEditor->setLeftMeasure(p_controlPoint->GetMeasure(Base), p_baseCube,
                                  p_controlPoint->GetId());
    p_pointEditor->setRightMeasure(p_controlPoint->GetMeasure(Match),
                                   p_matchCube, p_controlPoint->GetId());

    //  Write pointId
    iString ptId = "Point ID:  " + p_controlPoint->GetId();
    p_ptIdValue->setText(ptId);
  }


  /**
   * Draw all Control Measures on each viewport
   *
   */
  void QtieTool::drawMeasuresOnViewports() {

    MdiCubeViewport *vp;
    for (int i = 0; i < (int)cubeViewportList()->size(); i++) {
      vp = (*(cubeViewportList()))[i];
      vp->viewport()->update();
    }
  }


  /**
   * Repaint the given CubeViewport
   *
   * @param vp       Input  CubeViewport to repain
   * @param painter  Input  Qt Painter
   *
   */
  void QtieTool::paintViewport(MdiCubeViewport *vp, QPainter *painter) {

    //  Make sure we have points to draw
    if (p_controlNet == NULL || p_controlNet->GetNumPoints() == 0)
      return;

    //  Draw all measures
    std::string serialNumber = Isis::SerialNumber::Compose(*vp->cube(), true);
    for (int i = 0; i < p_controlNet->GetNumPoints(); i++) {
      Isis::ControlPoint &p = *p_controlNet->GetPoint(i);
      if (p_controlPoint != NULL && p.GetId() == p_controlPoint->GetId()) {
        painter->setPen(QColor(200, 0, 0));
      }
      else {
        painter->setPen(QColor(0, 200, 0));
      }

      double samp, line;
      if (vp->cube()->Filename() == p_baseCube->Filename()) {
        // Draw on left viewport (base)
        samp = p[Base]->GetSample();
        line = p[Base]->GetLine();
      }
      else {
        // Draw on right viewport (match)
        samp = p[Match]->GetSample();
        line = p[Match]->GetLine();
      }
      int x, y;
      vp->cubeToViewport(samp, line, x, y);
      painter->drawLine(x - 5, y, x + 5, y);
      painter->drawLine(x, y - 5, x, y + 5);
    }

  }


  /**
   * Perform the BundleAdjust Solve
   *
   * @history 2011-04-11 Debbie A. Cook - Removed obsolete argument from
   *          BundleAdjust method Solve
   */
  void QtieTool::solve() {

    //  First off , get tolerance,  NEED to VALIDATE
    p_tolerance = p_tolValue->text().toDouble();

    //  Need at least 2 points to solve for twist
    if (p_twist) {
      if (p_controlNet->GetNumPoints() < 2) {
        QString message = "Need at least 2 points to solve for twist. \n";
        QMessageBox::critical((QWidget *)parent(), "Error", message);
        return;
      }
    }
    Isis::ControlNet net;

    // Bundle adjust to solve for new pointing
    try {
      //  Create new control net for bundle adjust , deleting ignored measures
      for (int p = 0; p < p_controlNet->GetNumPoints(); p++) {
        ControlPoint *pt = new ControlPoint(*p_controlNet->GetPoint(p));
        for (int m = 0; m < pt->GetNumMeasures(); m++) {
          if (pt->GetMeasure(m)->SetIgnored(true))
            pt->Delete(m);
        }
        net.AddPoint(pt);
      }

      BundleAdjust b(net, *p_serialNumberList, false);
      b.SetSolveTwist(p_twist);
      //b.Solve(p_tolerance, p_maxIterations);
      b.Solve();

      // Print results and give user option of updating cube pointin
//      double maxError = net.MaximumResidual();
//      double avgError = net.AverageResidual();
      double maxError = net.GetMaximumResidual();
      double avgError = net.AverageResidual();

      QString message = "Maximum Error = " + QString::number(maxError);
      message += "\nAverage Error = " + QString::number(avgError);
      QString msgTitle = "Update camera pointing?";
      QMessageBox msgBox(QMessageBox::Question, msgTitle, message, 0, p_tieTool);
      QPushButton *update = msgBox.addButton("Update", QMessageBox::AcceptRole);
      QPushButton *close = msgBox.addButton("Close", QMessageBox::RejectRole);
      msgBox.setDefaultButton(close);
      msgBox.exec();
      if (msgBox.clickedButton() == update) {
        p_matchCube->ReOpen("rw");
        Isis::Table cmatrix = b.Cmatrix(0);
        //cmatrix = b.Cmatrix(0);
        emit newSolution(&cmatrix);
      }
      else {
        return;
      }

    }
    catch (Isis::iException &e) {
      QString message = "Bundle Solution failed.\n";
      string errors = e.Errors();
      message += errors.c_str();
      e.Clear();
//      message += "\n\nMaximum Error = " + QString::number(net.MaximumResiudal());
//      message += "\nAverage Error = " + QString::number(net.AverageResidual());
      message += "\n\nMaximum Error = " + QString::number(net.GetMaximumResidual());
      message += "\nAverage Error = " + QString::number(net.AverageResidual());
      QMessageBox::warning((QWidget *)parent(), "Error", message);
      return;
    }
  }


  /**
   * Write the new cmatrix to the match cube
   *
   * @param cmatrix    Input  New adjusted cmatrix
   *
   */
  void QtieTool::writeNewCmatrix(Isis::Table *cmatrix) {

    //check for existing polygon, if exists delete it
    if (p_matchCube->Label()->HasObject("Polygon")) {
      p_matchCube->Label()->DeleteObject("Polygon");
    }

    // Update the cube history
    p_matchCube->Write(*cmatrix);
    Isis::History h("IsisCube");
    try {
      p_matchCube->Read(h);
    }
    catch (Isis::iException &e) {
      QString message = "Could not read cube history, "
                        "will not update history.\n";
      string errors = e.Errors();
      message += errors.c_str();
      QMessageBox::warning((QWidget *)parent(), "Warning", message);
      e.Clear();
      return;
    }
    Isis::PvlObject history("qtie");
    history += Isis::PvlKeyword("IsisVersion", Isis::Application::Version());
    QString path = QCoreApplication::applicationDirPath();
    history += Isis::PvlKeyword("ProgramPath", path);
    history += Isis::PvlKeyword("ExecutionDateTime",
                                Isis::Application::DateTime());
    history += Isis::PvlKeyword("HostName", Isis::Application::HostName());
    history += Isis::PvlKeyword("UserName", Isis::Application::UserName());
    Isis::PvlGroup results("Results");
    results += Isis::PvlKeyword("CameraAnglesUpdated", "True");
    results += Isis::PvlKeyword("BaseMap", p_baseCube->Filename());
    history += results;

    h.AddEntry(history);
    p_matchCube->Write(h);
    p_matchCube->ReOpen("r");

  }


  /**
   * Allows user to set a new template file.
   * @author 2008-12-10 Jeannie Walldren
   * @internal
   *   @history 2008-12-10 Jeannie Walldren - Original Version
   */

  void QtieTool::setTemplateFile() {
    QString filename = QFileDialog::getOpenFileName(p_mw,
                       "Select a registration template", ".",
                       "Registration template files (*.def *.pvl);;All files (*)");

    if (filename.isEmpty())
      return;

    p_pointEditor->setTemplateFile(filename);
  }


  /**
   * Allows the user to view the template file that is currently
   * set.
   * @author 2008-12-10 Jeannie Walldren
   * @internal
   *   @history 2008-12-10 Jeannie Walldren - Original Version
   *   @history 2008-12-10 Jeannie Walldren - Added "Isis::"
   *            namespace to PvlEditDialog reference and changed
   *            registrationDialog from pointer to object
   *   @history 2008-12-15 Jeannie Walldren - Added QMessageBox
   *            warning in case Template File cannot be read.
   */
  void QtieTool::viewTemplateFile() {
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
    catch (Isis::iException &e) {
      QString message = e.Errors().c_str();
      e.Clear();
      QMessageBox::warning((QWidget *)parent(), "Error", message);
    }
  }


  /**
   * Save the ground points to a ControlNet.
   *
   * @author 2008-12-30 Tracie Sucharski
   */
  void QtieTool::saveNet() {

    QString filter = "Control net (*.net);;";
    filter += "Text file (*.txt);;";
    filter += "All (*)";
    QString fn = QFileDialog::getSaveFileName((QWidget *)parent(),
                 "Choose filename to save under",
                 ".", filter);
    if (!fn.isEmpty()) {
      try {
        //  Create new control net for bundle adjust , deleting ignored measures
        // which are the basemap measures.
        Isis::ControlNet net;
        for (int p = 0; p < p_controlNet->GetNumPoints(); p++) {
          ControlPoint *pt = new ControlPoint(*p_controlNet->GetPoint(p));
          for (int m = 0; m < pt->GetNumMeasures(); m++) {
            if (pt->GetMeasure(m)->IsIgnored())
              pt->Delete(m);
          }
//          net.SetType(Isis::ControlNet::ImageToGround);
          net.SetTarget(p_matchCube->Camera()->Target());
          net.SetNetworkId("Qtie");
          net.SetUserName(Isis::Application::UserName());
          net.SetCreatedDate(Isis::Application::DateTime());
          net.SetModifiedDate(Isis::iTime::CurrentLocalTime());
          net.SetDescription("Qtie Ground Points");
          net.AddPoint(pt);
        }

        net.Write(fn.toStdString());
      }
      catch (Isis::iException &e) {
        QString message = "Error saving control network.  \n";
        string errors = e.Errors();
        message += errors.c_str();
        e.Clear();
        QMessageBox::information((QWidget *)parent(), "Error", message);
        return;
      }
    }
    else {
      QMessageBox::information((QWidget *)parent(),
                               "Error", "Saving Aborted");
    }
  }
}
