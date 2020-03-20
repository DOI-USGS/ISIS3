#ifndef ADVANCEDSTRETCHDIALOG_H
#define ADVANCEDSTRETCHDIALOG_H

#include <QDialog>

class QShowEvent;
class QWidget;

namespace Isis {
  class Stretch;
  class Histogram;
};

namespace Isis {
  class AdvancedStretch;
  class CubeViewport;

  /**
   * @brief Advanced Stretch Dialog
   *
   * @ingroup Visualization Tools
   *
   * @author 2010-05-20 Steven Lambright
   *
   * @internal
   *   @history 2011-03-22 Sharmila Prasad - Added API updateForRGBMode to
   *        accomodate changes for all Bands for BandID All.
   */
  class AdvancedStretchDialog : public QDialog {
      Q_OBJECT

    public:
      AdvancedStretchDialog(QWidget *parent);
      ~AdvancedStretchDialog();

      void enableRgbMode(Stretch &redStretch, Histogram &redHist,
                         Stretch &grnStretch, Histogram &grnHist,
                         Stretch &bluStretch, Histogram &bluHist);
      void updateHistograms(const Histogram &redHist,
                            const Histogram &grnHist,
                            const Histogram &bluHist);

      void updateForRGBMode(Stretch &redStretch, Histogram &redHist,
                            Stretch &grnStretch, Histogram &grnHist,
                            Stretch &bluStretch, Histogram &bluHist);

      void enableGrayMode(Stretch &grayStretch,
                          Histogram &grayHist);
      void updateHistogram(const Histogram &grayHist);
      bool isRgbMode() const;
      void restoreSavedStretch(Stretch stretch); 

      Stretch getGrayStretch();
      Stretch getRedStretch();
      Stretch getGrnStretch();
      Stretch getBluStretch();

      /**
       * Returns true if the advanced stretch is enabled
       *
       * @return
       */
      bool enabled() {
        return p_enabled;
      }

      /**
       * Sets the enabled state to enable
       *
       * @param enable
       */
      void enable(bool enable) {
        p_enabled = enable;
      }

    signals:
      //! Emitted when an advanced stretch has changed
      void stretchChanged();
      //! Emitted when this dialog is shown or hidden
      void visibilityChanged();
      void saveToCube(); 
      void deleteFromCube(); 
      void loadStretch(); 

    public slots:
      void updateStretch(CubeViewport *);

    protected slots:
      void showEvent(QShowEvent *);
      void hideEvent(QHideEvent *);

    private:
      void destroyCurrentStretches();

    private:
      bool p_enabled; //!< True if advanced stretch should be used
      AdvancedStretch *p_grayStretch; //!< Gray stretch pane
      AdvancedStretch *p_redStretch;  //!< Red stretch pane
      AdvancedStretch *p_grnStretch;  //!< Green stretch pane
      AdvancedStretch *p_bluStretch;  //!< Blue stretch pane

  };
};


#endif
