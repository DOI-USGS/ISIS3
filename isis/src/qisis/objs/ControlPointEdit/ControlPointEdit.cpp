#include "ControlPointEdit.h"

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QDial>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLCDNumber>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QSize>
#include <QString>
#include <QTimer>
#include <QToolButton>

#include "Application.h"
#include "AutoReg.h"
#include "AutoRegFactory.h"
#include "ChipViewport.h"
#include "ControlMeasure.h"
#include "Filename.h"
#include "iString.h"
#include "Pvl.h"
#include "System.h"
#include "UniversalGroundMap.h"

namespace Qisis {

  const int VIEWSIZE = 301;

  /**
   * Constructs a ControlPointEdit widget
   *
   * @param parent           Input  Parent of widget
   * @param allowLeftMouse   Input  Allow/Disallow mouse events on Left
   *                                    ChipViewport
   * @author Tracie Sucharski
   * @internal
   *   @history 2008-15-2008  Jeannie Walldren - Added error
   *                            string to Isis::iException::Message before
   *                            creating QMessageBox
   */
  ControlPointEdit::ControlPointEdit(Isis::ControlNet * cnet, QWidget *parent,
      bool allowLeftMouse) : QWidget(parent) {

    p_rotation = 0;
    p_timerOn = false;
    p_autoRegFact = NULL;
    p_allowLeftMouse = allowLeftMouse;

    //  Initialize some pointers
    p_leftCube = 0;
    p_rightCube = 0;
    p_leftGroundMap = 0;
    p_rightGroundMap = 0;

    try {
      p_templateFilename = "$base/templates/autoreg/qnetReg.def";
      Isis::Pvl pvl(p_templateFilename);
      p_autoRegFact = Isis::AutoRegFactory::Create(pvl);
    }
    catch(Isis::iException &e) {
      p_autoRegFact = NULL;
      e.Message(Isis::iException::Io,
                "Cannot create AutoRegFactory.  As a result, sub-pixel registration will not work.",
                _FILEINFO_);
      QString message = e.Errors().c_str();
      e.Clear();
      QMessageBox::information((QWidget *)parent, "Error", message);
    }

    createPointEditor(parent);
    emit newControlNetwork(cnet);
  }



