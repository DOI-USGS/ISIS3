#include "StretchTool.h"

#include <QHBoxLayout>
#include <QMenuBar>
#include <QPixmap>
#include <QSizePolicy>
#include <QToolBar>
#include <QValidator>

#include "AdvancedStretchDialog.h"
#include "Brick.h"
#include "CubeViewport.h"
#include "Histogram.h"
#include "iException.h"
#include "iString.h"
#include "MainWindow.h"
#include "MdiCubeViewport.h"
#include "RubberBandTool.h"
#include "Statistics.h"
#include "Stretch.h"
#include "ToolPad.h"
#include "ViewportBuffer.h"
#include "ViewportMainWindow.h"
#include "Workspace.h"


using namespace Isis;
using namespace std;

namespace Qisis {
  /**
   * StretchTool constructor
   *
   *
   * @param parent
   */
  StretchTool::StretchTool(QWidget *parent) : Tool::Tool(parent) {
    
    p_chipViewportStretch = NULL;
    p_preGlobalStretches = NULL;
    p_advancedStretch = NULL;
    
    p_chipViewportStretch = new Stretch;
    
    p_advancedStretch = new AdvancedStretchDialog(parent);
    connect(p_advancedStretch, SIGNAL(stretchChanged()),
            this, SLOT(advancedStretchChanged()));
    connect(p_advancedStretch, SIGNAL(visibilityChanged()),
            this, SLOT(updateTool()));

    QPushButton *hiddenButton = new QPushButton();
    hiddenButton->setVisible(false);
    hiddenButton->setDefault(true);

    p_stretchGlobal = new QAction(parent);
    p_stretchGlobal->setShortcut(Qt::CTRL + Qt::Key_G);
    p_stretchGlobal->setText("Global Stretch");
    connect(p_stretchGlobal, SIGNAL(activated()), this, SLOT(stretchGlobal()));

    p_stretchRegional = new QAction(parent);
    p_stretchRegional->setShortcut(Qt::CTRL + Qt::Key_R);
    p_stretchRegional->setText("Regional Stretch");
    connect(p_stretchRegional, SIGNAL(activated()), this, SLOT(stretchRegional()));

    // Emit a signal when an exception and connect o the Warning object to display Warning icon and the message
    connect(this, SIGNAL(warningSignal(std::string &, const std::string)),
            (Qisis::ViewportMainWindow *)parent,
            SLOT(displayWarning(std::string &, const std::string &)));
  }


  /**
   * Destructor
   */
  StretchTool::~StretchTool() {
    if(p_preGlobalStretches) {
      delete [] p_preGlobalStretches;
      p_preGlobalStretches = NULL;
    }
    
    if (p_chipViewportStretch) {
      delete p_chipViewportStretch;
      p_chipViewportStretch = NULL;
    }
  }


  /**
   * Adds the stretch action to the toolpad.
   *
   *
   * @param pad
   *
   * @return QAction*
   */
  QAction *StretchTool::toolPadAction(ToolPad *pad) {
    QAction *action = new QAction(pad);
    action->setIcon(QPixmap(toolIconDir() + "/stretch_global.png"));
    action->setToolTip("Stretch (S)");
    action->setShortcut(Qt::Key_S);
    QString text  =
      "<b>Function:</b>  Change the stretch range of the cube.\
      <p><b>Shortcut:</b>  S</p> ";
    action->setWhatsThis(text);

    return action;
  }


  /**
   * Adds the stretch action to the given menu.
   *
   *
   * @param menu
   */
  void StretchTool::addTo(QMenu *menu) {
    menu->addAction(p_stretchGlobal);
    menu->addAction(p_stretchRegional);
  }


