#ifndef FindTool_h
#define FindTool_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// The only includes allowed in this file are parents of classes defined here.
#include <QWidget>
#include "Tool.h"

class QAction;
class QCheckBox;
class QLineEdit;
class QTabWidget;
class QToolButton;

namespace Isis {
  class Distance;
  class MdiCubeViewport;

  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class GroundTab : public QWidget {
      Q_OBJECT

    public:
      GroundTab(QWidget *parent = 0);

      QLineEdit *p_lonLineEdit;  //!< Input for longitude
      QLineEdit *p_latLineEdit;  //!< Input for latitude
  };

  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class ImageTab : public QWidget {
      Q_OBJECT

    public:
      ImageTab(QWidget *parent = 0);

      QLineEdit *p_sampLineEdit;  //!< Input for sample
      QLineEdit *p_lineLineEdit;  //!< Input for line
  };


  /**
   * @brief Tool to locate a point on a cube that is projected and/or has a camera
   *        model
   *
   * This tool is part of the Qisis namespace and allows the user to locate a
   * point on a cube that has been projected and/or has a camera model. It also
   * allows the user to link viewports and sync scales.
   *
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Unknown
   *
   * @internal

   *  @history 2008-06-25 Noah Hilt - Switched positions of the sample/line line
   *           edits and labels.
   *
   *  @history 2008-08-?? Stacy Alley - Added red dot which is draw
   *           at the specified lat/lon or line/sample.  Also, the
   *           red dot is draw in the corresponding spot of a
   *           linked image, if there is overlap in the two
   *           images.
   *  @history 2010-03-08 - Jeannie Walldren - The recordAction()
   *           slot and recordPoint() signal were created to
   *           connect in qview to the AdvancedTrackTool record()
   *           slot.
   *  @history 2010-03-24 Sharmila Prasad - Enable FindTool for no camera and/or
   *           projection image
   *  @history 2010-05-06 Eric Hyer - Class redesigned to work with new
   *                                  CubeViewport
   *  @history 2010-05-18 Eric Hyer - Moved button for showing/hiding the red
   *                                  dot from the dialog to the toolbar
   *  @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport instead of
   *           CubeViewport.  Also fixed include issues.
   *  @history 2011-09-22 Steven Lambright - Sync scale with projections now
   *                          uses a better guess for the equivalent scale.
   *                          Fixes #205.
   *  @history 2011-09-28 Steven Lambright - The last change would zoom the
   *                          active viewport. This no longer happens.
   *                          References #205
   *  @history 2011-11-01 Steven Lambright - Explicitly wrote out and fixed
   *                          equations for sync scale. Fixes #205
   *  @history 2011-11-03 Steven Lambright - Fixed find given a line/sample.
   *                          We were calling SetUniversalGround given
   *                          DBL_MAX which causes an infinite loop.
   *                          So when you looked for a line/sample it would
   *                          infinite loop. Sync scale does not work for
   *                          image coordinates - is it supposed to?
   *  @history 2013-01-10 Steven Lambright - Improved sync scale calculations
   *                          to work in more cases. Fixes #953.
   *  @history 2014-05-19 Ian Humphrey - Fixed CTRL+F shortcut and menu option to open Find Tool's
   *                          Find Latitude/Longitude Coordinate dialog window. Added help text
   *                          to Find Tool menu option. Minor coding standards fixes.
   *                          Fixes #2087.
   *  @history 2016-03-23 Makayla Shepherd - Added a check on the latitude
   *                          entered that caused an error message to pop up
   *                          continuously. Fixes #2130.
   */
  class FindTool : public Tool {
      Q_OBJECT

    public:
      FindTool(QWidget *parent);
      ~FindTool();
      void addTo(QMenu *menu);
      void paintViewport(MdiCubeViewport *vp, QPainter *painter);

      /**
       * This method returns the menu name associated with this tool.
       *
       *
       * @return QString
       */
      QString menuName() const {
        return "&Options";
      }

    signals:
      void recordPoint(QPoint p); //!< Emitted when point should be recorded

    protected:
      QAction *toolPadAction(ToolPad *toolpad);
      QWidget *createToolBarWidget(QStackedWidget *parent);
      void updateTool();
      void createDialog(QWidget *parent);

    protected slots:
      void mouseButtonRelease(QPoint p, Qt::MouseButton s);

    private slots:
      void handleOkClicked();
      void handleLinkClicked();
      void handleRecordClicked();
      void togglePointVisible();

    private: // methods
      void centerLinkedViewports();
      Distance distancePerPixel(MdiCubeViewport *viewport,
                                double lat, double lon);
      void refresh();

    private:
      QDialog *p_dialog;
      QAction *p_findPoint;
      QToolButton *p_showDialogButton;
      QToolButton *p_linkViewportsButton;
      QToolButton *p_togglePointVisibleButton;
      QCheckBox *p_syncScale;
      QLineEdit *p_statusEdit;
      QTabWidget *p_tabWidget;
      GroundTab *p_groundTab;
      ImageTab *p_imageTab;


    private:
      double p_line;
      double p_samp;

      double p_lat;
      double p_lon;

      bool p_pointVisible;
  };



};

#endif
