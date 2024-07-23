#include "MosaicZoomTool.h"

#include <float.h>
#include <iomanip>
#include <iostream>

#include <QDoubleSpinBox>
#include <QGraphicsSceneMouseEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QScrollBar>
#include <QToolButton>

#include "IString.h"
#include "MosaicGraphicsView.h"
#include "MosaicSceneWidget.h"
#include "Projection.h"

namespace Isis {
  /**
   * MosaicZoomTool constructor
   *
   *
   * @param parent
   */
  MosaicZoomTool::MosaicZoomTool(MosaicSceneWidget *scene) : MosaicTool(scene) {
    p_scaleBox = NULL;

    m_zoomInAction = new QAction(this);
    m_zoomInAction->setIcon(getIcon("viewmag+.png"));
    m_zoomInAction->setText("Zoom In");
    m_zoomInAction->setToolTip("Zoom in on the mosaic scene");
    m_zoomInAction->setShortcut(Qt::Key_Plus);
    QString text =
      "<b>Function:</b> Zoom in 2X at the center of the active viewport \
      <p><b>Shortcut:</b>  +</p> \
      <p><b>Mouse:</b>  LeftButton zooms in 2X under pointer</p> \
      <p><b>Modifiers:</b>  Shortcuts and mouse clicks can be augmented \
      using the Ctrl or Alt key for 4X and 8X zooms, respectively</p> \
      <p><b>Hint:</b>  Left click and drag for a local zoom which scales data \
      in the marquee to the view</p>";
    m_zoomInAction->setWhatsThis(text);
    connect(m_zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn2X()));

