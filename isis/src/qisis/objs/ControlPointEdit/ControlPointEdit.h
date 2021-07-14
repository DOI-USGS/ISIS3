#ifndef ControlPointEdit_h
#define ControlPointEdit_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// This is the only include allowed in this file!
#include <QWidget>


class QCheckBox;
class QDial;
class QDoubleSpinBox;
class QHBoxLayout;
class QLabel;
class QLCDNumber;
class QPalette;
class QPushButton;
class QRadioButton;
class QScrollBar;
class QString;
class QTimer;
class QToolButton;
class QVBoxLayout;

namespace Isis {
  class AutoReg;
  class Chip;
  class ChipViewport;
  class ControlMeasure;
  class ControlNet;
  class Cube;
  class CubeViewport;
  class Stretch;
  class Tool;
  class UniversalGroundMap;

  /**
    * @brief Point Editor Widget
    *
    * @ingroup Visualization Tools
    *
    * @author 2008-09-09 Tracie Sucharski
    *
    * @internal
    *   @history 2008-11-19 Tracie Sucharski - Addition option to
    *                           constructor to allow mouse events
    *                           on leftChipViewport.
    *   @history 2008-11-24 Tracie Sucharski - Changed class name
    *                           from PointEdit
    *   @history 2008-12-02 Jeannie Walldren - Modified
    *                           createPointEditor() to allow the
    *                           leftChipViewport to refresh even if
    *                           allowLeftMouse = false.
    *   @history 2008-12-10 Jeannie Walldren - Added a private
    *                           variable for the template filename.
    *                           Set the default value of this
    *                           variable to the previously
    *                           hard-coded template filename. Added
    *                           setTemplateFile() method derived
    *                           from previously unused and
    *                           commented out method called
    *                           openTemplateFile().
    *   @history 2008-12-15 Jeannie Walldren - Some QMessageBox
    *                           warnings had strings tacked on to
    *                           the list of errors.  These strings
    *                           were changed to iExceptions and
    *                           added to the error stack to conform
    *                           with Isis standards.
    *   @history 2008-12-30 Jeannie Walldren - Modified
    *                           savePoint() method to update user
    *                           (chooser) name and date when point
    *                           is saved
    *   @history 2009-03-17 Tracie Sucharski - Add slot to set a
    *                           boolean to indicate whether the
    *                           registration chips should be saved.
    *   @history 2010-06-08 Jeannie Walldren - Added warning box
    *                           in registerPoint() if unable to
    *                           load left or right chip
    *   @history 2010-06-26 Eric Hyer - now uses MdiCubeViewport instead of
    *                           CubeViewport.  Fixed multiple include problems.
    *   @history 2010-11-17 Eric Hyer - now forwards new ControlNets to the
    *                           ChipViewports
    *   @history 2010-11-19 Tracie Sucharski - Renamed the "Save Point" button
    *                           to "Save Measure" along with signals and slots.
    *                           Add a new "Save Point" button which actually
    *                           saves the edit point to the network.
    *   @history 2010-11-22 Eric Hyer - Forwarded SIGNAL from StretchTool to
    *                           ChipViewports
    *   @history 2010-12-01 Eric Hyer - Added checkboxes for stretch locking
    *   @history 2010-12-08 Eric Hyer - Relocated stretch locking checkboxes
    *   @history 2010-12-14 Eric Hyer - setTemplateFile now takes the filename
    *                           as a parameter (no more open dialog!)
    *   @history 2011-05-04 Jai Rideout - updated saveChips() to reference new
    *                           AutoReg API for accessing chips used in
    *                           registration.
    *   @history 2011-06-14 Tracie Sucharski - Added slot to colorize the
    *                           Save Measure button.  If user moved the tack
    *                           point, the button text will be changed to red.
    *                           If they push the save button, button will go back
    *                           to black.
    *   @history 2011-06-15 Tracie Sucharski - Changed signal mouseClick
    *                           to userMovedTackPoint.  TODO:  Could not use
    *                           tackPointChanged signal because that signal is
    *                           emitted whenever the measure is loaded not just
    *                           when the user initiates the move.  This should be
    *                           cleaned up.
    *   @history 2012-04-17 Tracie Sucharski - If geom is turned on update the
    *                           right measure in ::setLeftMeasure.
    *   @history 2012-05-01 Tracie Sucharski - Fix intereface between No Geom, geom
    *                           and rotate.  Make sure resets are done properly when
    *                           switching beween the options.
    *   @history 2012-05-07 Tracie Sucharski - Change made on 2012-04-17 introduced bug when
    *                           loading a different control point, so in setLeftMeasure, only
    *                           update the right chip if we're not loading a
    *                           different control point.
    *   @history 2012-06-28 Tracie Sucharski - Add parameter to constructor to indicate
    *                           whether to use cube geometry.  This allow cubes with no camera
    *                           or SPICE information to be used.
    *   @history 2012-07-20 Tracie Sucharski - Change the Save Measure button text and tootip
    *                           depending on whether movement is allowed on the left chip.
    *   @history 2012-07-26 Tracie Sucharski - Added ability to link zooming between left and
    *                           right viewports. Added 2 methods, zoomFactor() which returns a
    *                           zoom factor and zoom(double zoomFactor).
    *   @history 2013-04-30 Tracie Sucharski - Fixed bug introduced by linking zooms between left
    *                           and right viewports.  Zoom factors were being passed into the
    *                           Chip::Load method as the second argument which should be the rotation
    *                           value.
    *   @history 2013-11-07 Tracie Sucharski - Moved error checking on edit locked measures from
    *                           QnetTool::measureSaved to ::saveMeasure.  The error checking now
    *                           forces the edit lock check box to be unchecked before the measure
    *                           can be saved.  Fixes #1624.
    *   @history 2013-12-30 Kimberly Oyama and Stuart Sides - In saveChips(), added single quotes
    *                           around the file names in case there are spaces or other special
    *                           characters in them. Fixes #1551.
    *   @history 2015-01-13 Ian Humphrey - Modified setTemplateFile() so opening a template file
    *                           will undo registration if a point is already registered. Modified
    *                           saveMeasure() to handle exception thrown by
    *                           ControlMeasure::SetLogData(). Fixes #2041.
    *   @history 2015-10-29 Ian Humphrey - Added shortcuts for Register (R), Save Measure (M),
    *                           Undo Registration (U), and Find (F). Fixes #2324.
    *   @history 2017-04-25 Marjorie Hahn - Moved AutoRegFactory creation from the constructor
    *                           to ControlPointEdit::registerPoint() so that AutoRegFactory is
    *                           not created until it is needed. Fixes #4590.
    *
    *   @todo  Re-think design of the change made on 2012-07-26.  The linking was put into
    *                          ::updateLeftPositionLabel because it was the fastest solution, but
    *                          should this be put somewhere else.
    */
  class ControlPointEdit : public QWidget {
      Q_OBJECT

