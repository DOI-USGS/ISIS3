#ifndef ADVANCEDSTRETCHDIALOG_H
#define ADVANCEDSTRETCHDIALOG_H

#include <QDialog>

class QShowEvent;
class QWidget;

namespace Isis {
  class Stretch;
  class Histogram;
};

namespace Qisis {
  class AdvancedStretch;
  class CubeViewport;

  /**
   * @brief Advanced Stretch Dialog
   *
   * @ingroup Visualization Tools
   *
   * @author Steven Lambright 2010-05-20
   *
   * @internal
   */

  class AdvancedStretchDialog : public QDialog {
      Q_OBJECT

    public:
      AdvancedStretchDialog(QWidget *parent);
      ~AdvancedStretchDialog();

      void enableRgbMode(Isis::Stretch &redStretch, Isis::Histogram &redHist,
                         Isis::Stretch &grnStretch, Isis::Histogram &grnHist,
                         Isis::Stretch &bluStretch, Isis::Histogram &bluHist);
      void updateHistograms(const Isis::Histogram &redHist,
                            const Isis::Histogram &grnHist,
                            const Isis::Histogram &bluHist);

      void enableGrayMode(Isis::Stretch &grayStretch,
                          Isis::Histogram &grayHist);
      void updateHistogram(const Isis::Histogram &grayHist);
      bool isRgbMode() const;

      Isis::Stretch getGrayStretch();
      Isis::Stretch getRedStretch();
      Isis::Stretch getGrnStretch();
      Isis::Stretch getBluStretch();

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