  /**
   * Design the PointEdit widget
   *
   * @author  Tracie Sucharski
   * @internal
   *   @history 2008-11-19  Tracie Sucharski - Added left pan buttons, but
   *                           default to hidden.
   *   @history 2008-12-02  Jeannie Walldren - Allow connection
   *                           between updateLeftView and refreshView for all
   *                           objects.  Previously this was only done if
   *                           allowLeftMouse = true.
   *   @history 2008-12-02  Tracie Sucharski - Another bug fix due to change
   *                           on 2008-11-19, The leftView tackPointChanged
   *                           connection needs to always be made whether mouse
   *                           button events are allowed or not.
   *   @history 2008-12-10  Jeannie Walldren - Set the default
   *                           value of the new private variable,
   *                           p_templateFilename, to the previously hard-coded
   *                           template filename.
   */
  void ControlPointEdit::createPointEditor(QWidget *parent) {
    // Place everything in a grid
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSizeConstraint(QLayout::SetFixedSize);
    //  grid row
    int row = 0;

    std::string tempFilename = Isis::Filename("$base/icons").Expanded();
    QString toolIconDir = tempFilename.c_str();

    QSize isize(27, 27);
    //  Add zoom buttons
    QToolButton *leftZoomIn = new QToolButton();
    leftZoomIn->setIcon(QPixmap(toolIconDir + "/viewmag+.png"));
    leftZoomIn->setIconSize(isize);
    leftZoomIn->setToolTip("Zoom In");

    QToolButton *leftZoomOut = new QToolButton();
    leftZoomOut->setIcon(QPixmap(toolIconDir + "/viewmag-.png"));
    leftZoomOut->setIconSize(isize);
    leftZoomOut->setToolTip("Zoom Out");

    QToolButton *leftZoom1 = new QToolButton();
    leftZoom1->setIcon(QPixmap(toolIconDir + "/viewmag1.png"));
    leftZoom1->setIconSize(isize);
    leftZoom1->setToolTip("Zoom 1:1");

    QHBoxLayout *leftZoomPan = new QHBoxLayout;
    leftZoomPan->addWidget(leftZoomIn);
    leftZoomPan->addWidget(leftZoomOut);
    leftZoomPan->addWidget(leftZoom1);

    // These buttons only used if allow mouse events in leftViewport
    QToolButton *leftPanUp = 0;
    QToolButton *leftPanDown = 0;
    QToolButton *leftPanLeft = 0;
    QToolButton *leftPanRight = 0;
    if(p_allowLeftMouse) {
      //  Add arrows for panning
      leftPanUp = new QToolButton(parent);
      leftPanUp->setIcon(QIcon(Isis::Filename("$base/icons/up.png").
                               Expanded().c_str()));
      leftPanUp->setIconSize(isize);
      leftPanUp->setToolTip("Move up 1 screen pixel");

      leftPanDown = new QToolButton(parent);
      leftPanDown->setIcon(QIcon(Isis::Filename("$base/icons/down.png").
                                 Expanded().c_str()));
      leftPanDown->setIconSize(isize);
      leftPanDown->setToolTip("Move down 1 screen pixel");

      leftPanLeft = new QToolButton(parent);
      leftPanLeft->setIcon(QIcon(Isis::Filename("$base/icons/back.png").
                                 Expanded().c_str()));
      leftPanLeft->setIconSize(isize);
      leftPanLeft->setToolTip("Move left 1 screen pixel");

      leftPanRight = new QToolButton(parent);
      leftPanRight->setIcon(QIcon(Isis::Filename("$base/icons/forward.png").
                                  Expanded().c_str()));
      leftPanRight->setIconSize(isize);
      leftPanRight->setToolTip("Move right 1 screen pixel");

      leftZoomPan->addWidget(leftPanUp);
      leftZoomPan->addWidget(leftPanDown);
      leftZoomPan->addWidget(leftPanLeft);
      leftZoomPan->addWidget(leftPanRight);
    }

    leftZoomPan->addStretch();
    gridLayout->addLayout(leftZoomPan, row, 0);

    p_rightZoomIn = new QToolButton();
    p_rightZoomIn->setIcon(QPixmap(toolIconDir + "/viewmag+.png"));
    p_rightZoomIn->setIconSize(isize);
    p_rightZoomIn->setToolTip("Zoom In");
    QString text = "Zoom in 2X";
    p_rightZoomIn->setWhatsThis(text);

    p_rightZoomOut = new QToolButton();
    p_rightZoomOut->setIcon(QIcon(Isis::Filename("$base/icons/viewmag-.png").
                                  Expanded().c_str()));
    p_rightZoomOut->setIconSize(isize);
    p_rightZoomOut->setToolTip("Zoom Out");
    text = "Zoom out 2X";
    p_rightZoomOut->setWhatsThis(text);

    p_rightZoom1 = new QToolButton();
    p_rightZoom1->setIcon(QPixmap(toolIconDir + "/viewmag1.png"));
    p_rightZoom1->setIconSize(isize);
    p_rightZoom1->setToolTip("Zoom 1:1");
    text = "Zoom 1:1";
    p_rightZoom1->setWhatsThis(text);

    QHBoxLayout *rightZoomPan = new QHBoxLayout;
    rightZoomPan->addWidget(p_rightZoomIn);
    rightZoomPan->addWidget(p_rightZoomOut);
    rightZoomPan->addWidget(p_rightZoom1);

    //  Add arrows for panning
    QToolButton *rightPanUp = new QToolButton(parent);
    rightPanUp->setIcon(QIcon(Isis::Filename("$base/icons/up.png").
                              Expanded().c_str()));
    rightPanUp->setIconSize(isize);
    rightPanUp->setToolTip("Move up 1 screen pixel");

    QToolButton *rightPanDown = new QToolButton(parent);
    rightPanDown->setIcon(QIcon(Isis::Filename("$base/icons/down.png").
                                Expanded().c_str()));
    rightPanDown->setIconSize(isize);
    rightPanDown->setToolTip("Move down 1 screen pixel");

    QToolButton *rightPanLeft = new QToolButton(parent);
    rightPanLeft->setIcon(QIcon(Isis::Filename("$base/icons/back.png").
                                Expanded().c_str()));
    rightPanLeft->setIconSize(isize);
    rightPanLeft->setToolTip("Move left 1 screen pixel");

    QToolButton *rightPanRight = new QToolButton(parent);
    rightPanRight->setIcon(QIcon(Isis::Filename("$base/icons/forward.png").
                                 Expanded().c_str()));
    rightPanRight->setIconSize(isize);
    rightPanRight->setToolTip("Move right 1 screen pixel");

    rightZoomPan->addWidget(rightPanUp);
    rightZoomPan->addWidget(rightPanDown);
    rightZoomPan->addWidget(rightPanLeft);
    rightZoomPan->addWidget(rightPanRight);
    rightZoomPan->addStretch();

    gridLayout->addLayout(rightZoomPan, row++, 1);

    //  Add zoom factor label
    p_leftZoomFactor = new QLabel();
    gridLayout->addWidget(p_leftZoomFactor, row, 0);
    p_rightZoomFactor = new QLabel();
    gridLayout->addWidget(p_rightZoomFactor, row++, 1);


    p_leftView = new ChipViewport(VIEWSIZE, VIEWSIZE, this);
    //  Do not want to accept mouse/keyboard events
    if(!p_allowLeftMouse) p_leftView->setDisabled(true);

    gridLayout->addWidget(p_leftView, row, 0);
    
    connect(this, SIGNAL(newControlNetwork(Isis::ControlNet *)),
            p_leftView, SLOT(setControlNet(Isis::ControlNet *)));
    
    connect(this,
        SIGNAL(stretchChipViewport(Isis::Stretch *, Qisis::CubeViewport *)),
        p_leftView,
        SLOT(stretchFromCubeViewport(Isis::Stretch *, Qisis::CubeViewport *)));

    //  Connect left zoom buttons to ChipViewport's zoom slots
    connect(leftZoomIn, SIGNAL(clicked()), p_leftView, SLOT(zoomIn()));
    connect(leftZoomOut, SIGNAL(clicked()), p_leftView, SLOT(zoomOut()));
    connect(leftZoom1, SIGNAL(clicked()), p_leftView, SLOT(zoom1()));

    //  If zoom on left, need to re-geom right
    connect(leftZoomIn, SIGNAL(clicked()), this, SLOT(updateRightGeom()));
    connect(leftZoomOut, SIGNAL(clicked()), this, SLOT(updateRightGeom()));
    connect(leftZoom1, SIGNAL(clicked()), this, SLOT(updateRightGeom()));

    //  Connect the ChipViewport tackPointChanged signal to
    //  the update sample/line label
    connect(p_leftView, SIGNAL(tackPointChanged(double)),
            this, SLOT(updateLeftPositionLabel(double)));
    // we want to allow this connection so that if a changed point is saved
    // and the same image is showing in both viewports, the left will refresh.
    connect(this, SIGNAL(updateLeftView(double, double)),
            p_leftView, SLOT(refreshView(double, double)));

    if(p_allowLeftMouse) {
      //  Connect pan buttons to ChipViewport
      connect(leftPanUp, SIGNAL(clicked()), p_leftView, SLOT(panUp()));
      connect(leftPanDown, SIGNAL(clicked()), p_leftView, SLOT(panDown()));
      connect(leftPanLeft, SIGNAL(clicked()), p_leftView, SLOT(panLeft()));
      connect(leftPanRight, SIGNAL(clicked()), p_leftView, SLOT(panRight()));
    }

    p_rightView = new ChipViewport(VIEWSIZE, VIEWSIZE, this);
    gridLayout->addWidget(p_rightView, row, 1);
    
    connect(this, SIGNAL(newControlNetwork(Isis::ControlNet *)),
            p_rightView, SLOT(setControlNet(Isis::ControlNet *)));
    
    connect(this,
        SIGNAL(stretchChipViewport(Isis::Stretch *, Qisis::CubeViewport *)),
        p_rightView,
        SLOT(stretchFromCubeViewport(Isis::Stretch *, Qisis::CubeViewport *)));
            
            
    //  Connect the ChipViewport tackPointChanged signal to
    //  the update sample/line label
    connect(p_rightView, SIGNAL(tackPointChanged(double)),
            this, SLOT(updateRightPositionLabel(double)));
    connect(this, SIGNAL(updateRightView(double, double)),
            p_rightView, SLOT(refreshView(double, double)));


    connect(p_rightZoomIn, SIGNAL(clicked()), p_rightView, SLOT(zoomIn()));
    connect(p_rightZoomOut, SIGNAL(clicked()), p_rightView, SLOT(zoomOut()));
    connect(p_rightZoom1, SIGNAL(clicked()), p_rightView, SLOT(zoom1()));

    //  Connect pan buttons to ChipViewport
    connect(rightPanUp, SIGNAL(clicked()), p_rightView, SLOT(panUp()));
    connect(rightPanDown, SIGNAL(clicked()), p_rightView, SLOT(panDown()));
    connect(rightPanLeft, SIGNAL(clicked()), p_rightView, SLOT(panLeft()));
    connect(rightPanRight, SIGNAL(clicked()), p_rightView, SLOT(panRight()));

    //  Create chips for left and right
    p_leftChip = new Isis::Chip(VIEWSIZE, VIEWSIZE);
    p_rightChip = new Isis::Chip(VIEWSIZE, VIEWSIZE);

    p_nogeom = new QRadioButton("No geom");
    p_nogeom->setChecked(true);
    p_geom   = new QRadioButton("Geom");
    QRadioButton *rotate = new QRadioButton("Rotate");
    QButtonGroup *bgroup = new QButtonGroup();
    bgroup->addButton(p_nogeom);
    bgroup->addButton(p_geom);
    bgroup->addButton(rotate);

    connect(p_nogeom, SIGNAL(clicked()), this, SLOT(setNoGeom()));
    connect(p_geom, SIGNAL(clicked()), this, SLOT(setGeom()));

    //  TODO:  ?? Don't think we need this connection
    connect(rotate, SIGNAL(clicked()), this, SLOT(setRotate()));

    //  Set some defaults
    p_geomIt = false;
    p_rotation = 0;

    p_dial = new QDial();
    p_dial->setRange(0, 360);
    p_dial->setWrapping(false);
    p_dial->setNotchesVisible(true);
    p_dial->setNotchTarget(5.);
    p_dial->setEnabled(false);

    p_dialNumber = new QLCDNumber();
    p_dialNumber->setEnabled(false);
    connect(p_dial, SIGNAL(valueChanged(int)), p_dialNumber, SLOT(display(int)));
    connect(p_dial, SIGNAL(valueChanged(int)), p_rightView, SLOT(rotateChip(int)));

    QCheckBox *cross = new QCheckBox("Show crosshair");
    connect(cross, SIGNAL(toggled(bool)), p_leftView, SLOT(setCross(bool)));
    connect(cross, SIGNAL(toggled(bool)), p_rightView, SLOT(setCross(bool)));
    cross->setChecked(true);

    QCheckBox *circle = new QCheckBox("Circle");
    circle->setChecked(false);
    connect(circle, SIGNAL(toggled(bool)), this, SLOT(setCircle(bool)));

    p_slider = new QScrollBar(Qt::Horizontal);
    p_slider->setRange(1, 100);
    p_slider->setSingleStep(1);
    connect(p_slider, SIGNAL(valueChanged(int)), p_leftView, SLOT(setCircleSize(int)));
    connect(p_slider, SIGNAL(valueChanged(int)), p_rightView, SLOT(setCircleSize(int)));
    p_slider->setValue(20);
    p_slider->setDisabled(true);
    p_slider->hide();

    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->addWidget(p_nogeom);
    vlayout->addWidget(p_geom);
    vlayout->addWidget(rotate);
    vlayout->addWidget(p_dial);
    vlayout->addWidget(p_dialNumber);
    vlayout->addWidget(cross);
    vlayout->addWidget(circle);
    vlayout->addWidget(p_slider);
    gridLayout->addLayout(vlayout, row++, 2);

    //  Show sample / line for measure of chips shown
    p_leftSampLinePosition = new QLabel();
    gridLayout->addWidget(p_leftSampLinePosition, row, 0);
    p_rightSampLinePosition = new QLabel();
    gridLayout->addWidget(p_rightSampLinePosition, row++, 1);

    //  Show lat / lon for measure of chips shown
    p_leftLatLonPosition = new QLabel();
    gridLayout->addWidget(p_leftLatLonPosition, row, 0);
    p_rightLatLonPosition = new QLabel();
    gridLayout->addWidget(p_rightLatLonPosition, row++, 1);


    //  Add auto registration extension
    p_autoRegExtension = new QWidget;
    p_oldPosition = new QLabel;
    p_goodFit = new QLabel;
    QVBoxLayout *autoRegLayout = new QVBoxLayout;
    autoRegLayout->setMargin(0);
    autoRegLayout->addWidget(p_oldPosition);
    autoRegLayout->addWidget(p_goodFit);
    p_autoRegExtension->setLayout(autoRegLayout);
    p_autoRegShown = false;
    p_autoRegAttempted = false;
    gridLayout->addWidget(p_autoRegExtension, row++, 1);


    QHBoxLayout *leftLayout = new QHBoxLayout();
    QToolButton *stop = new QToolButton();
    stop->setIcon(QPixmap(toolIconDir + "/blinkStop.png"));
    stop->setIconSize(QSize(22, 22));
    stop->setToolTip("Blink Stop");
    text = "<b>Function:</b> Stop automatic timed blinking";
    stop->setWhatsThis(text);
    connect(stop, SIGNAL(released()), this, SLOT(blinkStop()));

    QToolButton *start = new QToolButton();
    start->setIcon(QPixmap(toolIconDir + "/blinkStart.png"));
    start->setIconSize(QSize(22, 22));
    start->setToolTip("Blink Start");
    text = "<b>Function:</b> Start automatic timed blinking.  Cycles \
           through linked viewports at variable rate";
    start->setWhatsThis(text);
    connect(start, SIGNAL(released()), this, SLOT(blinkStart()));

    p_blinkTimeBox = new QDoubleSpinBox();
    p_blinkTimeBox->setMinimum(0.1);
    p_blinkTimeBox->setMaximum(5.0);
    p_blinkTimeBox->setDecimals(1);
    p_blinkTimeBox->setSingleStep(0.1);
    p_blinkTimeBox->setValue(0.5);
    p_blinkTimeBox->setToolTip("Blink Time Delay");
    text = "<b>Function:</b> Change automatic blink rate between " +
           QString::number(p_blinkTimeBox->minimum()) + " and " +
           QString::number(p_blinkTimeBox->maximum()) + " seconds";
    p_blinkTimeBox->setWhatsThis(text);
    connect(p_blinkTimeBox, SIGNAL(valueChanged(double)),
            this, SLOT(changeBlinkTime(double)));

    QPushButton *find = new QPushButton("Find");

    leftLayout->addWidget(stop);
    leftLayout->addWidget(start);
    leftLayout->addWidget(p_blinkTimeBox);
    leftLayout->addWidget(find);
    leftLayout->addStretch();
    //  TODO: Move AutoPick to NewPointDialog
//    QPushButton *autopick = new QPushButton ("AutoPick");
//    autopick->setFocusPolicy(Qt::NoFocus);
//    QPushButton *blink = new QPushButton ("Blink");
//    blink->setFocusPolicy(Qt::NoFocus);
//    leftLayout->addWidget(autopick);
//    leftLayout->addWidget(blink);
    gridLayout->addLayout(leftLayout, row, 0);


    QHBoxLayout *rightLayout = new QHBoxLayout();
    p_autoReg = new QPushButton("Register");
    QPushButton *save = new QPushButton("Save Point");

    rightLayout->addWidget(p_autoReg);
    rightLayout->addWidget(save);
    rightLayout->addStretch();
    gridLayout->addLayout(rightLayout, row, 1);

    connect(find, SIGNAL(clicked()), this, SLOT(findPoint()));
    connect(p_autoReg, SIGNAL(clicked()), this, SLOT(registerPoint()));
    connect(save, SIGNAL(clicked()), this, SLOT(savePoint()));

    setLayout(gridLayout);

    //p_pointEditor->setCentralWidget(cw);
    p_autoRegExtension->hide();

    //p_pointEditor->setShown(true);
    //p_pointEditor->raise();
  }




