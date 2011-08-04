#ifndef Qisis_TrackTool_h
#define Qisis_TrackTool_h

// This is the only include allowed in this file!
#include "Tool.h"


class QLabel;
class QStatusBar;

namespace Isis {
  class MdiCubeViewport;
  class WarningWidget;

  /**
   * @brief This tool is part of the Qisis namespace and displays the statusbar of the window.
   *      The status bar tracks the line, sample, lat ,lon and pixel values. Also displays Warning
   *        status for any application activity.
   *
   * @ingroup Visualization Tools
   *
   * @author
   *
   * @internal
   *
   *   @history 2010-03-18 Sharmila Prasad - Added the Warning widget to the
   *            status bar to display the error status both icon and message for
   *            any application activity
   *   @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport instead of
   *            CubeViewport.  Fixed include issues.
   *   @history 2011-04-25 Steven Lambright - Fixed updates at the bottom right
   */
  class TrackTool : public Tool {
      Q_OBJECT

    public:
      TrackTool(QStatusBar *parent);
      void displayWarning(std::string &pStr,  const std::string   &pExStr);
      void resetStatusWarning(void);
      QStatusBar *getStatusBar(void);

    public slots:
      virtual void mouseMove(QPoint p);
      virtual void mouseLeave();

    protected:
      void addConnections(MdiCubeViewport *cvp);
      void removeConnections(MdiCubeViewport *cvp);

    private slots:
      void locateCursor();

    private:
      void updateLabels(QPoint p);
      void clearLabels();

      QStatusBar *p_sbar;           //!< Status bar
      QLabel *p_sampLabel;          //!< Sample label
      QLabel *p_lineLabel;          //!< Line label
      QLabel *p_latLabel;           //!< Lat label
      QLabel *p_lonLabel;           //!< Lon label
      QLabel *p_grayLabel;          //!< Gray label
      QLabel *p_redLabel;           //!< Red label
      QLabel *p_grnLabel;           //!< Green label
      QLabel *p_bluLabel;           //!< Blue label
      WarningWidget *mWarningWidget;  //!< Warning Widget
  };
};

#endif