    m_zoomOutAction = new QAction(this);
    m_zoomOutAction->setIcon(getIcon("viewmag-.png"));
    m_zoomOutAction->setText("Zoom Out");
    m_zoomOutAction->setToolTip("Zoom out on the mosaic scene");
    m_zoomOutAction->setShortcut(Qt::Key_Minus);
    text =
      "<b>Function:</b> Zoom out 2X at the center of the view \
      <p><b>Shortcut:</b>  +</p> \
      <p><b>Mouse:</b>  RightButton zooms out 2X under pointer</p> \
      <p><b>Modifiers:</b>  Shortcuts and mouse clicks can be augmented \
      using the Ctrl or Alt key for 4X and 8X zooms, respectively</p> \
      <p><b>Hint:</b>  Left click and drag for a local zoom which scales data \
      in the marquee to the view</p>";
    m_zoomOutAction->setWhatsThis(text);
    connect(m_zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut2X()));

    m_zoomFitAction = new QAction(this);
    m_zoomFitAction->setIcon(getIcon("viewmagfit.png"));
    m_zoomFitAction->setText("Fit in View");
    m_zoomFitAction->setToolTip("Zoom to where all of the cubes are visible in "
                                "the mosaic scene");
    m_zoomFitAction->setShortcut(Qt::Key_Asterisk);
    text =
      "<b>Function:</b> Fit the entire mosaic inside the view. \
      <p><b>Shortcut:</b> *</p> \
      <p><b>Hint:</b>  Many shortcuts for the zoom tool and other tools \
      are easily available on the numeric keypad </p>";
    m_zoomFitAction->setWhatsThis(text);
    connect(m_zoomFitAction, SIGNAL(triggered()), this, SLOT(zoomFit()));
  }


  /**
   * Updates the text in the screen resolution display box to the
   * current screen resolution, in meters per pixel.
   *
   */
  void MosaicZoomTool::updateResolutionBox() {
    if(p_scaleBox) {
      // Using these two points we can calculated the scene's width.
      QPointF point1 = getWidget()->getView()->mapToScene(0, 0);
      QPointF point2 = getWidget()->getView()->mapToScene(
          (int)getWidget()->getView()->width(), 0);
      double newWidth = point2.x() - point1.x();

      // The scene width divided by the viewport's width gives us the screen res.
      p_screenResolution = newWidth / getWidget()->getView()->viewport()->width();

      // Update the text box display
      p_scaleBox->setValue(p_screenResolution);
      //------------------------------------------------------------------------
      // This sets the up and down arrows (on the text box) so that each time a
      // user clicks them, a resonable amount of zoom happens.
      //------------------------------------------------------------------------
      p_scaleBox->setSingleStep(p_screenResolution * .05);
    }
  }


  QList<QAction *> MosaicZoomTool::getViewActions() {
    QList<QAction *> viewActs;

    viewActs.append(m_zoomInAction);
    viewActs.append(m_zoomOutAction);
    viewActs.append(m_zoomFitAction);

    return viewActs;
  }


  QAction *MosaicZoomTool::getPrimaryAction() {
    QAction *action = new QAction(this);
    action->setIcon(getIcon("viewmag.png"));
    action->setToolTip("Zoom (z)");
    action->setShortcut(Qt::Key_Z);
    QString text  =
      "<b>Function:</b>  Zoom in or out of the current cube.<br><br>"
      "This tool gives you a <b>click</b> to zoom by 2X and center on the "
      "point you clicked on, a <b>right-click</b> to zoom out by 2X and center "
      "on the point you clicked on, a <b>click and drag</b> box to best fit "
      "the given area into the visible screen, a <b>right-click and drag</b> "
      "box to zoom out and center on the center (smaller box means zoom out "
      "more), and disables context menus on the mosaic scene."
      "<p><b>Shortcut:</b>  z</p> ";
    action->setWhatsThis(text);
    return action;
  }


  /**
   * Creates the widget to add to the tool bar.
   *
   *
   * @param parent
   *
   * @return QWidget*
   */
  QWidget *MosaicZoomTool::getToolBarWidget() {
    QWidget *hbox = new QWidget;

    QToolButton *zoomInButton = new QToolButton(hbox);
    zoomInButton->setAutoRaise(true);
    zoomInButton->setDefaultAction(m_zoomInAction);
    zoomInButton->setIconSize(QSize(22, 22));

    QToolButton *zoomOutButton = new QToolButton(hbox);
    zoomOutButton->setAutoRaise(true);
    zoomOutButton->setDefaultAction(m_zoomOutAction);
    zoomOutButton->setIconSize(QSize(22, 22));

    QToolButton *zoomFitButton = new QToolButton(hbox);
    zoomFitButton->setAutoRaise(true);
    zoomFitButton->setDefaultAction(m_zoomFitAction);
    zoomFitButton->setIconSize(QSize(22, 22));

    p_scaleBox = new QDoubleSpinBox();
    p_scaleBox->setRange(DBL_MIN, DBL_MAX);
    p_scaleBox->setDecimals(8);
    connect(p_scaleBox, SIGNAL(editingFinished()), this, SLOT(zoomManual()));

    QLabel *resolutionLabel = new QLabel("Meters per pixel");

    QHBoxLayout *layout = new QHBoxLayout(hbox);
    layout->setMargin(0);
    layout->addWidget(zoomInButton);
    layout->addWidget(zoomOutButton);

    //-------------------------------------------
    // These two actions are not quite ready yet.
    // layout->addWidget(zoomActButton);
    //-------------------------------------------
    layout->addWidget(zoomFitButton);
    layout->addWidget(p_scaleBox);
    layout->addWidget(resolutionLabel);
    layout->addStretch(1);
    hbox->setLayout(layout);
    return hbox;
  }


  void MosaicZoomTool::mouseButtonRelease(QPointF mouseLoc, Qt::MouseButton s) {
    if(!isActive())
      return;

    if(s == Qt::LeftButton)
      zoomIn2X(mouseLoc);
    else if(s == Qt::RightButton)
      zoomOut2X(mouseLoc);
  }


  void MosaicZoomTool::mouseWheel(QPointF mouseLoc, int delta) {
    static const double sensitivity = 60.0; // this causes factor of 2 changes
    double scale = delta / sensitivity;

    if(delta < 0)
      scale = 1.0 / (-delta / sensitivity);

    scale = limitZoomBy(scale);

    QPoint screenMouseLoc(getWidget()->getView()->mapFromScene(mouseLoc));
    QPoint screenCenter(
        getWidget()->getView()->viewport()->size().width() / 2,
        getWidget()->getView()->viewport()->size().height() / 2);

    // We're going to do this by zooming on the mouse and then
    //   undo-ing the center in screen pixels.
    zoomBy(scale, mouseLoc);

    // The new center screen pixel is:
    //   center - mouseLoc
    QPoint desiredScreenCenter =
        screenCenter + (screenCenter - screenMouseLoc);
    QPointF newCenterPoint =
        getWidget()->getView()->mapToScene(desiredScreenCenter);
    getWidget()->getView()->centerOn(newCenterPoint);
  }


  /**
   * Zooms in 2 times.
   *
   */
  void MosaicZoomTool::zoomIn2X(QPointF center) {
    zoomBy(2.0, center);
  }


  /**
   * Zoom out 2 times.
   *
   */
  void MosaicZoomTool::zoomOut2X(QPointF center) {
    zoomBy(1.0 / 2.0, center);
  }


  /**
   * Zoom back to 1 to 1.
   *
   */
  void MosaicZoomTool::zoomActual(QPointF center) {
    zoomBy(1.1, center);
  }


  double MosaicZoomTool::limitZoomBy(double factor) {
    QTransform matrix = getWidget()->getView()->viewportTransform();
    matrix = matrix.scale(factor, factor).inverted();

    int smallerDimension = getWidget()->getView()->width();
    int largerDimension = getWidget()->getView()->height();

    if(smallerDimension > largerDimension) {
      int tmp = smallerDimension;
      smallerDimension = largerDimension;
      largerDimension = tmp;
    }

    QPointF point1 = matrix.map(QPointF(0, 0));
    QPointF point2 = matrix.map(QPointF(smallerDimension, 0));

    // If the factor cannot be done, find the one that can
    //   This max zoom in factor isn't right; I haven't found the number
    //   that corresponds to the Qt bug(?) causing us to not pan correctly on
    //   the zoom in.
    if((point2.x() - point1.x()) < 1) {
      QTransform origMatrix = getWidget()->getView()->viewportTransform();
      factor = smallerDimension / (origMatrix.m11() * 1.0);
    }

    point1 = matrix.map(QPointF(0, 0));
    point2 = matrix.map(QPointF(largerDimension, 0));

    if((point2.x() - point1.x()) > 1E10) {
      QTransform origMatrix = getWidget()->getView()->viewportTransform();
      factor = largerDimension / (origMatrix.m11() * 1E10);
    }

    return factor;
  }


  /**
   * Zoom IN by the given factor.
   *
   *
   * @param factor
   */
  void MosaicZoomTool::zoomBy(double factor, QPointF center) {
    factor = limitZoomBy(factor);

    getWidget()->getView()->scale(factor, factor);

    if(!center.isNull()) {
      getWidget()->getView()->centerOn(center);
    }

    updateResolutionBox();
  }


  /**
   * Fits the in the graphics view.
   *
   */
  void MosaicZoomTool::zoomFit() {
    getWidget()->refit();
    updateResolutionBox();
  }


  /**
   * Slot for the "Fit to Width" menu item on the Fit button.
   *
   */
  void MosaicZoomTool::zoomFitWidth() {

  }


  /**
   * Slot for the "Fit to Heighth" menu item on the Fit button.  This will display
   * the cube so that the entire cube heighth is displayed.
   *
   */
  void MosaicZoomTool::zoomFitHeight() {

  }


  /**
   * This method zooms by the value input in the line edit next to
   * the zoom tools.
   * This method is called when the double spin box value has been
   * changed.  First we figure out what we need to scale the
   * graphics view in order to achive the user's desired screen
   * resolution.
   *
   */
  void MosaicZoomTool::zoomManual() {
    if(p_scaleBox) {
      double x = p_screenResolution / p_scaleBox->value();
      zoomBy(x);
    }
  }


  /**
   * This method updates the line edits text to the correct zoom
   * value.
   *
   */
  void MosaicZoomTool::updateTool() {
    if(isActive()) {
      getWidget()->setCubesSelectable(false);
      getWidget()->getView()->setContextMenuPolicy(Qt::NoContextMenu);
      getWidget()->enableRubberBand(true);
    }
    else {
      getWidget()->setCubesSelectable(true);
      getWidget()->getView()->setContextMenuPolicy(Qt::DefaultContextMenu);
      getWidget()->enableRubberBand(false);
    }

    updateResolutionBox();
  }


  /**
   * This method is called when the RubberBandTool is complete.
   * The view is centered on the center point of the rubberband's
   * rectangle, then zoomed based on the largest side of the
   * rectange. (Width or height.)
   *
   */
  void MosaicZoomTool::rubberBandComplete(QRectF r, Qt::MouseButton s) {
    if(!isActive())
      return;

    double meters_pixel = 0.0;

    //--------------------------------------------------------------------
    // We only user width or height to determine the desired meters/pixel.
    // Which ever is larger.
    //--------------------------------------------------------------------
    if(r.width() > r.height()) {
      meters_pixel = r.width() / getWidget()->getView()->viewport()->width();
    }
    else {
      meters_pixel = r.height() / getWidget()->getView()->viewport()->height();
    }

    // calculate the scale.
    double scale = p_screenResolution / meters_pixel;

    // zooming in.
    if(s == Qt::LeftButton) {
      zoomBy(scale, r.center());
    }

    // zooming out.
    if(s == Qt::RightButton) {
      zoomBy(1 / scale, r.center());
    }

    updateResolutionBox();
  }

}