  /**
   * Creates the widget to add to the tool bar.
   *
   *
   * @param parent
   *
   * @return QWidget*
   */
  QWidget *StretchTool::createToolBarWidget(QStackedWidget *parent) {
    QWidget *hbox = new QWidget(parent);

    QToolButton *butt = new QToolButton(hbox);
    butt->setAutoRaise(true);
    butt->setIconSize(QSize(22, 22));
    butt->setIcon(QPixmap(toolIconDir() + "/regional_stretch-2.png"));
    butt->setToolTip("Stretch");
    QString text  =
      "<b>Function:</b> Automatically compute min/max stretch using viewed \
      pixels in the band(s) of the active viewport.  That is, only pixels \
      that are visible in the viewport are used. \
      If the viewport is in RGB color all three bands will be stretched. \
      <p><b>Shortcut:</b>  Ctrl+R</p> \
      <p><b>Mouse:</b>  Left click \
      <p><b>Hint:</b>  Left click and drag for a local stretch.  Uses only \
      pixels in the red marquee</p>";
    butt->setWhatsThis(text);
    connect(butt, SIGNAL(clicked()), this, SLOT(stretchRegional()));
    p_stretchRegionalButton = butt;

    p_stretchBandComboBox = new QComboBox(hbox);
    p_stretchBandComboBox->setEditable(false);
    p_stretchBandComboBox->addItem("Red Band", Red);
    p_stretchBandComboBox->addItem("Green Band", Green);
    p_stretchBandComboBox->addItem("Blue Band", Blue);
    p_stretchBandComboBox->setToolTip("Select Color");
    text =
      "<b>Function:</b> Selecting the color will allow the appropriate \
      min/max to be seen and/or edited in text fields to the right.";

//      The All option implies the same min/max will be applied
//      to all three colors (RGB) if either text field is edited";
    p_stretchBandComboBox->setWhatsThis(text);
    p_stretchBandComboBox->setCurrentIndex(p_stretchBandComboBox->findData(Red));
    p_stretchBand = Red;
    connect(p_stretchBandComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(stretchBandChanged(int)));

    QDoubleValidator *dval = new QDoubleValidator(hbox);
    p_stretchMinEdit = new QLineEdit(hbox);
    p_stretchMinEdit->setValidator(dval);
    p_stretchMinEdit->setToolTip("Minimum");
    text =
      "<b>Function:</b> Shows the current minimum pixel value.  Pixel values \
      below minimum are shown as black.  Pixel values above the maximum \
      are shown as white or the highest intensity of red/green/blue \
      if in color. Pixel values between the minimum and maximum are stretched \
      linearly between black and white (or color component). \
      <p><b>Hint:</b>  You can manually edit the minimum but it must be \
      less than the maximum.";
    p_stretchMinEdit->setWhatsThis(text);
    p_stretchMinEdit->setMaximumWidth(100);
    connect(p_stretchMinEdit, SIGNAL(returnPressed()), this, SLOT(changeStretch()));

    p_stretchMaxEdit = new QLineEdit(hbox);
    p_stretchMaxEdit->setValidator(dval);
    p_stretchMaxEdit->setToolTip("Maximum");
    text =
      "<b>Function:</b> Shows the current maximum pixel value.  Pixel values \
      below minimum are shown as black.  Pixel values above the maximum \
      are shown as white or the highest intensity of red/green/blue \
      if in color. Pixel values between the minimum and maximum are stretched \
      linearly between black and white (or color component). \
      <p><b>Hint:</b>  You can manually edit the maximum but it must be \
      greater than the minimum";
    p_stretchMaxEdit->setWhatsThis(text);
    p_stretchMaxEdit->setMaximumWidth(100);
    connect(p_stretchMaxEdit, SIGNAL(returnPressed()), this, SLOT(changeStretch()));

    // Create the two menus that drop down from the buttons
    QMenu *copyMenu = new QMenu();
    QMenu *globalMenu = new QMenu();

    p_copyBands = new QAction(parent);
    p_copyBands->setText("to All Bands");
    connect(p_copyBands, SIGNAL(triggered(bool)), this, SLOT(setStretchAcrossBands()));

    QAction *copyAll = new QAction(parent);
    copyAll->setIcon(QPixmap(toolIconDir() + "/copy_stretch.png"));
    copyAll->setText("to All Viewports");
    connect(copyAll, SIGNAL(triggered(bool)), this, SLOT(setStretchAllViewports()));

    copyMenu->addAction(copyAll);
    copyMenu->addAction(p_copyBands);

    p_copyButton = new QToolButton();
    p_copyButton->setAutoRaise(true);
    p_copyButton->setIconSize(QSize(22, 22));
    p_copyButton->setIcon(QPixmap(toolIconDir() + "/copy_stretch.png"));
    p_copyButton->setPopupMode(QToolButton::MenuButtonPopup);
    p_copyButton->setMenu(copyMenu);
    p_copyButton->setDefaultAction(copyAll);
    p_copyButton->setToolTip("Copy");
    text  =
      "<b>Function:</b> Copy the current stretch to all the \
      active viewports. Or use the drop down menu to copy the current stretch \
      to all the  bands in the active viewport. \
      <p><b>Hint:</b>  Can reset the stretch to an automaticaly computed \
      stretch by using the 'Reset' stretch button option. </p>";
    p_copyButton->setWhatsThis(text);

    QAction *currentView = new QAction(parent);
    currentView->setText("Active Viewport");
    currentView->setIcon(QPixmap(toolIconDir() + "/global_stretch.png"));
    globalMenu->addAction(currentView);
    connect(currentView, SIGNAL(triggered(bool)), this, SLOT(stretchGlobal()));

    QAction *globalAll = new QAction(parent);
    globalAll->setText("All Viewports");
    globalMenu->addAction(globalAll);
    connect(globalAll, SIGNAL(triggered(bool)), this, SLOT(stretchGlobalAllViewports()));

    QAction *globalBands = new QAction(parent);
    globalBands->setText("All Bands");
    globalMenu->addAction(globalBands);
    connect(globalBands, SIGNAL(triggered(bool)), this, SLOT(stretchGlobalAllBands()));

    p_globalButton = new QToolButton(); //basically acts as a 'reset'
    p_globalButton->setAutoRaise(true);
    p_globalButton->setIconSize(QSize(22, 22));
    p_globalButton->setPopupMode(QToolButton::MenuButtonPopup);
    p_globalButton->setMenu(globalMenu);
    p_globalButton->setDefaultAction(currentView);
    p_globalButton->setToolTip("Reset");
    text  =
      "<b>Function:</b> Reset the stretch to be automatically computed \
      using the statisics from the entire image. Use the drop down menu \
      to reset the stretch for all the bands in the active viewport or \
      to reset the stretch for all the viewports. </p>";
    p_globalButton->setWhatsThis(text);

    QPushButton *advancedButton = new QPushButton("Advanced");
    connect(advancedButton, SIGNAL(clicked()), this, SLOT(showAdvancedDialog()));

    p_flashButton = new QPushButton("Show Global");
    connect(p_flashButton, SIGNAL(pressed()), this, SLOT(stretchChanged()));
    connect(p_flashButton, SIGNAL(released()), this, SLOT(stretchChanged()));

    QHBoxLayout *layout = new QHBoxLayout(hbox);
    layout->setMargin(0);
    layout->addWidget(p_copyButton, Qt::AlignLeft);
    layout->addWidget(p_globalButton, Qt::AlignLeft);
    layout->addWidget(p_stretchRegionalButton, Qt::AlignLeft);
    layout->addWidget(p_stretchBandComboBox, Qt::AlignLeft);
    layout->addWidget(p_stretchMinEdit, Qt::AlignLeft);
    layout->addWidget(p_stretchMaxEdit, Qt::AlignLeft);
    layout->addWidget(advancedButton, Qt::AlignLeft);
    layout->addWidget(p_flashButton, Qt::AlignLeft);
    layout->addStretch(1); // Pushes everything else left in the menu bar
    hbox->setLayout(layout);

    return hbox;
  }


