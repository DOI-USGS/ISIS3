#ifndef ControlPointEditWidget_h
#define ControlPointEditWidget_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include "ControlPoint.h"
#include "FileName.h"
#include "SpecialPixel.h"
#include "TemplateList.h"

#include <QCloseEvent>
#include <QDir>
#include <QHideEvent>
#include <QPalette>
#include <QPointer>
#include <QStatusBar>
#include <QString>
#include <QStringList>
#include <QWidget>

class QAction;
class QBoxLayout;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QMainWindow;
class QObject;
class QPainter;
class QPoint;
class QPushButton;
class QSplitter;
class QStackedWidget;
class QStandardItemModel;
class QString;
class QTableWidget;
class QTextEdit;
class QWidget;

namespace Isis {
  class Control;
  class ControlMeasureEditWidget;
  class ControlMeasure;
  class ControlNet;
  class Cube;
  class CubeViewport;
  class Directory;
  class MdiCubeViewport;
  class SerialNumberList;
  class Stretch;
  class ToolPad;
  class UniversalGroundMap;

  /**
   * @brief Gui for editing ControlPoints in ipce application
   *
   * @ingroup Visualization Tools
   *
   * @author 2014-06-25 Tracie Sucharski
   *
   * @internal
   *   @history 2016-06-27 Ian Humphrey - Updated documentation and coding standards. Fixes #4004.
   *   @history 2016-09-30 Tracie Sucharski - Pass in directory to constructor, so that we can
   *                           query for shapes and other data from the project.
   *   @history 2017-01-05 Tracie Sucharski - Allow a new ground source location to be entered which
   *                           includes allowing the option to change the location in the active
   *                           control for all ground points.
   *   @history 2017-05-18 Tracie Sucharski - Added serialNumber to the setEditPoint slot.
   *   @history 2017-08-02 Tracie Sucharski - Added methods to return the current editPoint and
   *                           current editPoint Id.  Removed measure table methods. Fixes #5007,
   *                           #5008.
   *   @history 2017-08-09 Adam Goins - Changed method references of SerialNumberList.Delete() to
   *                           SerialNumberList.remove()
   *   @history 2017-08-09 Christopher Combs - Added QPushButton and slot for reloading a point's
   *                           measures in the ChipViewports. Fixes #5070.
   *   @history 2017-08-09 Christopher Combs - Added Apriori Latitude, Longitude, and Radius to
   *                           the dialog. Fixes #5066.
   *   @history 2017-08-11 Tracie Sucharski - Added shortcuts for Save Point, Save Net.
   *                           Fixes #4982.
   *   @history 2017-08-11 Tracie Sucharski - Fixed save point and colorization of buttons.
   *                           Fixes #4984.
   *   @history 2017-08-15 Tracie Sucharski - When ControlPoint is deleted, set the visibility of
   *                           this widget to false, then to true in loadPoint().  Fixes #5073.
   *   @history 2018-03-23 Tracie Sucharski - Update the cnet filename with current cnet when it is
   *                           changed.
   *   @history 2018-03-26 Tracie Sucharski - Added slot, setControlFromActive which update editor
   *                           if a new active control net is set in ipce. References #4567.
   *   @history 2018-03-30 Tracie Sucharski - Save Control in addition to the control net and use
   *                           Control to write the control net so Control can keep track of the
   *                           modification state of the control net.
   *   @history 2018-04-25 Tracie Sucharski - Fix bug when creating a control point from CubeDnView
   *                           or FootprintView if a ground source exists in the serial number list.
   *                           Fixes #5399.
   *   @history 2018-05-02 Tracie Sucharski - Colorize save buttons properly when creating new
   *                           control point and loading a different control point.
   *   @history 2018-06-11 Summer Stapleton - Stripped path from displayed filename of Control
   *                           Network and set the tooltip to the full path for easier access.
   *   @history 2018-06-19 Adam Goins - Fixed updating references in selectLeftMeasure and
   *                           selectRightMeasure to fix a segfault that was occuring. #Fixes #5435
   *   @history 2018-06-28 Kaitlyn Lee - Removed shortcut from reload point button.
   *   @history 2018-07-07 Summer Stapleton - Added a QComboBox to the widget to allow for changing
   *                           the active registration template from the widget itself.
   *   @history 2018-07-13 Kaitlyn Lee - Added calls to setModified(true) when a cnet is modified.
   *                           References #5396.
   *   @history 2018-08-08 Tracie Sucharski - Removed temporary autosave of active control, most
   *                           likely causing problems with large networks.
   *   @history 2018-10-04 Tracie Sucharski - Changed functionality of ground and radius source
   *                           choices on constrained and fixed points.  Fixes #5504.
   *   @history 2018-10-04 Tracie Sucharski - Removed calls to ControlNet::GetTargetRadii because of
   *                           changes to ControlNet.
   *   @history 2018-10-05 Tracie Sucharski - Added radius source combo to the NewControlPoint
   *                           dialog.
   */
  class ControlPointEditWidget : public QWidget {
    Q_OBJECT

