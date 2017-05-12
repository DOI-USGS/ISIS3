#ifndef ZoomTool_h
#define ZoomTool_h

/**
 * @file
 * $Date: 2010/06/28 09:24:10 $
 * $Revision: 1.7 $
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


// This should be the only include in this file!
#include "Tool.h"

// FIXME: remove this include
#include <QCursor>

class QAction;
class QLineEdit;

namespace Isis {
  /**
   * @brief Handles zoom operations for Isis qt apps
   *
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Jeff Anderson -  ???
   *
   * @internal
   *   @history 2007-03-21  Tracie Sucharski - Added zoomFitWidth and
   *                         zoomFitHeight slots and changed the zoomFit button
   *                         to contain a popup menu for "Fit to Width" & "Fit
   *                         to Height".
   *   @history 2008-05-23 Noah Hilt - Added RubberBandTool functionality and
   *                          changed the mouseButtonReleased Method.
   *   @history 2009-02-12 Steven Lambright - Fixed zooming in/out with
   *                          rectangle rubber band
   *   @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport instead of
   *                          CubeViewport.  Fixed some include issues.
   *   @history 2010-07-12 Jeannie Walldren - Created setScale() methods to
   *                          display a QMessageBox if a scale value could not
   *                          be set.
   *   @history 2010-07-14 Jeannie Walldren - Fixed bug that caused 1:1 to fail.
   *                          Added iExceptions if value is manually entered in
   *                          text box is invalid. Updated documentation.
   *   @history 2010-12-22 Eric Hyer - Removed mouseButtonPress method.
   *   @history 2011-09-28 Steven Lambright - Removed p_userCursor. It was only
   *                           causing bugs. As a result, the mouseButtonRelease
   *                           method did nothing so I removed that too. Zooming
   *                           happens on the rubberBandComplete slot.
   *  @history 2017-05-10 Ian Humphrey - Modified zoomFit() so that when doing a "Fit in viewport"
   *                          zoom, the image is centered properly in the viewport. Fixes #4756. 
   */
  class ZoomTool : public Tool {
      Q_OBJECT

    public:
      ZoomTool(QWidget *parent);
      void addTo(QMenu *menu);

      //! Returns the name of the menu.
      QString menuName() const {
        return "&View";
      }

    protected slots:
      void rubberBandComplete();

    protected:
      QAction *toolPadAction(ToolPad *toolpad);
      void updateTool();
      QWidget *createToolBarWidget(QStackedWidget *parent);
      void enableRubberBandTool();

    private slots:
      void zoomIn2X();
      void zoomIn4X();
      void zoomIn8X();

      void zoomOut2X();
      void zoomOut4X();
      void zoomOut8X();

      void zoomActual();
      void zoomFit();
      void zoomFitWidth();
      void zoomFitHeight();
      void zoomManual();


      double setScale(MdiCubeViewport *d, double newScale);
      double setScale(MdiCubeViewport *d, double newScale, int x, int y);
      double setScale(MdiCubeViewport *d, double newScale, double samp, double line);

    private:
      void zoomBy(double factor);

      QAction *p_zoomIn2X; //!< Zoom in 2 times.
      QAction *p_zoomIn4X; //!< Zoom in 4 times.
      QAction *p_zoomIn8X; //!< Zoom in 8 times.

      QAction *p_zoomOut2X; //!< Zoom out 2 times.
      QAction *p_zoomOut4X; //!< Zoom out 4 times.
      QAction *p_zoomOut8X; //!< Zoom out 8 times.

      QAction *p_zoomActual;  //!< Zoom to actual size action.
      QAction *p_zoomFit; //!< Fit the cube in the viewport action.

      QLineEdit *p_zoomLineEdit; //!< Line edit for manual zoom factor.
      double p_lastScale;
  };
};

#endif
