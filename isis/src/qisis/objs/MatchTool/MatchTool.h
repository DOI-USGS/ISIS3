#ifndef MatchTool_h
#define MatchTool_h

#include "Tool.h"
#include "ControlPoint.h"
#include "SerialNumberList.h"

#include <QCloseEvent>
#include <QHideEvent>
#include <QPalette>
#include <QPointer>
#include <QStatusBar>
#include <QStringList>

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
class QString;
class QTableWidget;
class QTextEdit;
class QWidget;

namespace Isis {
  class ControlMeasure;
  class ControlNet;
  class ControlPointEdit;
  class Cube;
  class CubeViewport;
  class MainWindow;
  class MatchToolNewPointDialog;
  class MdiCubeViewport;
  class MatchHoldPointDialog;
  class SerialNumberList;
  class Stretch;
  class ToolPad;
  class UniversalGroundMap;

  /**
   * @brief Match tool operations
   *
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2008-11-24 Jeannie Walldren - Changed name from TieTool.
   *                          Replace references to PointEdit class with
   *                          ControlPointEdit. Added "Goodness of Fit" to right
   *                          and left measure info.
   *   @history 2008-11-26  Jeannie Walldren - Added "Number of Measures" to
   *                          MatchTool point information. Defined updateNet() so
   *                          that the MatchTool window's title bar contains the
   *                          name of the control net file. Created
   *                          ignoreChanged() signal, modified pointSaved() and
   *                          createMatchTool() so message box appears if users are
   *                          saving an "Ignore" point and asks whether they would
   *                          like to set Ignore=false.
   *   @history 2008-12-09 Tracie Sucharski - Cleaned up some
   *                          signal/slot connections between MatchTool and
   *                          MatchNavTool for deleting or adding ControlPoints.
   *   @history 2008-12-09 Tracie Sucharski - Add new public slot
   *                          refresh to handle the ignorePoints and deletePoints
   *                          from the MatchNavTool.
   *   @history 2008-12-10 Jeannie Walldren - Added slot methods
   *                          viewTemplateFile() and setTemplateFile() to allow
   *                          user to view, edit or choose a new template file.
   *                          Added "What's this?" descriptions to actions.
   *   @history 2008-12-15 Jeannie Walldren - Some QMessageBox
   *                          warnings had strings tacked on to the list of
   *                          errors.  These strings were changed to iExceptions
   *                          and added to the error stack to conform with Isis
   *                          standards.
   *   @history 2008-12-15 Jeannie Walldren - Created newHoldPoint() method.
   *                          Replaced references to MatchGroundPointDialog with
   *                          MatchHoldPointDialog. Disabled ground point check box
   *                          so user may see whether the point is ground but may
   *                          not change this.  Thus setGroundPoint() and
   *                          newGroundPoint() methods still exist but are not
   *                          currently called.
   *   @history 2008-12-30 Jeannie Walldren - Modified to set measures in
   *                          viewports to Ignore=False if when saving, the user
   *                          chooses to set a point's Ignore=False. Replaced
   *                          references to ignoreChanged() with
   *                          ignorePointChanged(). Added signals
   *                          ignoreLeftChanged() and ignoreRightChanged().
   *   @history 2008-12-31 Jeannie Walldren - Added question box to pointSaved()
   *                          method to ask user whether the reference measure
   *                          should be replaced with the measure in the left
   *                          viewport. Added documentation.
   *   @history 2009-03-09 Jeannie Walldren - Modified createPoint() method to
   *                          clear the error stack after displaying a QMessageBox
   *                          to the user
   *   @history 2009-03-17 Tracie Sucharski - Added the ability to save the
   *                          registration chips to the Options menu.
   *   @history 2010-01-27 Jeannie Walldren - Fixed bug in setIgnoreLeftMeasure()
   *                          and setIgnoreRightMeasure() that caused segmentation
   *                          faults. Added question box to warn user that they
   *                          are saving changes to an ignored measure.
   *   @history 2010-06-02 Jeannie Walldren - Added cancelHoldPoint() method and
   *                          connected this slot to MatchHoldPointDialog's
   *                          holdCancelled() signal.
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers to null in
   *                          constructor.
   *   @history 2010-06-08 Jeannie Walldren - Fixed bug in drawAllMeasurments() so
   *                          that points with ignored measures matching the image
   *                          shown are drawn as yellow unless the MatchTool is
   *                          open to this point.  Changed QMessageBox types for
   *                          consistency with other match warnings and errors
   *   @history 2010-07-01 Jeannie Walldren - Added createToolBarWidget(),
   *                          showNavWindow() methods to reopen NavTool
   *                          (if closed) whenever the Tie tool button on the tool
   *                          pad is clicked.  Modified drawAllMeasurments() to
   *                          draw points selected in MatchTool last so they lay on
   *                          top of all other points in the image.  Replaced
   *                          #includes with forward class declarations and moved
   *                          #include to .cpp file.
   *   @history 2010-07-12 Jeannie Walldren - Fixed bug in deletePoint() method.
   *                          To prevent a seg fault, set m_controlPoint to NULL
   *                          and emit editPointChanged with an empty string if
   *                          the entire point is deleted.
   *   @history 2010-07-12 Jeannie Walldren - Fixed bug in newHoldPoint() method
   *                          that was causing the Hold Point Dialog to call the
   *                          reject() command.  Updated documentation.
   *  @history 2010-10-28 Tracie Sucharski - Fixed some include problems caused
   *                          by changes made to the ControlNet,ControlPoint,
   *                          ControlMeasure header files.  Remove findPointFiles
   *                          method, the code is now in the createPoint method.
   *  @history 2010-11-19 Tracie Sucharski - Renamed pointSaved slot to measureSaved.
   *
   *
   *   @history 2010-11-17 Eric Hyer - Added newControlNetwork SIGNAL
   *   @history 2010-11-22 Eric Hyer - Added stretchChipViewport SIGNAL for
   *                forwarding of SIGNAL from StretchTool to ChipViewport
   *   @history 2010-12-08 Eric Hyer - Template filename now shown.  Widgets
   *                in main window now organized into groupBoxes.  Removed
   *                Options menu and moved registration menu to main menu bar.
   *                Added toolbar for actions also in menu.  All actions now
   *                have icons.
   *   @history 2010-12-14 Eric Hyer - Template editor is now a widget within
   *                the main window.  Newly saved template files take effect
   *                after saving.
   *   @history 2010-12-17 Eric Hyer - Fixed bug where current template file
   *                was not being updated with saveAs.  Moved template file
   *                display to outside of control point groupbox.
   *   @history 2011-06-08 Tracie Sucharski - Point types renamed:
   *                   Ground ----> Fixed
   *                   Tie    ----> Free
   *   @history 2011-06-28 Tracie Sucharski - Added methods, "loadMeasureTable"
   *                          and "measureColumnToString".  TODO:  If these
   *                          stay in match, they really need cleaning up.  This
   *                          was a quick & dirty band-aid until cneteditor is
   *                          ready to be folded into match.
   *   @history 2011-07-27 Tracie Sucharski - Added method to return the radius
   *                          from a Dem if one is open.
   *   @history 2011-09-16 Tracie Sucharski - Added method to draw Fixed and
   *                          Constrained points on the ground source viewport.
   *   @history 2012-01-11 Tracie Sucharski - Add error check for invalid lat,
   *                          lon when creating new control point.
   *   @history 2012-04-09 Tracie Sucharski - When checking if left measure
   *                          editLock has changed, use measure->IsEditLocked()
   *                          instead of this classes IsMeasureLocked().
   *   @history 2012-04-16 Tracie Sucharski - When attempting to un-lock a measure
   *                          print error if point is locked.
   *   @history 2012-04-26 Tracie Sucharski - Cleaned up private slot,measureSaved.
   *                          Abstracted out checking for a new reference measure
   *                          and updating the surface point for a ground point.
   *   @history 2012-05-07 Tracie Sucharski - Removed code in measureSaved to re-load
   *                          left measure if left and right are the same, this is
   *                          already handled in ControlPointEdit::saveMeasure.
   *   @history 2012-05-08 Tracie Sucharski - Fixed bug where m_leftFile was not
   *                          being reset in mouseButtonRelease.  Change m_leftFile
   *                          from a QString to a QString.
   *   @history 2012-06-12 Tracie Sucharski - Change made to measureSaved on 2012-04-26 caused a
   *                          bug where if no ground is loaded the checkReference was not being
   *                          called and reference measure could not be changed and there was no
   *                          warning printed.  Fix:  Only call checkReference if there is no
   *                          explicit reference for the point, otherwise simply set reference to
   *                          left measure.  If ground source is on the left do not print warning
   *                          about new reference measure.
   *   @history 2012-08-11 Tracie Sucharski - In ::openNet, do not prompt for saving current
   *                          net if net hasn't changed since last save.
   *   @history 2012-10-02 Tracie Sucharski - Qview was printing error messages and somtimes
   *                          crashing when viewports were closed either individually or using
   *                          "Close All" from the file menu.  The serial number list was not
   *                          being updated correctly.  The fix no longer keeps a maintainable
   *                          serial number list, but calculates one in real-time base on available
   *                          viewports.  Fixes #1130.  Also, added a few more error checks to
   *                          insure cubes for displayed measures are available.
   * @history 2013-05-09 Tracie Sucharski - Check for user selecting all measures for deletion and
   *                          print warning that point will be deleted. References #1491.
   * @history 2013-05-09 Tracie Sucharski - For editing (left button) and deleting (right button),
   *                          Swapped checking for empty network and not allowing mouse clicks on
   *                          the ground source. First check if there are any points in the network.
   *                          If not print message and return.  References #1493.
   * @history 2015-11-08 Ian Humphrey - Added shortcut for Save Point (P). Added shortcuts
   *                         (PageUp/PageDown) for selecting right measures and added slots to
   *                         handle these shortcuts. References #2324.
   * @history 2016-08-28 Kelvin Rodriguez - Removed unused member variables to eliminate warnings
   *                         in clang. Part of porting to OS X 10.11
   */
  class MatchTool : public Tool {
    Q_OBJECT

