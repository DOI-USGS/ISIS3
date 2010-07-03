#include "MosaicZoomTool.h"

#include <QMenu>

class QHBoxLayout;
class QLabel;

namespace Qisis {
  /**
   * MosaicZoomTool constructor 
   *  
   * 
   * @param parent 
   */
  MosaicZoomTool::MosaicZoomTool (MosaicWidget *parent) : Qisis::MosaicTool(parent) {
    p_parent = parent;
    connect(this,SIGNAL(activated(bool)),this,SLOT(updateTool()));

    p_zoomIn2X = new QAction(parent);
    p_zoomIn2X->setShortcut(Qt::Key_Plus);
    p_zoomIn2X->setText("Zoom In");
    p_zoomIn2X->setIcon(QPixmap(toolIconDir()+"/viewmag+.png"));
    connect(p_zoomIn2X,SIGNAL(activated()),this,SLOT(zoomIn2X()));

    p_zoomOut2X = new QAction(parent);
    p_zoomOut2X->setShortcut(Qt::Key_Minus);
    p_zoomOut2X->setText("Zoom Out");
    p_zoomOut2X->setIcon(QPixmap(toolIconDir()+"/viewmag-.png"));
    connect(p_zoomOut2X,SIGNAL(activated()),this,SLOT(zoomOut2X()));

    p_zoomActual = new QAction (parent);
    p_zoomActual->setShortcut(Qt::Key_Plus);
    p_zoomActual->setText("&Actual Pixels");
    p_zoomActual->setIcon(QPixmap(toolIconDir()+"/viewmag1.png"));
    connect(p_zoomActual,SIGNAL(activated()),this,SLOT(zoomActual()));

    p_zoomFit = new QAction (parent);
    p_zoomFit->setShortcut(Qt::Key_Asterisk);
    p_zoomFit->setText("&Fit in Window");
    p_zoomFit->setIcon(QPixmap(toolIconDir()+"/viewmagfit.png"));
    connect(p_zoomFit,SIGNAL(activated()),this,SLOT(zoomFit()));

  }

  /**
   * Adds the action to the toolpad.
   * 
   * 
   * @param toolpad 
   * 
   * @return QAction* 
   */
  QAction *MosaicZoomTool::toolPadAction(ToolPad *toolpad) {
    QAction *action = new QAction(toolpad);
    action->setIcon(QPixmap(toolIconDir()+"/viewmag.png"));
    action->setToolTip("Zoom (Z)");
    action->setShortcut(Qt::Key_Z);
    QString text  =
      "<b>Function:</b>  Zoom in or out of the current cube. \
      <p><b>Shortcut:</b>  Z</p> ";
    action->setWhatsThis(text);
    return action;
  }


  /**
   * Adds the zoom action to the given menu.
   * 
   * 
   * @param menu 
   */
  void MosaicZoomTool::addToMenu(QMenu *menu) {
    menu->addAction(p_zoomFit);
    menu->addAction(p_zoomActual);
    menu->addAction(p_zoomIn2X);
    menu->addAction(p_zoomOut2X);
  }