  /**
   * Set the measure displayed in the left ChipViewport
   *
   * @param leftMeasure  Input  Measure displayed in left ChipViewport
   * @param leftCube     Input  Cube of measure displayed in left ChipViewport
   *
   * @author Tracie Sucharski
   * @internal
   *   @history 2008-11-19  Tracie Sucharski - If left cube changes, get new
   *                           universalGroundMap.
   */
  void ControlPointEdit::setLeftMeasure(Isis::ControlMeasure *leftMeasure,
                                        Isis::Cube *leftCube, std::string pointId) {

    //  Make sure registration is turned off
    if(p_autoRegShown) {
      //  Undo Registration
      p_autoRegShown = false;
      p_autoRegExtension->hide();
      p_autoReg->setText("Register");
    }

    p_leftMeasure = leftMeasure;
    p_pointId = pointId;

    //  get new ground map
    if(p_leftGroundMap != 0) delete p_leftGroundMap;
    p_leftGroundMap = new Isis::UniversalGroundMap(*leftCube);
    p_leftCube = leftCube;

    p_leftChip->TackCube(p_leftMeasure->Sample(), p_leftMeasure->Line());
    p_leftChip->Load(*p_leftCube);

    // Dump into left chipViewport
    p_leftView->setChip(p_leftChip, p_leftCube);

  }