    public:
      MatchTool(QWidget *parent);
      virtual ~MatchTool();
      void paintViewport(MdiCubeViewport *mvp, QPainter *painter);

//    void addTo(Workspace *);

      // measure column values
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
      static const int NUMCOLUMNS = 16;

      QString measureColumnToString(MeasureColumns column);


    signals:
      void editPointChanged();
      void ignorePointChanged();
      void ignoreLeftChanged();
      void ignoreRightChanged();
      void newControlNetwork(ControlNet *);
      void stretchChipViewport(Stretch *, CubeViewport *);
      void measureChanged();

    public slots:
      void createPoint(MdiCubeViewport *mvp, double sample, double line);
      void modifyPoint(ControlPoint *point);
      void deletePoint(ControlPoint *point);
      void updatePointInfo(QString pointId);
      void refresh();

    protected:
      QAction *toolPadAction(ToolPad *pad);
      bool eventFilter(QObject *o,QEvent *e);

    protected slots:
      QWidget *createToolBarWidget(QStackedWidget *parent);
      void mouseButtonRelease(QPoint p, Qt::MouseButton s);

    private slots:
      void showHelp();
      void enterWhatsThisMode();
      void paintAllViewports ();
      void activateTool();
      void openNet();
      void saveNet();
      void saveAsNet();
      void setLockPoint (bool ignore);
      void setIgnorePoint (bool ignore);
      void setLockLeftMeasure (bool ignore);
      void setIgnoreLeftMeasure (bool ignore);
      void setLockRightMeasure (bool ignore);
      void setIgnoreRightMeasure (bool ignore);