  /**
   * This updates the visible histograms in the advanced stretch,
   * if present.
   */
  void StretchTool::updateHistograms() {
    if(p_advancedStretch->isVisible()) {
      MdiCubeViewport *cvp = cubeViewport();

      if(!cvp) return;

      if(cvp->isGray() && !cvp->grayBuffer()->working()) {
        if(p_advancedStretch->isRgbMode()) {
          updateTool();
        }
        else {
          Isis::Histogram hist(histFromBuffer(cvp->grayBuffer()));

          if(hist.ValidPixels() > 0) {
            p_advancedStretch->updateHistogram(hist);
          }
        }
      }
      //Otherwise it is in color mode
      else if(!cvp->isGray() &&
              !cvp->redBuffer()->working() &&
              !cvp->greenBuffer()->working() &&
              !cvp->blueBuffer()->working()) {
        if(!p_advancedStretch->isRgbMode()) {
          updateTool();
        }
        else {
          Isis::Histogram redHist(histFromBuffer(cvp->redBuffer()));
          Isis::Histogram grnHist(histFromBuffer(cvp->greenBuffer()));
          Isis::Histogram bluHist(histFromBuffer(cvp->blueBuffer()));

          if(redHist.ValidPixels() > 0 &&
              grnHist.ValidPixels() > 0 &&
              bluHist.ValidPixels() > 0) {
            p_advancedStretch->updateHistograms(redHist, grnHist, bluHist);
          }
        }
      }
    }
  }


  /**
   * This is called when the visible area changes.
   */
  void StretchTool::screenPixelsChanged() {
    updateHistograms();
  }


  /**
   * This updates the advanced stretch to use the given viewport
   *
   * @param cvp
   */
  void StretchTool::setCubeViewport(CubeViewport *cvp) {
    if(p_advancedStretch->isVisible()) {
      p_advancedStretch->enable(true);

      //If the viewport is in gray mode
      if(cvp->isGray() && !cvp->grayBuffer()->working()) {
        Isis::Histogram hist(histFromBuffer(cvp->grayBuffer()));
        Isis::Stretch stretch(cvp->grayStretch());

        p_advancedStretch->enableGrayMode(stretch, hist);
      }
      //Otherwise it is in color mode
      else if(!cvp->isGray() &&
              !cvp->redBuffer()->working() &&
              !cvp->greenBuffer()->working() &&
              !cvp->blueBuffer()->working()) {
        Isis::Histogram redHist(histFromBuffer(cvp->redBuffer()));
        Isis::Histogram grnHist(histFromBuffer(cvp->greenBuffer()));
        Isis::Histogram bluHist(histFromBuffer(cvp->blueBuffer()));
        Isis::Stretch redStretch(cvp->redStretch());
        Isis::Stretch grnStretch(cvp->greenStretch());
        Isis::Stretch bluStretch(cvp->blueStretch());

        p_advancedStretch->enableRgbMode(redStretch, redHist,
                                         grnStretch, grnHist,
                                         bluStretch, bluHist);
      }
      else {
        p_advancedStretch->enable(false);
      }
    }
    else {
      p_advancedStretch->enable(false);
    }
  }


  /**
   * Updates the stretch tool.
   *
   */
  void StretchTool::updateTool() {
    CubeViewport *cvp = cubeViewport();

    if(cvp == NULL) {
      //If the current viewport is NULL and the advanced dialog is visible, hide it
      if(p_advancedStretch->isVisible()) {
        p_advancedStretch->hide();
      }
    }
    else {
      if(!p_advancedStretch->enabled() ||
          p_advancedStretch->isRgbMode() != !cvp->isGray()) {
        setCubeViewport(cvp);
      }
    }

    if(cvp && cvp->isGray()) {
      p_copyBands->setEnabled(true);
      p_stretchBandComboBox->setShown(false);
    }
    else if(cvp) {
      p_copyBands->setEnabled(true);
      p_stretchBandComboBox->setShown(true);
    }
    else {
      p_copyBands->setEnabled(false);
      p_stretchBandComboBox->setShown(false);
    }

    if(p_advancedStretch->isVisible()) {
      p_stretchMinEdit->setEnabled(false);
      p_stretchMaxEdit->setEnabled(false);
    }
    else {
      p_stretchMinEdit->setEnabled(true);
      p_stretchMaxEdit->setEnabled(true);
    }

    updateHistograms();
    stretchChanged();
  }