  /**
   * Set the measure displayed in the right ChipViewport
   *
   *
   * @param rightMeasure  Input  Measure displayed in right ChipViewport
   * @param rightCube     Input  Cube of measure displayed in right ChipViewport
   *
   * @author Tracie Sucharski
   * @internal
   *   @history 2008-11-19  Tracie Sucharski - If right cube changes, get new
   *                           universalGroundMap.
   *   @history 2008-15-2008 Jeannie Walldren - Added error string to
  *                             Isis::iException::Message before
  *                             creating QMessageBox
   *   @history 2009-09-14  Tracie Sucharski - Call geomChip to make
   *                           sure left chip is initialized in the
   *                           ChipViewport.  This was done for the changes
   *                           made to the Chip class and the ChipViewport
   *                           class where the Cube info is no longer stored.
  *
    */
  void ControlPointEdit::setRightMeasure(Isis::ControlMeasure *rightMeasure,
                                         Isis::Cube *rightCube, std::string pointId) {

    //  Make sure registration is turned off
    if(p_autoRegShown) {
      //  Undo Registration
      p_autoRegShown = false;
      p_autoRegExtension->hide();
      p_autoReg->setText("Register");
    }
    p_autoRegAttempted = false;

    p_rightMeasure = rightMeasure;
    p_pointId = pointId;

    //  get new ground map
    if(p_rightGroundMap != 0) delete p_rightGroundMap;
    p_rightGroundMap = new Isis::UniversalGroundMap(*rightCube);
    p_rightCube = rightCube;

    p_rightChip->TackCube(p_rightMeasure->Sample(), p_rightMeasure->Line());
    if(p_geomIt == false) {
      p_rightChip->Load(*p_rightCube);
    }
    else {
      try {
        p_rightChip->Load(*p_rightCube, *p_leftChip, *p_leftCube);

      }
      catch(Isis::iException &e) {
        e.Message(Isis::iException::User, "Geom failed.", _FILEINFO_);
        QString message = e.Errors().c_str();
        e.Clear();
        QMessageBox::information((QWidget *)parent(), "Error", message);
        p_rightChip->Load(*p_rightCube);
        p_geomIt = false;
        p_nogeom->setChecked(true);
        p_geom->setChecked(false);
      }
    }

    // Dump into left chipViewport
    p_rightView->setChip(p_rightChip, p_rightCube);
    updateRightGeom();
    //p_rightView->geomChip(p_leftChip,p_leftCube);

  }