      void nextRightMeasure();
      void previousRightMeasure();
      void selectLeftMeasure (int index);
      void selectRightMeasure (int index);
      void updateLeftMeasureInfo ();
      void updateRightMeasureInfo ();

      void measureSaved();
      void checkReference();
      void savePoint();
      void colorizeSaveButton();

      void cancelNewPoint();
      void doneWithMeasures();

      void openTemplateFile();
      void viewTemplateFile();
      void saveChips();
      void showHideTemplateEditor();
      void saveTemplateFile();
      void saveTemplateFileAs();
      void setTemplateModified();
      void writeTemplateFile(QString);
      void clearEditPoint();

      void exiting();

    private:
      void createActions();
      void createMenus();
      void createToolBars();

      void loadPoint();
      void loadMeasureTable();
      void drawAllMeasurments (MdiCubeViewport *mvp,QPainter *painter);
      void createMatchTool(QWidget *parent);
      QSplitter * createTopSplitter();
      QGroupBox * createControlPointGroupBox();
      QGroupBox * createLeftMeasureGroupBox();
      QGroupBox * createRightMeasureGroupBox();
      void createTemplateEditorWidget();
      void loadTemplateFile(QString);
      bool okToContinue();
      bool IsMeasureLocked(QString serialNumber);
      void addMeasure(MdiCubeViewport *mvp, double sample, double line);
      bool validateMeasureChange(ControlMeasure *m);