  /**
   * The cube viewport requested a stretch at this time, give it
   * to the viewport
   *
   * @param cvp
   * @param bandId
   */
  void StretchTool::stretchRequested(MdiCubeViewport *cvp, int bandId) {
    // Yeah this is a hack... but it's necessary to make this tool
    //   do anything while its not the active tool.
    connect(cvp, SIGNAL(screenPixelsChanged()), this, SLOT(updateHistograms()));

    QRect rect(0, 0, cvp->viewport()->width(), cvp->viewport()->height());

    if(bandId == (int)Gray) {
      if(cvp->grayBuffer() && cvp->grayBuffer()->hasEntireCube()) {
        Stretch newStretch = cvp->grayStretch();
        newStretch.CopyPairs(stretchBuffer(cvp->grayBuffer(), rect));
        cvp->stretchGray(newStretch);
      }
      else {
        Stretch newStretch = stretchBand(cvp, (StretchBand)bandId);
        cvp->stretchGray(newStretch);
      }
    }
    else if(bandId == (int)Red) {
      if(cvp->redBuffer() && cvp->redBuffer()->hasEntireCube()) {
        Stretch newStretch = cvp->redStretch();
        newStretch.CopyPairs(stretchBuffer(cvp->redBuffer(), rect));
        cvp->stretchRed(newStretch);
      }
      else {
        Stretch newStretch = stretchBand(cvp, (StretchBand)bandId);
        cvp->stretchRed(newStretch);
      }
    }
    else if(bandId == (int)Green) {
      if(cvp->greenBuffer() && cvp->greenBuffer()->hasEntireCube()) {
        Stretch newStretch = cvp->greenStretch();
        newStretch.CopyPairs(stretchBuffer(cvp->greenBuffer(), rect));
        cvp->stretchGreen(newStretch);
      }
      else {
        Stretch newStretch = stretchBand(cvp, (StretchBand)bandId);
        cvp->stretchGreen(newStretch);
      }
    }
    else if(bandId == (int)Blue) {
      if(cvp->blueBuffer() && cvp->blueBuffer()->hasEntireCube()) {
        Stretch newStretch = cvp->blueStretch();
        newStretch.CopyPairs(stretchBuffer(cvp->blueBuffer(), rect));
        cvp->stretchBlue(newStretch);
      }
      else {
        Stretch newStretch = stretchBand(cvp, (StretchBand)bandId);
        cvp->stretchBlue(newStretch);
      }
    }

    stretchChanged();
  }


  /**
   * This method is called when the stretch has changed and sets the min/max
   * text fields to the correct values.
   *
   */
  void StretchTool::stretchChanged() {
    MdiCubeViewport *cvp = cubeViewport();
    if(cvp == NULL) return;

    if(p_flashButton->isDown()) {
      if(!p_preGlobalStretches) {
        p_preGlobalStretches = new Stretch[4];
        p_preGlobalStretches[0] = cvp->grayStretch();
        p_preGlobalStretches[1] = cvp->redStretch();
        p_preGlobalStretches[2] = cvp->greenStretch();
        p_preGlobalStretches[3] = cvp->blueStretch();
      }

      cvp->stretchKnownGlobal();
      return;
    }
    else if(p_preGlobalStretches) {
      if(cvp->isGray()) {
        cvp->stretchGray(p_preGlobalStretches[0]);
      }
      else {
        cvp->stretchRed(p_preGlobalStretches[1]);
        cvp->stretchGreen(p_preGlobalStretches[2]);
        cvp->stretchBlue(p_preGlobalStretches[3]);
      }

      delete [] p_preGlobalStretches;
      p_preGlobalStretches = NULL;
    }

    double min = 0, max = 0;
    //If the viewport is in gray mode
    if(cvp->isGray()) {
      //Get the min/max from the current stretch
      Isis::Stretch stretch = cvp->grayStretch();
      min = stretch.Input(0);
      max = stretch.Input(stretch.Pairs() - 1);
      
      // send the stretch to any ChipViewports that want to listen
      *p_chipViewportStretch = stretch;
      emit stretchChipViewport(p_chipViewportStretch, cvp);
    }

    //Otherwise it is in color mode
    else {
      Isis::Stretch rstretch = cvp->redStretch();
      Isis::Stretch gstretch = cvp->greenStretch();
      Isis::Stretch bstretch = cvp->blueStretch();

      //Get the min/max from the current stretch
      if(p_stretchBand == Red) {
        min = rstretch.Input(0);
        max = rstretch.Input(rstretch.Pairs() - 1);
      }
      else if(p_stretchBand == Green) {
        min = gstretch.Input(0);
        max = gstretch.Input(gstretch.Pairs() - 1);
      }
      else if(p_stretchBand == Blue) {
        min = bstretch.Input(0);
        max = bstretch.Input(bstretch.Pairs() - 1);
      }
    }

    //Set the min/max text fields
    QString strMin;
    strMin.setNum(min);
    p_stretchMinEdit->setText(strMin);

    QString strMax;
    strMax.setNum(max);
    p_stretchMaxEdit->setText(strMax);

    if(p_advancedStretch->isVisible()) {
      p_advancedStretch->updateStretch(cvp);
    }
  }


