#include "AdvancedStretchDialog.h"

#include <QHBoxLayout>
#include <QString>

#include "CubeViewport.h"
#include "Stretch.h"
#include "AdvancedStretch.h"

namespace Isis {
  /**
   * This constructs an advanced stretch.
   *
   * @param parent the parent widget
   */
  AdvancedStretchDialog::AdvancedStretchDialog(QWidget *parent) :
    QDialog(parent) {
    p_grayStretch = NULL;
    p_redStretch = NULL;
    p_grnStretch = NULL;
    p_bluStretch = NULL;
    p_enabled = false;

    setWindowTitle("Advanced Stretch Tool");

    QHBoxLayout *layout = new QHBoxLayout();
    
    setLayout(layout);
  }


  /**
   * This destroys the advanced stretch dialog
   */
  AdvancedStretchDialog::~AdvancedStretchDialog() {
    destroyCurrentStretches();
  }


  /**
   * This displays RGB advanced stretches.
   *
   * @param redStretch
   * @param redHist
   * @param grnStretch
   * @param grnHist
   * @param bluStretch
   * @param bluHist
   */
  void AdvancedStretchDialog::enableRgbMode(Stretch &redStretch,
      Histogram &redHist,
      Stretch &grnStretch, Histogram &grnHist,
      Stretch &bluStretch, Histogram &bluHist) {
    destroyCurrentStretches();

    p_redStretch = new AdvancedStretch(redHist, redStretch,
                                       "Red", QColor(Qt::red));
    layout()->addWidget(p_redStretch);

    p_grnStretch = new AdvancedStretch(grnHist, grnStretch,
                                       "Green", QColor(Qt::green));
    layout()->addWidget(p_grnStretch);

    p_bluStretch = new AdvancedStretch(bluHist, bluStretch,
                                       "Blue", QColor(Qt::blue));
    layout()->addWidget(p_bluStretch);
    updateGeometry();

    connect(p_redStretch, SIGNAL(stretchChanged()),
            this, SIGNAL(stretchChanged()));
    connect(p_grnStretch, SIGNAL(stretchChanged()),
            this, SIGNAL(stretchChanged()));
    connect(p_bluStretch, SIGNAL(stretchChanged()),
            this, SIGNAL(stretchChanged()));
  }

  /**
   * Update the stretch and histogram for all the bands
   * for All BandId option.
   *
   * @author Sharmila Prasad (3/14/2011)
   *
   * @param redStretch - Updated Red Stretch
   * @param redHist    - Updated Red Histogram
   * @param grnStretch - Updated Green Stretch
   * @param grnHist    - Updated Green Histogram
   * @param bluStretch - Updated Blue Stretch
   * @param bluHist    - Updated Blue Histogram
   */
  void AdvancedStretchDialog::updateForRGBMode(Stretch &redStretch,
      Histogram &redHist,
      Stretch &grnStretch, Histogram &grnHist,
      Stretch &bluStretch, Histogram &bluHist)
  {
    if(p_redStretch) {
      p_redStretch->setStretch(redStretch);
      p_redStretch->setHistogram(redHist);
    }

    if(p_grnStretch) {
      p_grnStretch->setStretch(grnStretch);
      p_grnStretch->setHistogram(grnHist);
    }

    if(p_bluStretch) {
      p_bluStretch->setStretch(bluStretch);
      p_bluStretch->setHistogram(bluHist);
    }
  }



  /**
   * This displays a gray advanced stretch.
   *
   * @param grayStretch
   * @param grayHist
   */
  void AdvancedStretchDialog::enableGrayMode(Stretch &grayStretch,
      Histogram &grayHist) {
    destroyCurrentStretches();

    p_grayStretch = new AdvancedStretch(grayHist, grayStretch,
                                        "Gray", QColor(Qt::gray));
    layout()->addWidget(p_grayStretch);
    updateGeometry();

    connect(p_grayStretch, SIGNAL(stretchChanged()),
            this, SIGNAL(stretchChanged()));
    connect(p_grayStretch, SIGNAL(saveToCube()),
            this, SIGNAL(saveToCube()));
    connect(p_grayStretch, SIGNAL(deleteFromCube()),
            this, SIGNAL(deleteFromCube()));
    connect(p_grayStretch, SIGNAL(loadStretch()),
            this, SIGNAL(loadStretch()));
  }


  /**
   * Restores a saved stretch from the cube 
   *  
   * @param stretch 
   */
  void AdvancedStretchDialog::restoreSavedStretch(Stretch stretch){
    p_grayStretch->restoreSavedStretch(stretch);
  }


