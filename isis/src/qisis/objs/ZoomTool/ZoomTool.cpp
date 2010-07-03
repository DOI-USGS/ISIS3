/**
 * @file
 * $Date: 2010/06/28 09:24:10 $
 * $Revision: 1.9 $
 *
 *  Unless noted otherwise, the portions of Isis written by the USGS are public domain. See
 *  individual third-party library and package descriptions for intellectual property information,
 *  user agreements, and related information.
 *
 *  Although Isis has been used by the USGS, no warranty, expressed or implied, is made by the
 *  USGS as to the accuracy and functioning of such software and related material nor shall the
 *  fact of distribution constitute any such warranty, and no responsibility is assumed by the
 *  USGS in connection therewith.
 *
 *  For additional information, launch $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *  in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *  http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *  http://www.usgs.gov/privacy.html.
 */

#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QLabel>
#include <QValidator>
#include <QHBoxLayout>
#include <QApplication>

#include "MdiCubeViewport.h"
#include "RubberBandTool.h"
#include "MainWindow.h"
#include "ToolPad.h"
#include "Workspace.h"
#include "ZoomTool.h"

namespace Qisis {
  /**
   * ZoomTool constructor
   *
   *
   * @param parent
   */
  ZoomTool::ZoomTool(QWidget *parent) : Qisis::Tool(parent) {
    p_userCursor = QCursor();

    p_zoomIn2X = new QAction(parent);
    p_zoomIn2X->setShortcut(Qt::Key_Plus);
    p_zoomIn2X->setText("Zoom In");
    p_zoomIn2X->setIcon(QPixmap(toolIconDir() + "/viewmag+.png"));
    connect(p_zoomIn2X, SIGNAL(activated()), this, SLOT(zoomIn2X()));

    p_zoomIn4X = new QAction(parent);
    p_zoomIn4X->setText("Zoom In 4X");
    p_zoomIn4X->setShortcut(Qt::CTRL + Qt::Key_Plus);
    connect(p_zoomIn4X, SIGNAL(activated()), this, SLOT(zoomIn4X()));

    p_zoomIn8X = new QAction(parent);
    p_zoomIn8X->setShortcut(Qt::ALT + Qt::Key_Plus);
    p_zoomIn8X->setText("Zoom In 8X");
    connect(p_zoomIn8X, SIGNAL(activated()), this, SLOT(zoomIn8X()));

    p_zoomOut2X = new QAction(parent);
    p_zoomOut2X->setShortcut(Qt::Key_Minus);
    p_zoomOut2X->setText("Zoom Out");
    p_zoomOut2X->setIcon(QPixmap(toolIconDir() + "/viewmag-.png"));
    connect(p_zoomOut2X, SIGNAL(activated()), this, SLOT(zoomOut2X()));

    p_zoomOut4X = new QAction(parent);
    p_zoomOut4X->setShortcut(Qt::CTRL + Qt::Key_Minus);
    p_zoomOut4X->setText("Zoom Out 4X");
    connect(p_zoomOut4X, SIGNAL(activated()), this, SLOT(zoomOut4X()));

    p_zoomOut8X = new QAction(parent);
    p_zoomOut8X->setShortcut(Qt::ALT + Qt::Key_Minus);
    p_zoomOut8X->setText("Zoom Out 8X");
    connect(p_zoomOut8X, SIGNAL(activated()), this, SLOT(zoomOut8X()));

    p_zoomActual = new QAction(parent);
    p_zoomActual->setShortcut(Qt::Key_Slash);
    p_zoomActual->setText("&Actual Pixels");
    p_zoomActual->setIcon(QPixmap(toolIconDir() + "/viewmag1.png"));
    connect(p_zoomActual, SIGNAL(activated()), this, SLOT(zoomActual()));

    p_zoomFit = new QAction(parent);
    p_zoomFit->setShortcut(Qt::Key_Asterisk);
    p_zoomFit->setText("&Fit in Window");
    p_zoomFit->setIcon(QPixmap(toolIconDir() + "/viewmagfit.png"));
    connect(p_zoomFit, SIGNAL(activated()), this, SLOT(zoomFit()));

  }