  /**
   * Update sample/line, lat/lon and zoom factor of left measure
   *
   * @param zoomFactor  Input  zoom factor
   *
   * @author Tracie Sucharski
   */
  void ControlPointEdit::updateLeftPositionLabel(double zoomFactor) {
    QString pos = "Sample: " + QString::number(p_leftView->tackSample()) +
                  "    Line:  " + QString::number(p_leftView->tackLine());
    p_leftSampLinePosition->setText(pos);

    //  Get lat/lon from point in left
    p_leftGroundMap->SetImage(p_leftView->tackSample(), p_leftView->tackLine());
    double lat = p_leftGroundMap->UniversalLatitude();
    double lon = p_leftGroundMap->UniversalLongitude();

    pos = "Latitude: " + QString::number(lat) +
          "    Longitude:  " + QString::number(lon);
    p_leftLatLonPosition->setText(pos);

    //  Print zoom scale factor
    pos = "Zoom Factor: " + QString::number(zoomFactor);
    p_leftZoomFactor->setText(pos);

  }


  /**
   * Update sample/line, lat/lon and zoom factor of right measure
   *
   * @param zoomFactor  Input  zoom factor
   *
   * @author Tracie Sucharski
   */
  void ControlPointEdit::updateRightPositionLabel(double zoomFactor) {
    QString pos = "Sample: " + QString::number(p_rightView->tackSample()) +
                  "    Line:  " + QString::number(p_rightView->tackLine());
    p_rightSampLinePosition->setText(pos);

    //  Get lat/lon from point in right
    p_rightGroundMap->SetImage(p_rightView->tackSample(), p_rightView->tackLine());
    double lat = p_rightGroundMap->UniversalLatitude();
    double lon = p_rightGroundMap->UniversalLongitude();

    pos = "Latitude: " + QString::number(lat) +
          "    Longitude:  " + QString::number(lon);
    p_rightLatLonPosition->setText(pos);

    //  Print zoom scale factor
    pos = "Zoom Factor: " + QString::number(zoomFactor);
    p_rightZoomFactor->setText(pos);

  }


  //! Find point from left ChipViewport in the right ChipViewport
  void ControlPointEdit::findPoint() {

    //  Get lat/lon from point in left
    p_leftGroundMap->SetImage(p_leftView->tackSample(), p_leftView->tackLine());
    double lat = p_leftGroundMap->UniversalLatitude();
    double lon = p_leftGroundMap->UniversalLongitude();

    //  Reload right chipViewport with this new tack point.
    if(p_rightGroundMap->SetUniversalGround(lat, lon))
      emit updateRightView(p_rightGroundMap->Sample(), p_rightGroundMap->Line());

  }