  /**
   * Creates the widget to add to the tool bar.
   * 
   * 
   * @param parent 
   * 
   * @return QWidget* 
   */
  QWidget *MosaicZoomTool::createToolBarWidget (QStackedWidget *parent) {
    QWidget *hbox = new QWidget(parent);

    QToolButton *zoomInButton = new QToolButton(hbox);
    zoomInButton->setIcon(QPixmap(toolIconDir()+"/viewmag+.png"));
    zoomInButton->setToolTip("Zoom In");
    QString text =
     "<b>Function:</b> Zoom in 2X at the center of the active viewport \
      <p><b>Shortcut:</b>  +</p> \
      <p><b>Mouse:</b>  LeftButton zooms in 2X under pointer</p> \
      <p><b>Modifiers:</b>  Shortcuts and mouse clicks can be augmented \
      using the Ctrl or Alt key for 4X and 8X zooms, respectively</p> \
      <p><b>Hint:</b>  Left click and drag for a local zoom which scales data \
      in the marquee to the view</p>";
    zoomInButton->setWhatsThis(text);
    connect(zoomInButton,SIGNAL(clicked()),this,SLOT(zoomIn2X()));
    zoomInButton->setAutoRaise(true);
    zoomInButton->setIconSize(QSize(22,22));

    QToolButton *zoomOutButton = new QToolButton(hbox);
    zoomOutButton->setIcon(QPixmap(toolIconDir()+"/viewmag-.png"));
    zoomOutButton->setToolTip("Zoom Out");
    text =
     "<b>Function:</b> Zoom out 2X at the center of the view \
      <p><b>Shortcut:</b>  +</p> \
      <p><b>Mouse:</b>  RightButton zooms out 2X under pointer</p> \
      <p><b>Modifiers:</b>  Shortcuts and mouse clicks can be augmented \
      using the Ctrl or Alt key for 4X and 8X zooms, respectively</p> \
      <p><b>Hint:</b>  Left click and drag for a local zoom which scales data \
      in the marquee to the view</p>";
    zoomOutButton->setWhatsThis(text);
    connect(zoomOutButton,SIGNAL(clicked()),this,SLOT(zoomOut2X()));
    zoomOutButton->setAutoRaise(true);
    zoomOutButton->setIconSize(QSize(22,22));

    QToolButton *zoomFitButton = new QToolButton(hbox);
    zoomFitButton->setIcon(QPixmap(toolIconDir()+"/viewmagfit.png"));
    //zoomFitButton->setMenu(zoomFitMenu);
    //zoomFitButton->setPopupMode(QToolButton::MenuButtonPopup);
    zoomFitButton->setToolTip("Fit in view");
    text =
     "<b>Function:</b> Fit the entire mosaic inside the view. \
      <p><b>Shortcut:</b> *</p> \
      <p><b>Hint:</b>  Many shortcuts for the zoom tool and other tools \
      are easily available on the numeric keypad </p>";
    zoomFitButton->setWhatsThis(text);
    connect(zoomFitButton,SIGNAL(clicked()),this,SLOT(zoomFit()));
    zoomFitButton->setAutoRaise(true);
    zoomFitButton->setIconSize(QSize(22,22));

    p_scaleBox = new QDoubleSpinBox();
    p_scaleBox->setRange(DBL_MIN, DBL_MAX);
    connect(p_scaleBox,SIGNAL(editingFinished()),this,SLOT(zoomManual()));

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


  /**
   * Zooms in 2 times.
   * 
   */
  void MosaicZoomTool::zoomIn2X() {
    zoomBy(2.0);
  }


  /**
   * Zoom out 2 times.
   * 
   */
  void MosaicZoomTool::zoomOut2X() {
    zoomBy(1.0/2.0);
  }


  /**
   * Zoom back to 1 to 1.
   * 
   */
  void MosaicZoomTool::zoomActual () {
    std::cout << "zooming to 1 to 1" << std::endl;
    zoomBy(1.1);
  }


  /**
   * Zoom IN by the given factor.
   * 
   * 
   * @param factor 
   */
  void MosaicZoomTool::zoomBy (double factor) {
    //if(isActive()) {
      getGraphicsView()->scale(factor, factor);
      updateResolutionBox();
    //}
    
  }


  /**
   * Fits the in the graphics view.
   * 
   */
  void MosaicZoomTool::zoomFit () {
    ((MosaicWidget *)p_parent)->refit();
    updateResolutionBox();
    
  }


  /**
   * Slot for the "Fit to Width" menu item on the Fit button.  
   * 
   */
  void MosaicZoomTool::zoomFitWidth () {
    
  }


  /**
   * Slot for the "Fit to Heighth" menu item on the Fit button.  This will display
   * the cube so that the entire cube heighth is displayed.
   * 
   */
  void MosaicZoomTool::zoomFitHeight () {
    
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
  void MosaicZoomTool::zoomManual () {
    double x = p_screenResolution/p_scaleBox->value();
    zoomBy(x);   
  }


 /** 
  * This method updates the line edits text to the correct zoom
  * value.
  * 
  */
  void MosaicZoomTool::updateTool() {
    //if(isActive()) {
      //QList<QGraphicsItem *> items = getGraphicsView()->scene()->items();
      //for(int i = 0; i < items.size(); i++) {
       // items[i]->setFlag(QGraphicsItem::ItemIsSelectable, false);
      //}
    //}

    updateResolutionBox();
    
  }


  /** 
   * This method is called when the RubberBandTool is complete. 
   * The view is centered on the center point of the rubberband's 
   * rectangle, then zoomed based on the largest side of the 
   * rectange. (Width or height.) 
   * 
   */
  void MosaicZoomTool::rubberBandComplete(QRect r, QGraphicsSceneMouseEvent *mouseEvent) {
    double meters_pixel = 0.0;
    // The RubberBandTool has a geometry (rectangle)
    QRectF s(getGraphicsView()->mapToScene(r.topLeft()), getGraphicsView()->mapToScene(r.bottomRight()));

    //----------------------------------------------------------------
    // If the user does a single click, then we will center where the
    // clicked and return.  No zooming!
    //----------------------------------------------------------------
    if(abs((int)r.bottomRight().x() - (int)r.topLeft().x()) < 6){
      if(mouseEvent->button() == Qt::LeftButton) {
        zoomIn2X();
      }
      if(mouseEvent->button() == Qt::RightButton) {
        zoomOut2X();
      }
      getGraphicsView()->centerOn(s.center());
      return;
    }

    //--------------------------------------------------------------------
    // We only user width or height to determine the desired meters/pixel.
    // Which ever is larger.
    //--------------------------------------------------------------------
    if(s.width() > s.height()) {
      meters_pixel = s.width() / getGraphicsView()->viewport()->width();
    } else {
      meters_pixel = s.height()/ getGraphicsView()->viewport()->height();
    }

    // calculate the scale.
    double scale = p_screenResolution / meters_pixel;

    // center the view
    getGraphicsView()->centerOn(s.center());

    // zooming in.
    if(mouseEvent->button() == Qt::LeftButton) {
      zoomBy(scale);
    }
    // zooming out.
    if(mouseEvent->button() == Qt::RightButton) {
      zoomBy(1/scale);
    }
    //getGraphicsView()->scale(scale,scale);
    
    updateResolutionBox();
    
  }


  /**
   * Updates the text in the screen resolution display box to the
   * current screen resolution, in meters per pixel.
   * 
   */
  void MosaicZoomTool::updateResolutionBox(){
    // Using these two points we can calculated the scene's width.
    QPointF point1 = getGraphicsView()->mapToScene(0,0);
    QPointF point2 = getGraphicsView()->mapToScene((int) getGraphicsView()->width(),0);
    double newWidth = point2.x() - point1.x();
    // The scene width divided by the viewport's width gives us the screen res.
    p_screenResolution = newWidth/getGraphicsView()->viewport()->width();
    // Update the text box display
    p_scaleBox->setValue(p_screenResolution);
    //------------------------------------------------------------------------
    // This sets the up and down arrows (on the text box) so that each time a
    // user clicks them, a resonable amount of zoom happens.
    //------------------------------------------------------------------------
    p_scaleBox->setSingleStep(p_screenResolution*.05);
    p_parent->updateScreenResolution(p_screenResolution);
  }

}