    public:
      ControlPointEditWidget(Directory *directory, QWidget *parent, bool addMeasures = false);
      virtual ~ControlPointEditWidget();

      QString editPointId();
      ControlPoint *editPoint();

    signals:
      void controlPointChanged(QString pointId);
      void controlPointAdded(QString pointId);
      void ignorePointChanged();
      void ignoreLeftChanged();
      void ignoreRightChanged();
      void cnetModified();
      void newControlNetwork(ControlNet *);
      void stretchChipViewport(Stretch *, CubeViewport *);
      void measureChanged();
      // temporary signal for quick & dirty autosave in Ipce
      void saveControlNet();

    public slots:
      void setSerialNumberList(SerialNumberList *snList);
      void setControl(Control *control);
      void setControlFromActive();
      void setEditPoint(ControlPoint *controlPoint, QString serialNumber = "");
      void deletePoint(ControlPoint *controlPoint);

      void createControlPoint(double latitude, double longitude, Cube *cube = 0,
                              bool isGroundSource = false);

      // Changed colorizeSaveNetButton to public slot so it could be called from
      // Directory::saveActiveControl().  This should be temporary until the modify/save functionality
      // of active control is re-factored. Also added reset parameter, defaulting to false so button
      // is red. This default was used so that current calls did not need to be changed.
      void colorizeSaveNetButton(bool reset = false);

      void addTemplates(TemplateList *templateList);

    protected:
      bool eventFilter(QObject *o,QEvent *e);

    private slots:
//    void enterWhatsThisMode();
      void reloadPoint();
      void saveNet();
//    void addMeasure();
      void setPointType (int pointType);
      void setLockPoint (bool ignore);
      void setIgnorePoint (bool ignore);
      void setLockLeftMeasure (bool ignore);
      void setIgnoreLeftMeasure (bool ignore);
      void setLockRightMeasure (bool ignore);
      void setIgnoreRightMeasure (bool ignore);

      void selectLeftMeasure (int index);
      void selectRightMeasure (int index);
      void nextRightMeasure();
      void previousRightMeasure();
      void updateLeftMeasureInfo ();
      void updateRightMeasureInfo ();

      void measureSaved();
      void checkReference();

      void updateGroundPosition();
      void updateSurfacePointInfo ();
      void openReferenceRadius();
      void groundSourceFileSelectionChanged(int index);

      void savePoint();

      void colorizeAllSaveButtons(QString color);
      void colorizeSavePointButton();

      void openTemplateFile();
      void viewTemplateFile();
      void saveChips();
      void showHideTemplateEditor();
      void saveTemplateFile();
      void saveTemplateFileAs();
      void setTemplateFile(QString);
      void setTemplateModified();
      void writeTemplateFile(QString);
      void resetTemplateComboBox(QString fileName);
      void clearEditPoint();

    private:
      void createActions();

      void loadPoint(QString serialNumber = "");
      void loadGroundMeasure();
      void createPointEditor(QWidget *parent, bool addMeasures);
      QSplitter * createTopSplitter();
      QGroupBox * createControlPointGroupBox();
      QGroupBox * createLeftMeasureGroupBox();
      QGroupBox * createRightMeasureGroupBox();
      void createTemplateEditorWidget();
      void loadTemplateFile(QString);
      bool okToContinue();
      bool IsMeasureLocked(QString serialNumber);
      bool validateMeasureChange(ControlMeasure *m);

      void setShapesForPoint(double latitude=Null, double longitude=Null);
      ControlMeasure *createTemporaryGroundMeasure();
      bool setGroundSourceInfo();
      FileName checkGroundFileLocation(FileName groundFile);
      void changeGroundLocationsInNet();
      void clearGroundSource();

      void initDem(QString demFile);
      double demRadius(double latitude, double longitude);

    private:

      QPointer<QWidget> m_parent; //!< Parent widget
      Directory *m_directory;
      bool m_addMeasuresButton;   //!< Indicates whether or not to add "Add Measures(s) to Point"

      QString m_cnetFileName; //!< Filename of the control network that is being modified
      QPointer<QLabel> m_cnetFileNameLabel; //!< Label with name of the control network file
      bool m_cnetModified; //!< Indicates if the control network has been modified

      QPointer<QAction> m_closePointEditor; //!< Action to close the point editor

      QPointer<QAction> m_saveChips; //!< Action to save the registration chips
      //! Action to toggle visibility of the registration template editor
      QPointer<QAction> m_showHideTemplateEditor;
      QPointer<QAction> m_openTemplateFile; //!< Action to open a registration template file to disk
      QPointer<QAction> m_saveTemplateFile; //!< Action to save a registration template file to disk
      QPointer<QAction> m_saveTemplateFileAs; //!< Action to save a new registration template