  /**
   * Sub-pixel register point in right chipViewport with point in
   * left.
   * @internal
   *   @history 2008-15-2008  Jeannie Walldren - Throw and catch
   *            error before creating QMessageBox
   *   @history 2009-03-23  Tracie Sucharski - Added private p_autoRegAttempted
   *                             for the SaveChips method.
   *   @history 2010-02-16  Tracie Sucharski- If autoreg fails,
   *                             print registration stats.
   *   @history 2010-02-18  Tracie Sucharski - Registration stats wasn't the
   *                             correct info to print.  Instead, check
   *                             registrationStatus and print separate errors
   *                             for each possibility.
   *   @history 2010-02-22  Tracie Sucharski - Added more info for registration
   *                             failures.
   *   @history 2010-06-08  Jeannie Walldren - Catch error and
   *                             warn user if unable to load
   *                             pattern (left) or search (right)
   *                             chip
   *
   */

  void ControlPointEdit::registerPoint() {

    if(p_autoRegShown) {
      //  Undo Registration
      p_autoRegShown = false;
      p_autoRegExtension->hide();
      p_autoReg->setText("Register");

      //  Reload chip with original measure
      emit updateRightView(p_rightMeasure->Sample(), p_rightMeasure->Line());
      return;

    }
    p_autoRegAttempted = true;

    try {
      p_autoRegFact->PatternChip()->TackCube(
        p_leftMeasure->Sample(), p_leftMeasure->Line());
      p_autoRegFact->PatternChip()->Load(*p_leftCube);
      p_autoRegFact->SearchChip()->TackCube(
        p_rightMeasure->Sample(), p_rightMeasure->Line());
      p_autoRegFact->SearchChip()->Load(*p_rightCube, *(p_autoRegFact->PatternChip()), *p_leftCube);
      Isis::AutoReg::RegisterStatus status = p_autoRegFact->Register();
      if(status != Isis::AutoReg::Success) {
        throw Isis::iException::Message(Isis::iException::User, "Autoregistration failed", _FILEINFO_);
      }
    }
    catch(Isis::iException &e) {
      Isis::AutoReg::RegisterStatus status = p_autoRegFact->Register();
      QString msg = "Cannot sub-pixel register this point.\n";
      msg += e.Errors().c_str();
      if(status != Isis::AutoReg::Success) {
        if(status == Isis::AutoReg::PatternChipNotEnoughValidData) {
          msg += "\n\nNot enough valid data in Pattern Chip.\n";
          msg += "  PatternValidPercent = ";
          msg += QString::number(p_autoRegFact->PatternValidPercent()) + "%";
        }
        else if(status == Isis::AutoReg::FitChipNoData) {
          msg += "\n\nNo valid data in Fit Chip.";
        }
        else if(status == Isis::AutoReg::FitChipToleranceNotMet) {
          msg += "\n\nGoodness of Fit Tolerance not met.\n";
          msg += "\nGoodnessOfFit = " + QString::number(p_autoRegFact->GoodnessOfFit());
          msg += "\nGoodnessOfFitTolerance = ";
          msg += QString::number(p_autoRegFact->Tolerance());
        }
        else if(status == Isis::AutoReg::SurfaceModelNotEnoughValidData) {
          msg += "\n\nNot enough points to fit a surface model for sub-pixel ";
          msg += "accuracy.  Probably too close to edge.\n";
        }
        else if(status == Isis::AutoReg::SurfaceModelSolutionInvalid) {
          msg += "\n\nCould not model surface for sub-pixel accuracy.\n";
        }
        else if(status == Isis::AutoReg::SurfaceModelDistanceInvalid) {
          double sampDist, lineDist;
          p_autoRegFact->Distance(sampDist, lineDist);
          msg += "\n\nSurface model moves registartion more than tolerance.\n";
          msg += "\nSampleMovement = " + QString::number(sampDist) +
                 "    LineMovement = " + QString::number(lineDist);
          msg += "\nDistanceTolerance = " +
                 QString::number(p_autoRegFact->DistanceTolerance());
        }
        else if(status == Isis::AutoReg::PatternZScoreNotMet) {
          double score1, score2;
          p_autoRegFact->ZScores(score1, score2);
          msg += "\n\nPattern data max or min does not pass z-score test.\n";
          msg += "\nMinimumZScore = " + QString::number(p_autoRegFact->MinimumZScore());
          msg += "\nCalculatedZscores = " + QString::number(score1) + ", " + QString::number(score2);
        }
        else if(status == Isis::AutoReg::SurfaceModelEccentricityRatioNotMet) {
          msg += "\n\nEccentricity of surface model exceeds tolerance.";
          QString calcEccentricity = QString::number(p_autoRegFact->EccentricityRatio(), 'f', 5);
          msg += "\nCalculated Eccentricity Ratio = " +
                 calcEccentricity + " (" + calcEccentricity + ":1)";
          QString tolEccentricity = QString::number(p_autoRegFact->EccentricityRatioTolerance(), 'f', 5);
          msg += "\nEccentricity Ratio Tolerance (i.e., EccentricityRatio) = " +
                 tolEccentricity + " (" + tolEccentricity + ":1)";
        }
        else if(status == Isis::AutoReg::AdaptiveAlgorithmFailed) {
          msg += "\n\nError occured in Adaptive algorithm.";
        }
        else {
          msg += "\n\nUnknown registration error.";
        }
      }

      QMessageBox::information((QWidget *)parent(), "Error", msg);
      e.Clear();
      return;
    }



    //  Load chip with new registered point
    emit updateRightView(p_autoRegFact->CubeSample(), p_autoRegFact->CubeLine());


    QString oldPos = "Original Sample: " +
                     QString::number(p_rightMeasure->Sample()) + "   Original Line:  " +
                     QString::number(p_rightMeasure->Line());
    p_oldPosition->setText(oldPos);

    QString goodFit = "Goodness of Fit:  " +
                      QString::number(p_autoRegFact->GoodnessOfFit());
    p_goodFit->setText(goodFit);

    p_autoRegExtension->show();
    p_autoRegShown = true;
    p_autoReg->setText("Undo Registration");
  }