  /**
   * This is called when one of the advanced stretches changed.
   * Give the stretch to the viewport.
   */
  void StretchTool::advancedStretchChanged() {
    CubeViewport *cvp = cubeViewport();
    if(cvp == NULL) return;

    if(!p_advancedStretch->isRgbMode()) {
      Stretch grayStretch = cvp->grayStretch();
      grayStretch.ClearPairs();
      grayStretch.CopyPairs(p_advancedStretch->getGrayStretch());
      cvp->stretchGray(grayStretch);
    }
    else {
      Stretch redStretch = cvp->redStretch();
      redStretch.ClearPairs();
      redStretch.CopyPairs(p_advancedStretch->getRedStretch());
      cvp->stretchRed(redStretch);

      Stretch grnStretch = cvp->greenStretch();
      grnStretch.ClearPairs();
      grnStretch.CopyPairs(p_advancedStretch->getGrnStretch());
      cvp->stretchGreen(grnStretch);

      Stretch bluStretch = cvp->blueStretch();
      bluStretch.ClearPairs();
      bluStretch.CopyPairs(p_advancedStretch->getBluStretch());
      cvp->stretchBlue(bluStretch);
    }

    stretchChanged();
  }


  /**
   * This method is called when the stretch has changed and sets the min/max
   * text fields to the correct values.
   *
   */
  void StretchTool::changeStretch() {
    MdiCubeViewport *cvp = cubeViewport();
    if(cvp == NULL) return;

    // Make sure the user didn't enter bad min/max and if so fix it
    double min = p_stretchMinEdit->text().toDouble();
    double max = p_stretchMaxEdit->text().toDouble();

    if(min >= max || p_stretchMinEdit->text() == "" ||
        p_stretchMaxEdit->text() == "") {
      updateTool();
      return;
    }

    //The viewport is in gray mode
    if(cvp->isGray()) {
      Isis::Stretch stretch = cvp->grayStretch();
      stretch.ClearPairs();
      stretch.AddPair(min, 0.0);
      stretch.AddPair(max, 255.0);

      cvp->stretchGray(stretch);
    }
    //Otherwise the viewport is in color mode
    else {
      Isis::Stretch redStretch = cvp->redStretch();
      Isis::Stretch greenStretch = cvp->greenStretch();
      Isis::Stretch blueStretch = cvp->blueStretch();

      if(p_stretchBand == Red) {
        redStretch.ClearPairs();
        redStretch.AddPair(min, 0.0);
        redStretch.AddPair(max, 255.0);
      }
      if(p_stretchBand == Green) {
        greenStretch.ClearPairs();
        greenStretch.AddPair(min, 0.0);
        greenStretch.AddPair(max, 255.0);
      }
      if(p_stretchBand == Blue) {
        blueStretch.ClearPairs();
        blueStretch.AddPair(min, 0.0);
        blueStretch.AddPair(max, 255.0);
      }

      cvp->stretchRed(redStretch);
      cvp->stretchGreen(greenStretch);
      cvp->stretchBlue(blueStretch);
    }

    stretchChanged();
  }


  /**
   * This methods shows and updates the advanced dialog.
   *
   */
  void StretchTool::showAdvancedDialog() {
    if(p_advancedStretch->isVisible()) return;

    if(cubeViewport()) {
      p_advancedStretch->updateStretch(cubeViewport());
      p_advancedStretch->show();
    }


    updateTool();
  }


  /**
   * Does a global stretch for the active viewport.
   *
   */
  void StretchTool::stretchGlobal() {
    CubeViewport *cvp = cubeViewport();
    if(cvp == NULL) return;

    stretchGlobal(cvp);
  }


  /**
   * This resets the stretch across all bands
   */
  void StretchTool::stretchGlobalAllBands() {
    CubeViewport *cvp = cubeViewport();
    if(cvp == NULL) return;

    cvp->forgetStretches();
    stretchGlobal(cvp);
  }


  /**
   * Does a global stretch for the specified viewport.
   *
   */
  void StretchTool::stretchGlobal(CubeViewport *cvp) {
    cvp->stretchKnownGlobal();
    stretchChanged();
  }


  /**
   * Does a global stretch for all the viewports.
   *
   */
  void StretchTool::stretchGlobalAllViewports() {
    for(int i = 0; i < (int)cubeViewportList()->size(); i++) {
      CubeViewport *cvp = cubeViewportList()->at(i);

      stretchGlobal(cvp);
    }
  }


  /**
   * Does a regional stretch for the active viewport.
   *
   */
  void StretchTool::stretchRegional() {
    CubeViewport *cvp = cubeViewport();
    if(cvp == NULL) return;

    stretchRegional(cvp);
  }

  /**
   * Does a regional stretch for the specified viewport.
   *
   */
  void StretchTool::stretchRegional(CubeViewport *cvp) {
    QRect rect(0, 0, cvp->viewport()->width(), cvp->viewport()->height());

    stretchRect(cvp, rect);
  }