    public:
      ControlPointEdit(ControlNet * cnet, QWidget *parent = 0,
                       bool allowLeftMouse = false, bool useGeometry = true);
      ~ControlPointEdit();
      QString templateFileName() {
        return p_templateFileName;
      };
      bool setTemplateFile(QString);
      void allowLeftMouse(bool allowMouse);

    signals:
      void updateLeftView(double sample, double line);
      void updateRightView(double sample, double line);
      void measureSaved();
      void newControlNetwork(ControlNet *);
      void stretchChipViewport(Stretch *, CubeViewport *);

    public slots:
      void setLeftMeasure(ControlMeasure *leftMeasure,
                          Cube *leftCube, QString pointId);
      void setRightMeasure(ControlMeasure *rightMeasure,
                           Cube *rightCube, QString pointId);
      void colorizeSaveButton();
      void refreshChips();
      void saveChips();

    protected slots:

    private slots:

      void setNoGeom();
      void setGeom();
      void setRotate();
      void setCircle(bool);
      void setZoomLink(bool);

      void findPoint();
      void registerPoint();
      void saveMeasure();
      void updateLeftPositionLabel(double zoomFactor);
      void updateRightGeom();
      void updateRightPositionLabel(double zoomFactor);

      void blinkStart();
      void blinkStop();
      void changeBlinkTime(double interval);
      void updateBlink();

    private:
      void createPointEditor(QWidget *parent);

      bool p_allowLeftMouse;
      bool p_useGeometry;

      QString p_templateFileName;
      QLabel *p_leftZoomFactor;
      QLabel *p_rightZoomFactor;
      QLabel *p_leftSampLinePosition;
      QLabel *p_rightSampLinePosition;
      QLabel *p_leftLatLonPosition;
      QLabel *p_rightLatLonPosition;
      QRadioButton *p_nogeom;
      QRadioButton *p_geom;
      QToolButton *p_rightZoomIn;
      QToolButton *p_rightZoomOut;
      QToolButton *p_rightZoom1;


      bool p_timerOn;
      QTimer *p_timer;
      std::vector<ChipViewport *> p_blinkList;
      unsigned char p_blinkIndex;

      QDial *p_dial;
      QLCDNumber *p_dialNumber;
      QDoubleSpinBox *p_blinkTimeBox;

      bool p_circle;
      QScrollBar *p_slider;

      QPushButton *p_autoReg;
      QWidget *p_autoRegExtension;
      QLabel *p_oldPosition;
      QLabel *p_goodFit;
      bool   p_autoRegShown;
      bool   p_autoRegAttempted;

      QPushButton *p_saveMeasure;
      QPalette p_saveDefaultPalette;

      ChipViewport *p_leftView;
      ChipViewport *p_rightView;

      Cube *p_leftCube;
      Cube *p_rightCube;
      ControlMeasure *p_leftMeasure;
      ControlMeasure *p_rightMeasure;
      Chip *p_leftChip;
      Chip *p_rightChip;
      UniversalGroundMap *p_leftGroundMap;
      UniversalGroundMap *p_rightGroundMap;

      AutoReg *p_autoRegFact;
      QString p_pointId;

      int p_rotation;
      bool p_geomIt;
      bool p_linkZoom;
  };
};

#endif