  /**
   * Adds the action to the toolpad.
   *
   *
   * @param toolpad
   *
   * @return QAction*
   */
  QAction *ZoomTool::toolPadAction(ToolPad *toolpad) {
    QAction *action = new QAction(toolpad);
    action->setIcon(QPixmap(toolIconDir() + "/viewmag.png"));
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
  void ZoomTool::addTo(QMenu *menu) {
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
  QWidget *ZoomTool::createToolBarWidget(QStackedWidget *parent) {
    QWidget *hbox = new QWidget(parent);

    QToolButton *zoomInButton = new QToolButton(hbox);
    zoomInButton->setIcon(QPixmap(toolIconDir() + "/viewmag+.png"));
    zoomInButton->setToolTip("Zoom In");
    QString text =
      "<b>Function:</b> Zoom in 2X at the center of the active viewport \
      <p><b>Shortcut:</b>  +</p> \
      <p><b>Mouse:</b>  LeftButton zooms in 2X under pointer</p> \
      <p><b>Modifiers:</b>  Shortcuts and mouse clicks can be augmented \
      using the Ctrl or Alt key for 4X and 8X zooms, respectively</p> \
      <p><b>Hint:</b>  Left click and drag for a local zoom which scales data \
      in the red marquee to the viewport</p>";
    zoomInButton->setWhatsThis(text);
    connect(zoomInButton, SIGNAL(clicked()), this, SLOT(zoomIn2X()));
    zoomInButton->setAutoRaise(true);
    zoomInButton->setIconSize(QSize(22, 22));

    QToolButton *zoomOutButton = new QToolButton(hbox);
    zoomOutButton->setIcon(QPixmap(toolIconDir() + "/viewmag-.png"));
    zoomOutButton->setToolTip("Zoom Out");
    text =
      "<b>Function:</b> Zoom out 2X at the center of the active viewport \
      <p><b>Shortcut:</b>  +</p> \
      <p><b>Mouse:</b>  RightButton zooms out 2X under pointer</p> \
      <p><b>Modifiers:</b>  Shortcuts and mouse clicks can be augmented \
      using the Ctrl or Alt key for 4X and 8X zooms, respectively</p> \
      <p><b>Hint:</b>  Left click and drag for a local zoom which scales data \
      in the red marquee to the viewport</p>";
    zoomOutButton->setWhatsThis(text);
    connect(zoomOutButton, SIGNAL(clicked()), this, SLOT(zoomOut2X()));
    zoomOutButton->setAutoRaise(true);
    zoomOutButton->setIconSize(QSize(22, 22));

    QToolButton *zoomActButton = new QToolButton(hbox);
    zoomActButton->setIcon(QPixmap(toolIconDir() + "/viewmag1.png"));
    zoomActButton->setToolTip("Zoom 1:1");
    text =
      "<b>Function:</b> Zoom the active viewport to 1:1 such that one \
      viewport pixel represents one cube pixel. That is, 100% scale. \
      <p><b>Shortcut:</b> /</p> \
      <p><b>Mouse:</b>  Ctrl+MiddleButton zooms 1:1 under pointer</p> \
      <p><b>Hint:</b>  MiddleButton (without Ctrl) retains current \
      scale but moves the pixel under the pointer to the center of the \
      viewport</p>";
    zoomActButton->setWhatsThis(text);
    connect(zoomActButton, SIGNAL(clicked()), this, SLOT(zoomActual()));
    zoomActButton->setAutoRaise(true);
    zoomActButton->setIconSize(QSize(22, 22));

    // Create menu on the zoomFit button to select fitting the cube for
    // width or height.
    QMenu *zoomFitMenu = new QMenu();
    QAction *fitWidth = new QAction(this);
    fitWidth->setText("Fit Width");
    connect(fitWidth, SIGNAL(triggered(bool)), this, SLOT(zoomFitWidth()));
    zoomFitMenu->addAction(fitWidth);

    QAction *fitHeight = new QAction(this);
    fitHeight->setText("Fit Height");
    connect(fitHeight, SIGNAL(triggered(bool)), this, SLOT(zoomFitHeight()));
    zoomFitMenu->addAction(fitHeight);

    QToolButton *zoomFitButton = new QToolButton(hbox);
    zoomFitButton->setIcon(QPixmap(toolIconDir() + "/viewmagfit.png"));
    zoomFitButton->setMenu(zoomFitMenu);
    zoomFitButton->setPopupMode(QToolButton::MenuButtonPopup);
    zoomFitButton->setToolTip("Fit in viewport");
    text =
      "<b>Function:</b> Fit the entire cube inside the active viewport. For \
      extremely large cubes, this may not be possible. \
      <p><b>Shortcut:</b> *</p> \
      <p><b>Hint:</b>  Many shortcuts for the zoom tool and other tools \
      are easily available on the numeric keypad </p>";
    zoomFitButton->setWhatsThis(text);
    connect(zoomFitButton, SIGNAL(clicked()), this, SLOT(zoomFit()));
    zoomFitButton->setAutoRaise(true);
    zoomFitButton->setIconSize(QSize(22, 22));

    p_zoomLineEdit = new QLineEdit(hbox);
    p_zoomLineEdit->setText("");
    p_zoomLineEdit->setMaxLength(8);
    p_zoomLineEdit->setMaximumWidth(80);

    QDoubleValidator *dval = new QDoubleValidator(hbox);
    p_zoomLineEdit->setValidator(dval);

    QSizePolicy policy = p_zoomLineEdit->sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Fixed);
    p_zoomLineEdit->setSizePolicy(policy);

    p_zoomLineEdit->setToolTip("Scale");
    text =
      "<b>Function:</b> Shows the scale of the active viewport.  Additionally, \
      you can manually enter the scale.";
    p_zoomLineEdit->setWhatsThis(text);
    connect(p_zoomLineEdit, SIGNAL(returnPressed()), this, SLOT(zoomManual()));

    QHBoxLayout *layout = new QHBoxLayout(hbox);
    layout->setMargin(0);
    layout->addWidget(zoomInButton);
    layout->addWidget(zoomOutButton);
    layout->addWidget(zoomActButton);
    layout->addWidget(zoomFitButton);
    layout->addWidget(p_zoomLineEdit);
    layout->addStretch(1);
    hbox->setLayout(layout);
    return hbox;
  }

  /**
   * Zooms in 2 times.
   *
   */
  void ZoomTool::zoomIn2X() {
    zoomBy(2.0);
  }


  /**
   * Zooms in 4 times.
   *
   */
  void ZoomTool::zoomIn4X() {
    zoomBy(4.0);
  }


  /**
   * Zooms in 8 times.
   *
   */
  void ZoomTool::zoomIn8X() {
    zoomBy(8.0);
  }


  /**
   * Zoom out 2 times.
   *
   */
  void ZoomTool::zoomOut2X() {
    zoomBy(1.0 / 2.0);
  }


  /**
   * Zoom out 4 times.
   *
   */
  void ZoomTool::zoomOut4X() {
    zoomBy(1.0 / 4.0);
  }


  /**
   * Zoom out 8 times.
   *
   */
  void ZoomTool::zoomOut8X() {
    zoomBy(1.0 / 8.0);
  }


  /**
   * Zoom back to 1 to 1.
   *
   */
  void ZoomTool::zoomActual() {
    zoomBy(0.0);
  }


  /**
   * Zoom by the given factor.
   *
   *
   * @param factor
   */
  void ZoomTool::zoomBy(double factor) {
    MdiCubeViewport *d = cubeViewport();
    if(d == NULL) return;
    double newScale = d->scale() * factor;
    if(newScale == 0.0) newScale = 1.0;
    d->setScale(newScale);
    updateTool();

    if(cubeViewport()->isLinked()) {
      for(int i = 0; i < (int)cubeViewportList()->size(); i++) {
        d = (*(cubeViewportList()))[i];
        if(d == cubeViewport()) continue;
        if(d->isLinked()) {
          newScale = d->scale() * factor;
          if(newScale == 0.0) newScale = 1.0;
          d->setScale(newScale);
        }
      }
    }
  }


  /**
   * Fits the cube in the viewport.
   *
   */
  void ZoomTool::zoomFit() {
    MdiCubeViewport *d = cubeViewport();
    if(d == NULL) return;
    d->setScale(d->fitScale(), d->cubeSamples() / 2.0, d->cubeLines() / 2.0);
    updateTool();

    if(d->isLinked()) {
      for(int i = 0; i < (int)cubeViewportList()->size(); i++) {
        d = (*(cubeViewportList()))[i];
        if(d == cubeViewport()) continue;
        if(d->isLinked()) {
          d->setScale(d->fitScale(), d->cubeSamples() / 2.0, d->cubeLines() / 2.0);
        }
      }
    }
  }



  /**
   * Slot for the "Fit to Width" menu item on the Fit button.  This will
   * display the cube so that the entire cube width is displayed.
   *
   */
  void ZoomTool::zoomFitWidth() {
    MdiCubeViewport *d = cubeViewport();
    if(d == NULL) return;
    d->setScale(d->fitScaleWidth(), d->cubeSamples() / 2.0, d->cubeLines() / 2.0);
    updateTool();

    if(d->isLinked()) {
      for(int i = 0; i < (int)cubeViewportList()->size(); i++) {
        d = (*(cubeViewportList()))[i];
        if(d == cubeViewport()) continue;
        if(d->isLinked()) {
          d->setScale(d->fitScaleWidth(), d->cubeSamples() / 2.0, d->cubeLines() / 2.0);
        }
      }
    }
  }


  /**
   * Slot for the "Fit to Heighth" menu item on the Fit button.  This will display
   * the cube so that the entire cube heighth is displayed.
   *
   */
  void ZoomTool::zoomFitHeight() {
    MdiCubeViewport *d = cubeViewport();
    if(d == NULL) return;
    d->setScale(d->fitScaleHeight(), d->cubeSamples() / 2.0, d->cubeLines() / 2.0);
    updateTool();

    if(d->isLinked()) {
      for(int i = 0; i < (int)cubeViewportList()->size(); i++) {
        d = (*(cubeViewportList()))[i];
        if(d == cubeViewport()) continue;
        if(d->isLinked()) {
          d->setScale(d->fitScaleHeight(), d->cubeSamples() / 2.0, d->cubeLines() / 2.0);
        }
      }
    }
  }


  /**
   * This method zooms by the value input in the line edit next to
   * the zoom tools.
   *
   */
  void ZoomTool::zoomManual() {
    MdiCubeViewport *d = cubeViewport();
    if(d == NULL) return;
    QString strScale = p_zoomLineEdit->text();
    double scale = strScale.toDouble() / 100.;
    d->setScale(scale);
    d->setFocus();
    updateTool();

    if(d->isLinked()) {
      for(int i = 0; i < (int)cubeViewportList()->size(); i++) {
        d = (*(cubeViewportList()))[i];
        if(d == cubeViewport()) continue;
        if(d->isLinked()) d->setScale(scale);
      }
    }
  }


  /**
   * This method updates the line edits text to the correct zoom
   * value.
   *
   */
  void ZoomTool::updateTool() {
    if(cubeViewport() == NULL) {
      p_zoomLineEdit->setText("");
    }
    else {
      double scale = cubeViewport()->scale() * 100.0;
      QString strScale;
      strScale.setNum(scale);
      p_zoomLineEdit->setText(strScale);
    }
  }


  /**
   * This method is called when the RubberBandTool is complete. It
   * will either zoom the CubeViewport to the rectangle specified
   * by the RubberBandTool or will handle different zoom methods
   * specified by the last RubberBandTool's  mouse button.
   *
   */
  void ZoomTool::rubberBandComplete() {
    QApplication::processEvents();
    MdiCubeViewport *d = cubeViewport();
    if(!RubberBandTool::isValid()) return;

    // The RubberBandTool has a rectangle
    if(!RubberBandTool::isPoint()) {
      QRect r = RubberBandTool::rectangle();
      if((r.width() >= 5) && (r.height() >= 5)) {
        int x = r.x() + r.width() / 2;
        int y = r.y() + r.height() / 2;
        double xscale = (double) d->viewport()->width() / r.width();
        double yscale = (double) d->viewport()->height() / r.height();
        double scale = xscale < yscale ? xscale : yscale;
        if(RubberBandTool::mouseButton() & Qt::RightButton) scale = 1.0 / scale;
        scale *= d->scale();
        d->setScale(scale, x, y);
        updateTool();
        if(d->isLinked()) {
          for(int i = 0; i < (int)cubeViewportList()->size(); i++) {
            d = (*(cubeViewportList()))[i];
            if(d == cubeViewport()) continue;
            if(d->isLinked()) {
              int x = r.x() + r.width() / 2;
              int y = r.y() + r.height() / 2;
              double xscale = (double) d->viewport()->width() / r.width();
              double yscale = (double) d->viewport()->height() / r.height();
              double scale = xscale < yscale ? xscale : yscale;
              if(RubberBandTool::mouseButton() & Qt::RightButton) scale = 1.0 / scale;
              scale *= d->scale();
              d->setScale(scale, x, y);
            }
          }
        }
      }
    }
    // The RubberBandTool has a point (mouse click)
    else {
      double factor = 2.0;
      if(RubberBandTool::mouseButton() & Qt::ControlModifier) factor = 4.0;
      if(RubberBandTool::mouseButton() & Qt::ShiftModifier) factor = 8.0;
      if(RubberBandTool::mouseButton() & Qt::RightButton) factor = 1.0 / factor;
      if(RubberBandTool::mouseButton() & Qt::MidButton) factor = 1.0;
      if(RubberBandTool::mouseButton() == Qt::MidButton + Qt::ControlModifier) factor = 0.0;
      MdiCubeViewport *d = cubeViewport();
      double scale = d->scale() * factor;
      if(scale == 0.0) scale = 1.0;
      QPoint p = RubberBandTool::getVertices()[0];
      d->setScale(scale, p.x(), p.y());
      updateTool();

      if(d->isLinked()) {
        for(int i = 0; i < (int)cubeViewportList()->size(); i++) {
          d = (*(cubeViewportList()))[i];
          if(d == cubeViewport()) continue;
          if(d->isLinked()) {
            scale = d->scale() * factor;
            if(scale == 0.0) scale = 1.0;
            d->setScale(scale, p.x(), p.y());
          }
        }
      }
      p_lastScale = scale;
    }

  }


  /**
   * This methods enables the RubberBandTool, it also sets the
   * RubberBandTool to allow points and to allow all clicks.
   *
   */
  void ZoomTool::enableRubberBandTool() {
    RubberBandTool::enable(RubberBandTool::Rectangle);
    RubberBandTool::allowPoints();
    RubberBandTool::allowAllClicks();
  }


  /**
   *
   *
   *
   * @param p
   * @param s
   */
  void ZoomTool::mouseButtonPress(QPoint p, Qt::MouseButton s) {
    if(s == Qt::RightButton) {
      cubeViewport()->viewport()->setCursor(QCursor(QPixmap(toolIconDir() + "/viewmag-.png")));
    }
    else if(s == Qt::LeftButton) {
      cubeViewport()->viewport()->setCursor(QCursor(QPixmap(toolIconDir() + "/viewmag+.png")));
    }
  }


  /**
   *
   *
   *
   * @param p
   * @param s
   */
  void ZoomTool::mouseButtonRelease(QPoint p, Qt::MouseButton s) {
    //set the cursor back to the original cursor shape.
    cubeViewport()->viewport()->setCursor(p_userCursor);
  }

}