  /**
   * This method is called when the RubberBandTool is complete. It
   * will get a rectangle from the RubberBandTool and stretch
   * accordingly.
   *
   */
  void StretchTool::rubberBandComplete() {
    CubeViewport *cvp = cubeViewport();
    if(cvp == NULL) return;
    if(!RubberBandTool::isValid()) return;

    QRect rubberBandRect = RubberBandTool::rectangle();
    //Return if the width or height is zero
    if(rubberBandRect.width() == 0 || rubberBandRect.height() == 0) return;
    
    stretchRect(cvp, rubberBandRect);
  }
  
  
  /**
   * stretch the specified CubeViewport with the given rect
   *
   * @param cvp The CubeViewport to stretch
   * @param rect The rect with which to stretch the CubeViewport
   */
   void StretchTool::stretchRect(CubeViewport *cvp, QRect rect) {
     Stretch newStretch;
   
     if(cvp->isGray()) {
        newStretch = cvp->grayStretch();
        newStretch.ClearPairs();
        newStretch.CopyPairs(stretchBuffer(cvp->grayBuffer(), rect));
        cvp->stretchGray(newStretch);
      }
      else {
        switch(p_stretchBand) {
          case Red:
            newStretch = cvp->redStretch();
            newStretch.ClearPairs();
            newStretch.CopyPairs(stretchBuffer(cvp->redBuffer(), rect));
            cvp->stretchRed(newStretch);
            break;
          case Green:
            newStretch = cvp->greenStretch();
            newStretch.ClearPairs();
            newStretch.CopyPairs(stretchBuffer(cvp->greenBuffer(), rect));
            cvp->stretchGreen(newStretch);
            break;
          case Blue:
            newStretch = cvp->blueStretch();
            newStretch.ClearPairs();
            newStretch.CopyPairs(stretchBuffer(cvp->blueBuffer(), rect));
            cvp->stretchBlue(newStretch);
            break;
          default:
            throw iException::Message(iException::Programmer,
                                      "Unknown stretch band", _FILEINFO_);
        }
      }
      
      stretchChanged();
   }
  
  


  /**
   * This method will call a global stretch if the right mouse
   * button is released.
   *
   * @param start
   * @param s
   */
  void StretchTool::mouseButtonRelease(QPoint start, Qt::MouseButton s) {
    CubeViewport *cvp = cubeViewport();
    if(cvp == NULL) return;

    // Call the parent Tool function to reset the Warning as different activity is
    // taking place
    Qisis::Tool::mouseButtonRelease(start, s);

    if(s == Qt::RightButton) {
      stretchGlobal(cvp);
      
      // notify any ChipViewports listening that the CubeViewport was stretched
      // back to global
      emit stretchChipViewport(NULL, cvp);

      // Resets the RubberBandTool on screen.
      enableRubberBandTool();
    }
  }

  /**
   * This method enables the RubberBandTool.
   *
   */
  void StretchTool::enableRubberBandTool() {
    RubberBandTool::enable(RubberBandTool::Rectangle);
  }


  /**
   * Sets the stretch for all the bands in the active viewport to
   * the current stretch
   *
   */
  void StretchTool::setStretchAcrossBands() {
    CubeViewport *cvp = cubeViewport();
    if(cvp == NULL) return;

    double min = p_stretchMinEdit->text().toDouble();
    double max = p_stretchMaxEdit->text().toDouble();

    Isis::Stretch stretch;

    if(cvp->isGray()) {
      stretch = cvp->grayStretch();
      stretch.ClearPairs();
      stretch.AddPair(min, 0.0);
      stretch.AddPair(max, 255.0);
    }
    else if(p_stretchBand == Red) {
      stretch = cvp->redStretch();
      stretch.ClearPairs();
      stretch.AddPair(min, 0.0);
      stretch.AddPair(max, 255.0);
      cvp->stretchGreen(stretch);
      cvp->stretchBlue(stretch);
    }
    else if(p_stretchBand == Green) {
      stretch = cvp->greenStretch();
      stretch.ClearPairs();
      stretch.AddPair(min, 0.0);
      stretch.AddPair(max, 255.0);
      cvp->stretchRed(stretch);
      cvp->stretchBlue(stretch);
    }
    else if(p_stretchBand == Blue) {
      stretch = cvp->blueStretch();
      stretch.ClearPairs();
      stretch.AddPair(min, 0.0);
      stretch.AddPair(max, 255.0);
      cvp->stretchRed(stretch);
      cvp->stretchGreen(stretch);
    }
    else {
      return;
    }

    cvp->setAllBandStretches(stretch);
  }


  /**
   * Sets the stretch for all the viewports to the current
   * stretch in the active viewport.
   *
   */
  void StretchTool::setStretchAllViewports() {
    CubeViewport *thisViewport = cubeViewport();

    if(thisViewport == NULL) return;

    for(int i = 0; i < (int)cubeViewportList()->size(); i++) {
      CubeViewport *cvp = cubeViewportList()->at(i);

      if(thisViewport->isGray() && cvp->isGray()) {
        Stretch newStretch(cvp->grayStretch());
        newStretch.CopyPairs(thisViewport->grayStretch());
        cvp->stretchGray(newStretch);
      }
      else if(!thisViewport->isGray() && !cvp->isGray()) {
        Stretch newStretchRed(cvp->redStretch());
        newStretchRed.CopyPairs(thisViewport->redStretch());
        cvp->stretchRed(newStretchRed);

        Stretch newStretchGreen(cvp->greenStretch());
        newStretchGreen.CopyPairs(thisViewport->greenStretch());
        cvp->stretchGreen(newStretchGreen);

        Stretch newStretchBlue(cvp->blueStretch());
        newStretchBlue.CopyPairs(thisViewport->blueStretch());
        cvp->stretchBlue(newStretchBlue);
      }
      else if(!thisViewport->isGray() && cvp->isGray()) {
        Stretch newStretch(cvp->grayStretch());

        if(p_stretchBand == Red) {
          newStretch.CopyPairs(thisViewport->redStretch());
        }
        else if(p_stretchBand == Green) {
          newStretch.CopyPairs(thisViewport->greenStretch());
        }
        else if(p_stretchBand == Blue) {
          newStretch.CopyPairs(thisViewport->blueStretch());
        }

        cvp->stretchGray(newStretch);
      }
      else if(thisViewport->isGray() && !cvp->isGray()) {
        // don't copy gray stretches to rgb
      }
    }

    stretchChanged();
  }


