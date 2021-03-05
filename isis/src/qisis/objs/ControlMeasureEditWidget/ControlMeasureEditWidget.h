#ifndef ControlMeasureEditWidget_h
#define ControlMeasureEditWidget_h

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
class QListWidget;
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
  class ControlPoint;
  class Cube;
  class CubeViewport;
  class SerialNumberList;
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
    *                          constructor to allow mouse events
    *                          on leftChipViewport.
    *   @history 2008-11-24 Tracie Sucharski - Changed class name
    *                          from PointEdit
    *   @history 2008-12-02 Jeannie Walldren - Modified
    *                          createPointEditor() to allow the
    *                          leftChipViewport to refresh even if
    *                          allowLeftMouse = false.
    *   @history 2008-12-10 Jeannie Walldren - Added a private
    *                          variable for the template filename.
    *                          Set the default value of this
    *                          variable to the previously
    *                          hard-coded template filename. Added
    *                          setTemplateFile() method derived
    *                          from previously unused and
    *                          commented out method called
    *                          openTemplateFile().
    *   @history 2008-12-15 Jeannie Walldren - Some QMessageBox
    *                          warnings had strings tacked on to
    *                          the list of errors.  These strings
    *                          were changed to iExceptions and
    *                          added to the error stack to conform
    *                          with Isis standards.
    *   @history 2008-12-30 Jeannie Walldren - Modified
    *                          savePoint() method to update user
    *                          (chooser) name and date when point
    *                          is saved
    *   @history 2009-03-17 Tracie Sucharski - Add slot to set a
    *                          boolean to indicate whether the
    *                          registration chips should be saved.
    *   @history 2010-06-08 Jeannie Walldren - Added warning box
    *                          in registerPoint() if unable to
    *                          load left or right chip
    *   @history 2010-06-26 Eric Hyer - now uses MdiCubeViewport instead of
    *                          CubeViewport.  Fixed multiple include problems.
    *   @history 2010-11-17 Eric Hyer - now forwards new ControlNets to the
    *                          ChipViewports
    *   @history 2010-11-19 Tracie Sucharski - Renamed the "Save Point" button
    *                           to "Save Measure" along with signals and slots.
    *                           Add a new "Save Point" button which actually
    *                           saves the edit point to the network.
    *   @history 2010-11-22 Eric Hyer - Forwarded SIGNAL from StretchTool to
    *                          ChipViewports
    *   @history 2010-12-01 Eric Hyer - Added checkboxes for stretch locking
    *   @history 2010-12-08 Eric Hyer - Relocated stretch locking checkboxes
    *   @history 2010-12-14 Eric Hyer - setTemplateFile now takes the filename
    *                          as a parameter (no more open dialog!)
    *   @history 2011-05-04 Jai Rideout - updated saveChips() to reference new
    *                          AutoReg API for accessing chips used in
    *                          registration.
    *   @history 2011-06-14 Tracie Sucharski - Added slot to colorize the
    *                          Save Measure button.  If user moved the tack
    *                          point, the button text will be changed to red.
    *                          If they push the save button, button will go back
    *                          to black.
    *   @history 2011-06-15 Tracie Sucharski - Changed signal mouseClick
    *                          to userMovedTackPoint.  TODO:  Could not use
    *                          tackPointChanged signal because that signal is
    *                          emitted whenever the measure is loaded not just
    *                          when the user initiates the move.  This should be
    *                          cleaned up.
    *   @history 2012-04-17  Tracie Sucharski - If geom is turned on update the
    *                          right measure in ::setLeftMeasure.
    *   @history 2012-05-01  Tracie Sucharski - Fix intereface between No Geom, geom
    *                          and rotate.  Make sure resets are done properly when
    *                          switching beween the options.
    *   @history 2012-05-07  Tracie Sucharski - Change made on 2012-04-17 introduced bug when
    *                          loading a different control point, so in setLeftMeasure, only
    *                          update the right chip if we're not loading a
    *                          different control point.
    *   @history 2012-06-28  Tracie Sucharski - Add parameter to constructor to indicate
    *                          whether to use cube geometry.  This allow cubes with no camera
    *                          or SPICE information to be used.
    *   @history 2012-07-20  Tracie Sucharski - Change the Save Measure button text and tootip
    *                          depending on whether movement is allowed on the left chip.
    *   @history 2012-07-26 Tracie Sucharski - Added ability to link zooming between left and
    *                          right viewports. Added 2 methods, zoomFactor() which returns a
    *                          zoom factor and zoom(double zoomFactor).
    *   @history 2013-04-30 Tracie Sucharski - Fixed bug introduced by linking zooms between left
    *                          and right viewports.  Zoom factors were being passed into the
    *                          Chip::Load method as the second argument which should be the rotation
    *                          value.
    *   @history 2013-11-07 Tracie Sucharski - Moved error checking on edit locked measures from
    *                          QnetTool::measureSaved to ::saveMeasure.  The error checking now
    *                          forces the edit lock check box to be unchecked before the measure
    *                          can be saved.  Fixes #1624.
    *   @history 2013-12-30 Kimberly Oyama and Stuart Sides - In saveChips(), added single quotes
    *                           around the file names in case there are spaces or other special
    *                           characters in them. Fixes #1551.
    *   @history 2014-07-03 Tracie Sucharski - Renamed from ControlPointEdit for IPCE.
    *   @history 2016-04-28 Tracie Sucharski - Removed position information, this will be shown
    *                           in ControlNetEditor type view.  Added blink capability.
    *   @history 2016-06-27 Ian Humphrey - Updated documentation and coding standards. Fixes #4004.
    *   @history 2017-07-27  Christopher Combs - Added sample, line, lat, and lon labels.
    *                           Fixes #5067.
    *   @history 2017-08-11 Tracie Sucharski - Added shortcuts for find,
    *                           registration/undo-registration, save measure. Fixes #4982.
    *   @history 2017-08-11 Tracie Sucharski - Created a new ControlMeasure when editing points so
    *                           that the edit ControlPoint is no changed until user selects
    *                           "Save Measures", and colorize save buttons.  Fixes #4984.
    *   @history 2018-06-28 Kaitlyn Lee - Removed shortcuts from zoom buttons because of ambiguous
    *                           shortcut errors. Set the shortcut and tooltip of m_autoReg inside of
    *                           registerPoint() to allow the user to use the shortcut after an
    *                           undo-registration ocurs.
    *   @history 2018-09-06 Tracie Sucharski - Added bug fixes from qnet's ControlPointEdit class
    *                           including moving the creation of AutoRegFactory from constructor
    *                           to the registerPoint method and fixing seg fault happening in
    *                           saveMeasure when calling ControlMeasure::SetLogData.
    *   @history 2018-09-24 Tracie Sucharski - Fixed right measure chooser name to the
    *                           Application::User.
    *   @history 2018-09-26 Tracie Sucharski - Added public method to allow change measure tack
    *                           points.
    *   @history 2018-10-10 Tracie Sucharski - Fixed blink extension to use geom if selected and
    *                           correct zoom factor.
    *
    *   @todo  Re-think design of the change made on 2012-07-26.  The linking was put into
    *                          ::updateLeftPositionLabel because it was the fastest solution, but
    *                          should this be put somewhere else.
    */
  class ControlMeasureEditWidget : public QWidget {
      Q_OBJECT

    public:
      ControlMeasureEditWidget(QWidget *parent = 0, bool allowLeftMouse = false,
                               bool useGeometry = true);
      ~ControlMeasureEditWidget();
      /**
       * Returns the template filename used for auto-registration.
       *
       * @return @b QString Template filename
       */
      QString templateFileName() {
        return m_templateFileName;
      };
      void allowLeftMouse(bool allowMouse);

      void setLeftPosition(double sample, double line);
      void setRightPosition(double sample, double line);

    signals:
      void updateLeftView(double sample, double line);
      void updateRightView(double sample, double line);
      void measureSaved();
      void newControlNetwork(ControlNet *);
      void setTemplateFailed(QString);
      void stretchChipViewport(Stretch *, CubeViewport *);

    public slots:
      bool setTemplateFile(QString);
      void setPoint(ControlPoint *editPoint, SerialNumberList *snList);
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

      void showBlinkExtension();
      void blinkStartRight();
      void blinkStopRight();
      void changeBlinkTimeRight(double interval);
      void updateBlinkRight();


    private:
      void createMeasureEditor(QWidget *parent);

      bool m_allowLeftMouse; //!< Whether or not to allow mouse events on left chip viewport
      bool m_useGeometry;    //!< Whether or not to allow geometry/rotation on right chip viewport

      QString m_templateFileName;      //!< Registration template filename
      QLabel *m_leftZoomFactor;        //!< Label for left chip viewport's zoom factor
      QLabel *m_rightZoomFactor;       //!< Label for right chip viewport's zoom factor
      QLabel *m_leftSampLinePosition;  //!< Label for left chip viewport's current sample/line
      QLabel *m_rightSampLinePosition; //!< Label for right chip viewport's current sample/line
      QLabel *m_leftLatLonPosition;    //!< Label for left chip viewport's current lat/lon
      QLabel *m_rightLatLonPosition;   //!< Label for right chip viewport's current lat/lon
      QRadioButton *m_nogeom; //!< Radio button to remove geometry/rotation for right chip viewport
      QRadioButton *m_geom; //!< Radio button to apply geometry/rotation to right chip viewport
      QToolButton *m_rightZoomIn;      //!< Button for zooming in right chip viewport
      QToolButton *m_rightZoomOut;     //!< Button for zooming out right chip viewport
      QToolButton *m_rightZoom1;       //!< Button for 1:1 zoom on right chip viewport

      bool m_timerOn;  //!< Indicates if the blink timer is on
      QTimer *m_timer; //!< Timer on the blinking
      QList<ChipViewport *> m_blinkList; //!< List of chip viewports to blink
      unsigned char m_blinkIndex; //!< Index of the chip to load in the left chip viewport

      QDial *m_dial;                  //!< Rotation dial
      QLCDNumber *m_dialNumber;       //!< The current amount of rotation (in degrees)
      QDoubleSpinBox *m_blinkTimeBox; //!< The current blink step (in seconds)

      bool m_circle;        //!< Whether or not to draw circle in center of the right chip viewport
      QScrollBar *m_slider; //!< Slider that controls the size of the center circle

      QPushButton *m_autoReg;      //!< Button to auto-register the measure
      QWidget *m_autoRegExtension; //!< Widget that shows after registering a measure
      QLabel *m_oldPosition;       //!< The old sample and line before registering
      QLabel *m_goodFit;           //!< The goodness of fit value after registering
      bool m_autoRegShown;         //!< Whether or not the auto-reg extension is shown
      bool m_autoRegAttempted;     //!< Whether or not auto-registration has been attempted

      QPushButton *m_saveMeasure;    //!< Button to save the current measure
      QPalette m_saveDefaultPalette; //!< Default color palette for the Save button

      ChipViewport *m_leftView;  //!< Left ChipViewport
      ChipViewport *m_rightView; //!< Right ChipViewport

      Cube *m_leftCube;  //!< Left chip viewport's Cube
      Cube *m_rightCube; //!< Right chip viewport's Cube
      ControlMeasure *m_leftMeasure;  //!< Left ControlMeasure
      ControlMeasure *m_rightMeasure; //!< Right ControlMeasure
      Chip *m_leftChip;  //!< Left Chip
      Chip *m_rightChip; //!< Right Chip
      UniversalGroundMap *m_leftGroundMap;  //!< UniversalGroundMap for left cube
      UniversalGroundMap *m_rightGroundMap; //!< UniversalGroundMap for right cube

      AutoReg *m_autoRegFact; //!< Created AutoReg
      QString m_pointId;      //!< Associated control point id of the right measure

      int m_rotation;  //!< Amount to rotate right chip viewport TODO Is this used??
      bool m_geomIt;   //!< Apply geometry to the right chip viewport
      bool m_linkZoom; //!< Link zoom factors between chip viewports



      ControlPoint *m_editPoint;            //!< The control point currently being edited
      SerialNumberList *m_serialNumberList; //!< The serial numbers for each measure of m_editpoint
      QWidget *m_blinkExtension;  //!< Widget for selecting images and timing to blink through them
      QListWidget *m_blinkListWidget;       //!< List of images being blinked through


      QDoubleSpinBox *m_blinkTimeBoxRight;  //!< Input for time between image blinks
      bool m_timerOnRight;                  //!< Timer is on for right viewport
      QTimer *m_timerRight;                 //!< Timer for tracking image blink time
      QList<ChipViewport *> m_blinkChipViewportListRight;  //!< List of viewports to blink through
      unsigned char m_blinkIndexRight;                     //!< Index of image being blinked

  };
};

#endif