      QStringList missingCubes(ControlPoint *point);

      // TODO  pointer, reference, what???
      SerialNumberList serialNumberList();
      QString serialNumber(MdiCubeViewport *mvp);
      void addViewportToSerialNumberList(MdiCubeViewport *mvp);

      void readSettings();
      void writeSettings() const;

    private:

      QMainWindow *m_matchTool;

      QWidget *m_parent;
      QStatusBar *m_statusBar;

      QString m_cnetFileName;
      QLabel *m_cnetFileNameLabel;
      bool m_coregNet;
      QString m_coregReferenceSN;
      bool m_netChanged;

      QAction *m_createPoint;
      QAction *m_modifyPoint;
      QAction *m_deletePoint;

      QAction *m_saveNet;
      QAction *m_saveAsNet;
      QAction *m_closeMatchTool;

      QAction *m_saveChips;
      QAction *m_showHideTemplateEditor;
      QAction *m_openTemplateFile;
      QAction *m_saveTemplateFile;
      QAction *m_saveTemplateFileAs;

      QAction *m_whatsThis;
      QAction *m_showHelp;

      ControlPointEdit *m_pointEditor;

      QPushButton *m_savePoint;
      QPalette m_saveDefaultPalette;

      QTextEdit *m_templateEditor;
      QWidget *m_templateEditorWidget;
      bool m_templateModified;

      QLabel *m_templateFileNameLabel;
      QLabel *m_ptIdValue;
      QLabel *m_numMeasures;

      QCheckBox *m_lockPoint;
      QCheckBox *m_ignorePoint;
      QLabel *m_leftReference;
      QLabel *m_leftMeasureType;
      QLabel *m_leftSampShift;
      QLabel *m_leftLineShift;
      QLabel *m_leftGoodness;
      QLabel *m_rightGoodness;
      QLabel *m_rightReference;
      QLabel *m_rightMeasureType;
      QLabel *m_rightSampShift;
      QLabel *m_rightLineShift;
      QCheckBox *m_lockLeftMeasure;
      QCheckBox *m_ignoreLeftMeasure;
      QCheckBox *m_lockRightMeasure;
      QCheckBox *m_ignoreRightMeasure;

      QComboBox *m_leftCombo;
      QComboBox *m_rightCombo;

      QMainWindow *m_measureWindow;
      QTableWidget *m_measureTable;

      QPointer<ControlNet> m_controlNet;
      ControlPoint *m_editPoint;
      MatchToolNewPointDialog *m_newPointDialog;
      ControlPoint *m_newPoint;
      QString m_lastUsedPointId;

      QStringList m_pointFiles;

      QString m_leftFile;
      ControlMeasure *m_leftMeasure;
      ControlMeasure *m_rightMeasure;
      Cube *m_leftCube;
      Cube *m_rightCube;

  };
};
#endif