  /**
   * This method computes the stretch over a region using the viewport buffer.
   *
   * @param buffer
   * @param rect
   */
  Isis::Stretch StretchTool::stretchBuffer(ViewportBuffer *buffer, QRect rect) {
    //Get the statistics and histogram from the region
    Isis::Statistics stats = statsFromBuffer(buffer, rect);
    Isis::Stretch stretch;

    if(stats.ValidPixels() > 1 &&
        fabs(stats.Minimum() - stats.Maximum()) > DBL_EPSILON) {
      Isis::Histogram hist = histFromBuffer(buffer, rect,
                                            stats.BestMinimum(), stats.BestMaximum());

      if(fabs(hist.Percent(0.5) - hist.Percent(99.5)) > DBL_EPSILON) {
        stretch.AddPair(hist.Percent(0.5), 0.0);
        stretch.AddPair(hist.Percent(99.5), 255.0);
      }
    }

    if(stretch.Pairs() == 0) {
      stretch.AddPair(-DBL_MAX, 0.0);
      stretch.AddPair(DBL_MAX, 255.0);
    }

    return stretch;
  }


  /**
   * This method computes the stretch over the entire cube.
   *
   * @param cvp
   * @param band Band to stretch
   */
  Isis::Stretch StretchTool::stretchBand(CubeViewport *cvp, StretchBand band) {
    int bandNum = cvp->grayBand();
    Isis::Stretch stretch = cvp->grayStretch();

    if(band == Red) {
      bandNum = cvp->redBand();
      stretch = cvp->redStretch();
    }
    else if(band == Green) {
      bandNum = cvp->greenBand();
      stretch = cvp->greenStretch();
    }
    else if(band == Blue) {
      bandNum = cvp->blueBand();
      stretch = cvp->blueStretch();
    }

    Isis::Statistics stats = statsFromCube(cvp->cube(), bandNum);
    Isis::Histogram  hist = histFromCube(cvp->cube(), bandNum,
                                         stats.BestMinimum(), stats.BestMaximum());

    stretch.ClearPairs();
    if(fabs(hist.Percent(0.5) - hist.Percent(99.5)) > DBL_EPSILON) {
      stretch.AddPair(hist.Percent(0.5), 0.0);
      stretch.AddPair(hist.Percent(99.5), 255.0);
    }
    else {
      stretch.AddPair(-DBL_MAX, 0.0);
      stretch.AddPair(DBL_MAX, 255.0);
    }

    return stretch;
  }

  /**
   * This method will calculate and return the statistics for a given cube and
   * band.
   *
   * @param cube
   * @param band
   *
   * @return Isis::Statistics
   */
  Isis::Statistics StretchTool::statsFromCube(Isis::Cube *cube, int band) {
    Isis::Statistics stats;
    Isis::Brick brick(cube->Samples(), 1, 1, cube->PixelType());

    for(int line = 0; line < cube->Lines(); line++) {
      brick.SetBasePosition(0, line, band);
      cube->Read(brick);
      stats.AddData(brick.DoubleBuffer(), cube->Samples());
    }

    return stats;
  }


  /**
   * This method will calculate and return the statistics for a given region and
   * viewport buffer.
   *
   * @param buffer
   * @param rect
   *
   * @return Isis::Statistics
   */
  Isis::Statistics StretchTool::statsFromBuffer(ViewportBuffer *buffer,
      QRect rect) {
    if(buffer->working()) {
      throw iException::Message(iException::User,
                                "Cannot stretch while the cube is still loading", _FILEINFO_);
    }

    QRect dataArea = QRect(buffer->bufferXYRect().intersected(rect));
    Isis::Statistics stats;

    for(int y = dataArea.top();
        !dataArea.isNull() && y <= dataArea.bottom();
        y++) {
      const std::vector<double> &line = buffer->getLine(y - buffer->bufferXYRect().top());

      for(int x = dataArea.left(); x < dataArea.right(); x++) {
        stats.AddData(line[x - buffer->bufferXYRect().left()]);
      }
    }

    return stats;
  }

  /**
   * This method will calculate and return the histogram for a given cube and
   * band.
   *
   * @param cube
   * @param band
   * @param min
   * @param max
   *
   * @return Isis::Histogram
   */
  Isis::Histogram StretchTool::histFromCube(Isis::Cube *cube, int band,
      double min, double max) {
    Isis::Histogram hist(min, max);
    Isis::Brick brick(cube->Samples(), 1, 1, cube->PixelType());

    for(int line = 0; line < cube->Lines(); line++) {
      brick.SetBasePosition(0, line, band);
      cube->Read(brick);
      hist.AddData(brick.DoubleBuffer(), cube->Samples());
    }

    return hist;
  }


  /**
   * Given a viewport buffer, this calculates a histogram
   *
   * @param buffer
   *
   * @return Isis::Histogram
   */
  Isis::Histogram StretchTool::histFromBuffer(ViewportBuffer *buffer) {
    Isis::Statistics stats = statsFromBuffer(buffer, buffer->bufferXYRect());
    return histFromBuffer(buffer, buffer->bufferXYRect(),
                          stats.BestMinimum(), stats.BestMaximum());

  }


