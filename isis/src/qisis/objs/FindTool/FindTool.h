#ifndef FindTool_h
#define FindTool_h
/**
 * @file
 * $Revision: 1.13 $
 * $Date: 2010/06/28 09:06:59 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

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
                                double lat, double lon) const;
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