  /**
   * Save point under the crosshair in right ChipViewport
   * @internal
   *   @history 2008-12-30 Jeannie Walldren - Modified to update
   *                          user (chooser) name and date when
   *                          point is saved
   */
  void ControlPointEdit::savePoint() {
    //  Get cube position at right chipViewport crosshair
    if(p_rightMeasure != NULL) {
      p_rightMeasure->SetCoordinate(p_rightView->tackSample(),
                                    p_rightView->tackLine());
      p_rightMeasure->SetDateTime();
      p_rightMeasure->SetChooserName();
    }
    if(p_allowLeftMouse) {
      if(p_leftMeasure != NULL)
        p_leftMeasure->SetCoordinate(p_leftView->tackSample(), p_leftView->tackLine());
      p_leftMeasure->SetDateTime();
      p_leftMeasure->SetChooserName();
    }

    //  If the right chip is the same as the left chip , update the left
    //  chipViewport.
    if(p_rightMeasure->CubeSerialNumber() == p_leftMeasure->CubeSerialNumber()) {
      p_leftMeasure->SetCoordinate(p_rightView->tackSample(),
                                   p_rightView->tackLine());
      p_leftMeasure->SetDateTime();
      p_leftMeasure->SetChooserName();
      emit updateLeftView(p_rightView->tackSample(), p_rightView->tackLine());
    }


    //  If autoReg is shown
    if(p_autoRegShown) {
      p_autoRegShown = false;
      p_autoRegExtension->hide();
      p_autoReg->setText("Register");
    }


    //  Redraw measures and overlaps on viewports
    emit pointSaved();
  }



  /**
   * Slot to update the geomed right ChipViewport for zoom
   * operations
   * @internal
   *   @history 2008-15-2008  Jeannie Walldren - Added error string to
   *                            Isis::iException::Message before
   *                            creating QMessageBox
   */
  void ControlPointEdit::updateRightGeom() {

    if(p_geomIt) {
      try {
        p_rightView->geomChip(p_leftChip, p_leftCube);

      }
      catch(Isis::iException &e) {
        e.Message(Isis::iException::User, "Geom failed.", _FILEINFO_);
        QString message = e.Errors().c_str();
        e.Clear();
        QMessageBox::information((QWidget *)parent(), "Error", message);
        p_geomIt = false;
        p_nogeom->setChecked(true);
        p_geom->setChecked(false);
      }
    }

  }


  //!  Slot to enable the rotate dial
  void ControlPointEdit::setRotate() {
    p_dial->setEnabled(true);
    p_dialNumber->setEnabled(true);
    p_dial->setNotchesVisible(true);

  }



  /**
   * Turn geom on
   *
   * @internal
   *   @history  2007-06-15 Tracie Sucharski - Grey out zoom buttons
   *   @history 2008-15-2008 Jeannie Walldren - Added error string to
   *                            Isis::iException::Message before
   *                            creating QMessageBox
   **/
  void ControlPointEdit::setGeom() {

    if(p_geomIt == true) return;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    //  Grey right view zoom buttons
    QString text = "Zoom functions disabled when Geom is set";
    p_rightZoomIn->setEnabled(false);
    p_rightZoomIn->setWhatsThis(text);
    p_rightZoomIn->setToolTip(text);
    p_rightZoomOut->setEnabled(false);
    p_rightZoomOut->setWhatsThis(text);
    p_rightZoomOut->setToolTip(text);
    p_rightZoom1->setEnabled(false);
    p_rightZoom1->setWhatsThis(text);
    p_rightZoom1->setToolTip(text);


    //  Reset dial to 0 before disabling
    p_dial->setValue(0);
    p_dial->setEnabled(false);
    p_dialNumber->setEnabled(false);

    p_geomIt = true;

    try {
      p_rightView->geomChip(p_leftChip, p_leftCube);

    }
    catch(Isis::iException &e) {
      e.Message(Isis::iException::User, "Geom failed.", _FILEINFO_);
      QString message = e.Errors().c_str();
      e.Clear();
      QMessageBox::information((QWidget *)parent(), "Error", message);
      p_geomIt = false;
      p_nogeom->setChecked(true);
      p_geom->setChecked(false);
    }

    QApplication::restoreOverrideCursor();
  }


  //!  Slot to turn off geom
  void ControlPointEdit::setNoGeom() {

    if(p_geomIt == false) return;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    QString text = "Zoom in 2X";
    p_rightZoomIn->setEnabled(true);
    p_rightZoomIn->setWhatsThis(text);
    p_rightZoomIn->setToolTip("Zoom In");
    text = "Zoom out 2X";
    p_rightZoomOut->setEnabled(true);
    p_rightZoomOut->setWhatsThis(text);
    p_rightZoomOut->setToolTip("Zoom Out");
    text = "Zoom 1:1";
    p_rightZoom1->setEnabled(true);
    p_rightZoom1->setWhatsThis(text);
    p_rightZoom1->setToolTip("Zoom 1:1");

    //  Reset dial to 0 before disabling
    p_dial->setValue(0);
    p_dial->setEnabled(false);
    p_dialNumber->setEnabled(false);

    p_geomIt = false;
    p_rightView->nogeomChip();

    QApplication::restoreOverrideCursor();
  }




