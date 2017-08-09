#ifndef ControlPointEditWidget_h
#define ControlPointEditWidget_h


#include "ControlPoint.h"
#include "FileName.h"

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
   * @brief Gui for editing ControlPoint
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
   *   @history 2017-08-09 Adam Goins - Changed method references of SerialNumberList.Delete() to
   *                           SerialNumberList.remove()
   */
  class ControlPointEditWidget : public QWidget {
    Q_OBJECT

    public:
      ControlPointEditWidget(Directory *directory, QWidget *parent, bool addMeasures = false);
      virtual ~ControlPointEditWidget();

      //! Measure column values for the measure table
      enum MeasureColumns{
        FILENAME,
        CUBESN,
        SAMPLE,
        LINE,
        APRIORISAMPLE,
        APRIORILINE,
        SAMPLERESIDUAL,
        LINERESIDUAL,
        RESIDUALMAGNITUDE,
        SAMPLESHIFT,
        LINESHIFT,
        PIXELSHIFT,
        GOODNESSOFFIT,
        IGNORED,
        EDITLOCK,
        TYPE
      };
      //! Number of entries (columns) in the measure table
      static const int NUMCOLUMNS = 16;

      QString measureColumnToString(MeasureColumns column);


    signals:
      void controlPointChanged(QString pointId);
      void controlPointAdded(QString pointId);
      void ignorePointChanged();
      void ignoreLeftChanged();
      void ignoreRightChanged();
      void netChanged();
      void newControlNetwork(ControlNet *);
      void stretchChipViewport(Stretch *, CubeViewport *);
      void measureChanged();
      void saveControlNet();

    public slots:
      void setSerialNumberList(SerialNumberList *snList);
      void setControl(Control *control);
      void setEditPoint(ControlPoint *controlPoint);
      void deletePoint(ControlPoint *controlPoint);

      void createControlPoint(double latitude, double longitude, Cube *cube = 0,
                              bool isGroundSource = false);

      void updatePointInfo(ControlPoint &updatedPoint);
//    void refresh();

    protected:
      bool eventFilter(QObject *o,QEvent *e);

    private slots:
//    void enterWhatsThisMode();
      void saveNet();
//    void saveAsNet();
//    void addMeasure();
//    void setPointType (int pointType);
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
      void savePoint();
      void colorizeSavePointButton();

      void openTemplateFile();
      void viewTemplateFile();
      void saveChips();
      void showHideTemplateEditor();
      void saveTemplateFile();
      void saveTemplateFileAs();
      void setTemplateModified();
      void writeTemplateFile(QString);
      void clearEditPoint();

      void colorizeSaveNetButton();

    private:
      void createActions();

      void loadPoint();
      void loadMeasureTable();
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

      ControlMeasure *createTemporaryGroundMeasure();
      FileName findGroundFile();
      void changeGroundLocationsInNet();


    private:

      QPointer<QWidget> m_parent; //!< Parent widget
      Directory *m_directory;
      bool m_addMeasuresButton;   //!< Indicates whether or not to add "Add Measures(s) to Point"

      QString m_cnetFileName; //!< Filename of the control network that is being modified
      QPointer<QLabel> m_cnetFileNameLabel; //!< Label with name of the control network file
      bool m_netChanged; //!< Indicates if the control network has been modified

      QPointer<QAction> m_closePointEditor; //!< Action to close the point editor

      QPointer<QAction> m_saveChips; //!< Action to save the registration chips
      //! Action to toggle visibility of the registration template editor
      QPointer<QAction> m_showHideTemplateEditor;
      QPointer<QAction> m_openTemplateFile; //!< Action to open a registration template file to disk
      QPointer<QAction> m_saveTemplateFile; //!< Action to save a registration template file to disk
      QPointer<QAction> m_saveTemplateFileAs; //!< Action to save a new registration template


      //! Pointer to control measure editor widget
      QPointer<ControlMeasureEditWidget> m_measureEditor;

      QPointer<QPushButton> m_savePoint; //!< Button to save current point being edited
      QPalette m_saveDefaultPalette; //!< Default color pallet of the "Save Point" button

      QPointer<QPushButton> m_saveNet; //!< Button to save the current control network

      QPointer<QTextEdit> m_templateEditor; //!< Text editor for editing the registration template
      QPointer<QWidget> m_templateEditorWidget; //!< Template editor widget
      bool m_templateModified; //!< Indicates if the registration template was edited

      QPointer<QLabel> m_templateFileNameLabel; //!< Label for the template filename
      QPointer<QLabel> m_ptIdValue; //!< Label for the point id of the current point
      QPointer<QComboBox> m_pointType; //!< Combobox to change the type of the current point
      QPointer<QLabel> m_numMeasures;
      
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

      QPointer<ControlPoint> m_editPoint; //!< The control point being edited
      SerialNumberList *m_serialNumberList; //!< Serial number list for the loaded cubes
      QPointer<ControlNet> m_controlNet; //!< Current control net

      QPointer<ControlPoint> m_newPoint; //!< New control point
      QString m_lastUsedPointId; //!< Point id of the last used control point

      QStringList m_pointFiles; //!< Associated files for current control point

      QString m_leftFile; //!< Filename of left measure
      QPointer<ControlMeasure> m_leftMeasure; //!< Left control measure
      QPointer<ControlMeasure> m_rightMeasure; //!< Right control measure
      QScopedPointer<Cube> m_leftCube; //!< Left cube
      QScopedPointer<Cube> m_rightCube; //!< Right cube

      QList<Cube *> m_pointCubes;

      QString m_groundSN;
      bool m_changeAllGroundLocation;
      bool m_changeGroundLocationInNet;
      QString m_newGroundDir;
  };
};
#endif