  /**
   * This cleans up memory from currently displayed advanced
   * stretches. No stretches are visible after this is called.
   */
  void AdvancedStretchDialog::destroyCurrentStretches() {
    if(p_redStretch) {
      layout()->removeWidget(p_redStretch);
      disconnect(p_redStretch, SIGNAL(stretchChanged()),
                 this, SIGNAL(stretchChanged()));
      delete p_redStretch;
      p_redStretch = NULL;
    }

    if(p_grnStretch) {
      layout()->removeWidget(p_grnStretch);
      disconnect(p_grnStretch, SIGNAL(stretchChanged()),
                 this, SIGNAL(stretchChanged()));
      delete p_grnStretch;
      p_grnStretch = NULL;
    }

    if(p_bluStretch) {
      layout()->removeWidget(p_bluStretch);
      disconnect(p_bluStretch, SIGNAL(stretchChanged()),
                 this, SIGNAL(stretchChanged()));
      delete p_bluStretch;
      p_bluStretch = NULL;
    }

    if(p_grayStretch) {
      layout()->removeWidget(p_grayStretch);
      disconnect(p_grayStretch, SIGNAL(stretchChanged()),
                 this, SIGNAL(stretchChanged()));
      delete p_grayStretch;
      p_grayStretch = NULL;
    }
  }


  /**
   * This calls setStretch on all applicable advanced stretches.
   * This should be called any time the cube viewport changes.
   *
   * @param cvp
   */
  void AdvancedStretchDialog::updateStretch(CubeViewport *cvp) {
    if(p_grayStretch){
      p_grayStretch->setStretch(cvp->grayStretch());
    }
    if(p_redStretch) {
      p_redStretch->setStretch(cvp->redStretch());
    }
    if(p_grnStretch){
      p_grnStretch->setStretch(cvp->greenStretch());
    }
    if(p_bluStretch){
      p_bluStretch->setStretch(cvp->blueStretch());
    }
  }


  /**
   * This calls setHistogram on all of the advanced stretches.
   * This should be called every time the visible area changes.
   *
   * @param redHist Histogram of visible area on red band
   * @param grnHist Histogram of visible area on green band
   * @param bluHist Histogram of visible area on blue band
   */
  void AdvancedStretchDialog::updateHistograms(const Histogram &redHist,
      const Histogram &grnHist,
      const Histogram &bluHist) {

    if(p_redStretch){
      p_redStretch->setHistogram(redHist);
    }
    if(p_grnStretch){
      p_grnStretch->setHistogram(grnHist);
    }
    if(p_bluStretch){
      p_bluStretch->setHistogram(bluHist);
    }
  }


  /**
   * This calls setHistogram on the gray advanced stretches. This
   * should be called every time the visible area changes.
   *
   * @param grayHist Histogram of visible area on gray band
   */
  void AdvancedStretchDialog::updateHistogram(const Histogram &grayHist) {
    if(p_grayStretch)
      p_grayStretch->setHistogram(grayHist);
  }


  /**
   * This is implemented to send a signal when visibility changes
   *
   * @param event
   */
  void AdvancedStretchDialog::showEvent(QShowEvent *event) {
    QDialog::showEvent(event);
    emit visibilityChanged();
  }


  /**
   * This is implemented to send a signal when visibility changes
   *
   * @param event
   */
  void AdvancedStretchDialog::hideEvent(QHideEvent *event) {
    QDialog::hideEvent(event);
    emit visibilityChanged();
  }


  /**
   * Returns true if the dialog is displaying the RGB advanced
   * stretches.
   *
   * @return bool
   */
  bool AdvancedStretchDialog::isRgbMode() const {
    return (p_redStretch && p_grnStretch && p_bluStretch);
  }


  /**
   * This returns the advanced stretch's stretch for gray.
   *
   * @return Stretch
   */
  Stretch AdvancedStretchDialog::getGrayStretch() {
    if(p_grayStretch) {
      return p_grayStretch->getStretch();
    }
    else {
      QString msg = "Gray mode not enabled, cannot get gray stretch";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * This returns the advanced stretch's stretch for red.
   *
   * @return Stretch
   */
  Stretch AdvancedStretchDialog::getRedStretch() {
    if(p_redStretch) {
      return p_redStretch->getStretch();
    }
    else {
      QString msg = "RGB mode not enabled, cannot get red stretch";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * This returns the advanced stretch's stretch for green.
   *
   * @return Stretch
   */
  Stretch AdvancedStretchDialog::getGrnStretch() {
    if(p_grnStretch) {
      return p_grnStretch->getStretch();
    }
    else {
      QString msg = "RGB mode not enabled, cannot get green stretch";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * This returns the advanced stretch's stretch for blue.
   *
   * @return Stretch
   */
  Stretch AdvancedStretchDialog::getBluStretch() {
    if(p_bluStretch) {
      return p_bluStretch->getStretch();
    }
    else {
      QString msg = "RGB mode not enabled, cannot get blue stretch";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }
}