  /**
   * This method will calculate and return the histogram for a given region and
   * viewport buffer.
   *
   * @param buffer
   * @param rect
   * @param min
   * @param max
   *
   * @return Isis::Histogram
   */
  Isis::Histogram StretchTool::histFromBuffer(ViewportBuffer *buffer,
      QRect rect, double min, double max) {
    QRect dataArea = QRect(buffer->bufferXYRect().intersected(rect));

    try {
      Isis::Histogram hist(min, max);

      for(int y = dataArea.top(); !dataArea.isNull() && y <= dataArea.bottom(); y++) {
        const std::vector<double> &line = buffer->getLine(y - buffer->bufferXYRect().top());
        hist.AddData(&line.front() + (dataArea.left() - buffer->bufferXYRect().left()), dataArea.width());
      }

      return hist;
    }
    catch(Isis::iException &e) {
      // get the min and max DN values of the data area
      Isis::iString sMin(min);
      Isis::iString sMax(max);
      std::string msg = "Insufficient data Min [" + sMin + "], Max [" + sMax + "]";
      msg += " in the stretch area.";

      // Emit signal to the parent tool to display Warning object with the warning message
      //emit warningSignal(msg, e.Errors());

      throw Isis::iException::Message(Isis::iException::None, msg, _FILEINFO_);
    }
  }



  /**
   * The selected band for stretching changed.
   */
  void StretchTool::stretchBandChanged(int) {
    p_stretchBand = (StretchBand) p_stretchBandComboBox->itemData(
                      p_stretchBandComboBox->currentIndex()
                    ).toInt();

    stretchChanged();
  }


  /**
   * This method is called when the flash button is pressed on the advanced dialog
   * and sets the viewport's stretch to the global stretch.
   *
   *
  void StretchTool::flash()
  {
    CubeViewport *cvp = cubeViewport(); if(cvp != NULL) {
      if(cvp->isGray()) {
        p_viewportMap[cvp][cvp->grayBand()-1]->currentStretch = p_viewportMap[cvp][cvp->grayBand()-1]->stretch->stretch();
      }
      else {
        p_viewportMap[cvp][cvp->redBand()-1]->currentStretch = p_viewportMap[cvp][cvp->redBand()-1]->stretch->stretch();
        p_viewportMap[cvp][cvp->greenBand()-1]->currentStretch = p_viewportMap[cvp][cvp->greenBand()-1]->stretch->stretch();
        p_viewportMap[cvp][cvp->blueBand()-1]->currentStretch = p_viewportMap[cvp][cvp->blueBand()-1]->stretch->stretch();
      }

      //Update the stretch
      stretchChanged();
    }
  }*/


  /**
   * This method will save the current stretch's stretch pairs to the specified
   * file name.
   *
   *
  void StretchTool::savePairsAs()
  {
    QString fn = QFileDialog::getSaveFileName((QWidget *)parent(),
                 "Choose filename to save under",
                 ".",
                 "Text Files (*.txt)");
    QString filename;

    //Make sure the filename is valid
    if(!fn.isEmpty())
    {
      if(!fn.endsWith(".txt"))
      {
        filename = fn + ".txt";
      }
      else
      {
        filename = fn;
      }
    }
    //The user cancelled, or the filename is empty
    else
    {
      return;
    }

    if(p_advancedStretch->isRgbMode())
    {
      Filename redFilename(filename.toStdString());
      Filename grnFilename(filename.toStdString());
      Filename bluFilename(filename.toStdString());

      redFilename.AddExtension("red");
      grnFilename.AddExtension("grn");
      bluFilename.AddExtension("blu");

      QFile redFile(QString::fromStdString(redFilename.Expanded()));
      QFile grnFile(QString::fromStdString(grnFilename.Expanded()));
      QFile bluFile(QString::fromStdString(bluFilename.Expanded()));
      bool success = redFile.open(QIODevice::WriteOnly) &&
                     grnFile.open(QIODevice::WriteOnly) &&
                     bluFile.open(QIODevice::WriteOnly);
      if(!success)
      {
        QMessageBox::critical((QWidget *)parent(),
                              "Error", "Cannot open file, please check permissions");
        return;
      }

      QTextStream redStream(&redFile);
      QTextStream grnStream(&grnFile);
      QTextStream bluStream(&bluFile);

      Isis::Stretch red = p_advancedStretch->getRedStretch();
      Isis::Stretch grn = p_advancedStretch->getGrnStretch();
      Isis::Stretch blu = p_advancedStretch->getBluStretch();

      redStream << red.Text().c_str() << endl;
      grnStream << grn.Text().c_str() << endl;
      bluStream << blu.Text().c_str() << endl;

      redFile.close();
      grnFile.close();
      bluFile.close();
    }
    else
    {
      QFile file;
      bool success = file.open(QIODevice::WriteOnly);
      if(!success)
      {
        QMessageBox::critical((QWidget *)parent(),
                              "Error", "Cannot open file, please check permissions");
        return;
      }

      QTextStream stream(&file);

      Isis::Stretch stretch = p_advancedStretch->getGrayStretch();

      //Add the pairs to the file
      stream << stretch.Text().c_str() << endl;

      file.close();
    }
  }*/
}