      //! Pointer to control measure editor widget
      QPointer<ControlMeasureEditWidget> m_measureEditor;

      QPointer<QPushButton> m_reloadPoint; //!< Button to reload current point to saved measures
      QPointer<QPushButton> m_savePoint; //!< Button to save current point being edited
      QPalette m_saveDefaultPalette; //!< Default color pallet of the "Save Point" button

      QPointer<QPushButton> m_saveNet; //!< Button to save the current control network

      QPointer<QTextEdit> m_templateEditor; //!< Text editor for editing the registration template
      QPointer<QWidget> m_templateEditorWidget; //!< Template editor widget
      bool m_templateModified; //!< Indicates if the registration template was edited

      QPointer<QComboBox> m_templateComboBox; //!< ComboBox of imported registration templates
      QPointer<QComboBox> m_groundSourceCombo; //!< ComboBox for selecting ground source
      QPointer<QComboBox> m_radiusSourceCombo; //!< ComboBox for selecting ground source
      QPointer<QLabel> m_ptIdValue; //!< Label for the point id of the current point
      QPointer<QComboBox> m_pointTypeCombo; //!< Combobox to change the type of the current point
      QPointer<QLabel> m_numMeasures;
      QPointer<QLabel> m_aprioriLatitude;
      QPointer<QLabel> m_aprioriLongitude;
      QPointer<QLabel> m_aprioriRadius;

      QPointer<QCheckBox> m_lockPoint; //!< Checkbox that locks/unlocks the current point
      QPointer<QCheckBox> m_ignorePoint; //!< Checkbox to ignore the current point
      QPointer<QLabel> m_leftReference; //!< Label indicating if left measure is the reference
      QPointer<QLabel> m_leftMeasureType; //!< Label for the left measure's adjustment type
      QPointer<QLabel> m_rightReference; //!< Label indicating if right measure is the reference
      QPointer<QLabel> m_rightMeasureType; //!< Label for the right measure's adjustment type
      QPointer<QCheckBox> m_lockLeftMeasure; //!< Checkbox to edit lock/unlock the left measure
      QPointer<QCheckBox> m_ignoreLeftMeasure; //!< Checkbox to ignore the left measure
      QPointer<QCheckBox> m_lockRightMeasure; //!< Checkbox to edit lock/unlock the right measure
      QPointer<QCheckBox> m_ignoreRightMeasure; //!< Checkbox to ignore the right measure

      QPointer<QComboBox> m_leftCombo; //!< Combobox to load left measure into left chip viewport
      QPointer<QComboBox> m_rightCombo; //!< Combobox to load right measure into right chip viewport
      QPointer<QStandardItemModel> m_model;

      QPointer<QMainWindow> m_measureWindow; //!< Main window for the the measure table widget
      QPointer<QTableWidget> m_measureTable; //!< Table widget for the measures

      QPointer<ControlPoint> m_editPoint;   //!< The control point being edited
      SerialNumberList *m_serialNumberList; //!< Serial number list for the loaded cubes
      QPointer<ControlNet> m_controlNet;    //!< Current control net
      QPointer<Control> m_control;          //!< Current Control

      QPointer<ControlPoint> m_newPoint; //!< New control point
      QString m_lastUsedPointId; //!< Point id of the last used control point

      QStringList m_pointFiles; //!< Associated files for current control point

      QString m_leftFile; //!< Filename of left measure
      QPointer<ControlMeasure> m_leftMeasure; //!< Left control measure
      QPointer<ControlMeasure> m_rightMeasure; //!< Right control measure
      QScopedPointer<Cube> m_leftCube; //!< Left cube
      QScopedPointer<Cube> m_rightCube; //!< Right cube

      QStringList m_projectShapeNames; //!< List of Shapes imported into project, at time of loaded CP
      int m_numberProjectShapesWithPoint; //!< Number of shapes containing control point
      QMap<QString, Shape *> m_nameToShapeMap; //!< Map between Shape display name and object

      QString m_groundFilename; //!< File name of ground source
      QString m_groundSN; //!< Serial number of ground source file
      ControlPoint::SurfacePointSource::Source m_groundSourceType; //!< SurfacePoint type of ground source
      QScopedPointer<UniversalGroundMap> m_groundGmap;

      bool m_changeAllGroundLocation; //!< Change the ground source location of all fixed,
                                      //!  constrained points in the network
      bool m_changeGroundLocationInNet; //!< Change the ground source location
      QString m_newGroundDir; //!< Contains the ground source location

      //  TODO:  Combine the following m_groundSourceFile, m_radiusSourceFile
      //           with m_groundFile and m_demFile.  Is it just a matter of
      //           full path vs filename only?
      QString m_radiusFilename;
      ControlPoint::RadiusSource::Source m_radiusSourceType;
      bool m_demOpen; //!< Has a radius source been opened?
      QString m_demFile;
      QScopedPointer<Cube> m_demCube;
  };
};
#endif