  /**
   * Turn circle widgets on/off
   *
   * @param checked   Input   Turn cirle on or off
   *
   * @author 2008-09-09 Tracie Sucharski
   */
  void ControlPointEdit::setCircle(bool checked) {

    if(checked == p_circle) return;

    p_circle = checked;
    if(p_circle) {
      // Turn on slider bar
      p_slider->setDisabled(false);
      p_slider->show();
      p_slider->setValue(20);
      p_leftView->setCircle(true);
      p_rightView->setCircle(true);
    }
    else {
      p_slider->setDisabled(true);
      p_slider->hide();
      p_leftView->setCircle(false);
      p_rightView->setCircle(false);
    }


  }



  //!  Slot to start blink function
  void ControlPointEdit::blinkStart() {
    if(p_timerOn) return;

    //  Set up blink list
    p_blinkList.push_back(p_leftView);
    p_blinkList.push_back(p_rightView);
    p_blinkIndex = 0;

    p_timerOn = true;
    int msec = (int)(p_blinkTimeBox->value() * 1000.0);
    p_timer = new QTimer(this);
    connect(p_timer, SIGNAL(timeout()), this, SLOT(updateBlink()));
    p_timer->start(msec);
  }


  //!  Slot to stop blink function
  void ControlPointEdit::blinkStop() {
    p_timer->stop();
    p_timerOn = false;
    p_blinkList.clear();

    //  Reload left chipViewport with original chip
    p_leftView->repaint();

  }



  /**
   * Set blink rate
   *
   * @param interval   Input   Blink rate in seconds
   * @author  Tracie Sucharski
   */
  void ControlPointEdit::changeBlinkTime(double interval) {
    if(p_timerOn) p_timer->setInterval((int)(interval * 1000.));
  }


  //!  Slot to cause the blink to happen coinciding with the timer
  void ControlPointEdit::updateBlink() {

    p_blinkIndex = !p_blinkIndex;
    p_leftView->loadView(*(p_blinkList)[p_blinkIndex]);
  }




  /**
   * Allows user to choose a new template file by opening a window
   * from which to select a filename.  This file is then
   * registered and set as the new template.
   *
   * @author Tracie Sucharski
   * @internal
   *   @history 2008-12-10 Jeannie Walldren - Changed name from
   *                          openTemplateFile() and added functionality to set
   *                          new template filename to the private variable,
   *                          p_templateFilename
   *   @history 2008-12-15 Jeannie Walldren - Modified code to
   *                          only allow the template file to be modified if
   *                          registration is successfull, otherwise the
   *                          original template file is kept.
   */
  void ControlPointEdit::setTemplateFile() {

    QString filter = "Select registration template (*.def *.pvl);;";
    filter += "All (*)";
    QString regDef = QFileDialog::getOpenFileName((QWidget *)parent(),
                     "Select a registration template",
                     ".",
                     filter);
    if(regDef.isEmpty()) return;
    Isis::AutoReg *reg = NULL;
    // save original template filename
    std::string temp = p_templateFilename;
    try {
      // set template filename to user chosen pvl file
      p_templateFilename = regDef.toStdString();
      // Create PVL object with this file
      Isis::Pvl pvl(regDef.toStdString());
      // try to register file
      reg = Isis::AutoRegFactory::Create(pvl);
      if(p_autoRegFact != NULL) delete p_autoRegFact;
      p_autoRegFact = reg;
    }
    catch(Isis::iException &e) {
      // set templateFilename back to its original value
      p_templateFilename = temp;
      e.Message(Isis::iException::Io, "Cannot create AutoRegFactory for "
                + regDef.toStdString() + ".  As a result, current template file will remain set to "
                + p_templateFilename, _FILEINFO_);
      QString message = e.Errors().c_str();
      e.Clear();
      QMessageBox::information((QWidget *)parent(), "Error", message);
    }
  }


  /**
   * Set the option that allows mouse movements in the left ChipViewport.
   *
   * @author Tracie Sucharski
   * @internal
   */
  void ControlPointEdit::allowLeftMouse(bool allowMouse) {
    p_allowLeftMouse = allowMouse;
  }




  /**
   * Slot to save registration chips to files and fire off qview.
   *
   * @author 2009-03-18  Tracie Sucharski
   * @internal
   * @history 2009-03-23 Tracie Sucharski - Use p_autoRegAttempted to see
   *                        if chips exist.
   */
  void ControlPointEdit::saveChips() {

//    if (!p_autoRegShown) {
    if(!p_autoRegAttempted) {
      QString message = "Point must be Registered before chips can be saved.";
      QMessageBox::warning((QWidget *)parent(), "Warning", message);
      return;
    }

    //  Save chips - pattern, search and fit
    std::string baseFile = p_pointId + "_" +
                           Isis::iString((int)(p_leftMeasure->Sample())) + "_" +
                           Isis::iString((int)(p_leftMeasure->Line())) + "_" +
                           Isis::iString((int)(p_rightMeasure->Sample())) + "_" +
                           Isis::iString((int)(p_rightMeasure->Line())) + "_";
    std::string fname = baseFile + "Search.cub";
    std::string command = "$ISISROOT/bin/qview " + fname;
    p_autoRegFact->SearchChip()->Write(fname);
    fname = baseFile + "Pattern.cub";
    command += " " + fname;
    p_autoRegFact->PatternChip()->Write(fname);
    fname = baseFile + "Fit.cub";
    command += " " + fname + "&";
    p_autoRegFact->FitChip()->Write(fname);
    Isis::System(command);
  }

#if 0
  void ControlPointEdit::setInterestOp() {



  }
#endif

}

