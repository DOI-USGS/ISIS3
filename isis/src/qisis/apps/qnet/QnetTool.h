#ifndef QnetTool_h
#define QnetTool_h

#include "Tool.h"
#include "ControlPoint.h"

#include <QPalette>
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
class QTextEdit;
class QWidget;

namespace Isis {
  class ControlMeasure;
  class ControlNet;
  class Cube;
  class iString;
  class Stretch;
  class UniversalGroundMap;
}

namespace Qisis {
  class ControlPointEdit;
  class CubeViewport;
  class MdiCubeViewport;
  class QnetHoldPointDialog;
  class ToolPad;
  /**
   * @brief Qnet tool operations
   *
   * @ingroup Visualization Tools
   *
   *
   * @internal
   *   @history 2008-11-24 Jeannie Walldren - Changed name from TieTool.
   *                          Replace references to PointEdit class with
   *                          ControlPointEdit. Added "Goodness of Fit" to right
   *                          and left measure info.
   *   @history 2008-11-26  Jeannie Walldren - Added "Number of Measures" to
   *                          QnetTool point information. Defined updateNet() so
   *                          that the QnetTool window's title bar contains the
   *                          name of the control net file. Created
   *                          ignoreChanged() signal, modified pointSaved() and
   *                          createQnetTool() so message box appears if users are
   *                          saving an "Ignore" point and asks whether they would
   *                          like to set Ignore=false.
   *   @history 2008-12-09 Tracie Sucharski - Cleaned up some
   *                          signal/slot connections between QnetTool and
   *                          QnetNavTool for deleting or adding ControlPoints.
   *   @history 2008-12-09 Tracie Sucharski - Add new public slot
   *                          refresh to handle the ignorePoints and deletePoints
   *                          from the QnetNavTool.
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
   *                          Replaced references to QnetGroundPointDialog with
   *                          QnetHoldPointDialog. Disabled ground point check box
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
   *                          connected this slot to QnetHoldPointDialog's
   *                          holdCancelled() signal.
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers to null in
   *                          constructor.
   *   @history 2010-06-08 Jeannie Walldren - Fixed bug in drawAllMeasurments() so
   *                          that points with ignored measures matching the image
   *                          shown are drawn as yellow unless the QnetTool is
   *                          open to this point.  Changed QMessageBox types for
   *                          consistency with other qnet warnings and errors
   *   @history 2010-07-01 Jeannie Walldren - Added createToolBarWidget(),
   *                          showNavWindow() methods to reopen NavTool
   *                          (if closed) whenever the Tie tool button on the tool
   *                          pad is clicked.  Modified drawAllMeasurments() to
   *                          draw points selected in QnetTool last so they lay on
   *                          top of all other points in the image.  Replaced
   *                          #includes with forward class declarations and moved
   *                          #include to .cpp file.
   *   @history 2010-07-12 Jeannie Walldren - Fixed bug in deletePoint() method.
   *                          To prevent a seg fault, set p_controlPoint to NULL
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
   */
  class QnetTool : public Tool {
    Q_OBJECT

    public:
      QnetTool (QWidget *parent);
      virtual ~QnetTool ();
      void paintViewport (MdiCubeViewport *cvp,QPainter *painter);

    signals:
      void qnetToolSave();
      void refreshNavList();
      void editPointChanged(QString pointId);
      void netChanged();
      void ignorePointChanged();
      void ignoreLeftChanged();
      void ignoreRightChanged();
      void showNavTool();
      void newControlNetwork(Isis::ControlNet *);
      void stretchChipViewport(Isis::Stretch *, Qisis::CubeViewport *);
      void measureChanged();

    public slots:
      void updateList();
      void updateNet(QString cNetFilename);
      void createPoint(double lat,double lon);
      void createFixedPoint(double lat,double lon);
      void modifyPoint(Isis::ControlPoint *point);
      void deletePoint(Isis::ControlPoint *point);
      void updatePointInfo(QString pointId);
      void refresh();

    protected:
      QAction *toolPadAction(ToolPad *pad);
      bool eventFilter(QObject *o,QEvent *e);

    protected slots:
      void mouseButtonRelease(QPoint p, Qt::MouseButton s);
      QWidget *createToolBarWidget (QStackedWidget *parent);

    private slots:
      void paintAllViewports (QString pointId );
      void saveNet();
      void addMeasure();
      void setLockPoint (bool ignore);
      void setIgnorePoint (bool ignore);
      void setFixedPoint (bool fixed);
      void setLockLeftMeasure (bool ignore);
      void setIgnoreLeftMeasure (bool ignore);
      void setLockRightMeasure (bool ignore);
      void setIgnoreRightMeasure (bool ignore);
      void showNavWindow (bool checked);

      void updateSurfacePointInfo ();

      void selectLeftMeasure (int index);
      void selectRightMeasure (int index);
      void updateLeftMeasureInfo ();
      void updateRightMeasureInfo ();

      void measureSaved ();
      void savePoint();
      void colorizeSaveButton();

      void openTemplateFile();
      void viewTemplateFile();
      void saveChips();

      void openGround();
      void openDem();
      void showHideTemplateEditor();
      void saveTemplateFile();
      void saveTemplateFileAs();
      void setTemplateModified();
      void writeTemplateFile(QString);


    private:
      void createActions();
      void createMenus();
      void createToolBars();
      void loadPoint();
      void drawAllMeasurments (MdiCubeViewport *vp,QPainter *painter);
//      void drawMeasures (MdiCubeViewport *vp,QPainter *painter,Isis::ControlPoint &point);
      void createQnetTool(QWidget *parent);
      QSplitter * createTopSplitter();
      QGroupBox * createControlPointGroupBox();
      QGroupBox * createLeftMeasureGroupBox();
      QGroupBox * createRightMeasureGroupBox();
      void createTemplateEditorWidget();
      void loadTemplateFile(QString);
      bool okToContinue();
      void initDem(QString demFile);
      void clearGroundSource();


      QMainWindow *p_qnetTool;

      QStringList findPointFiles(double lat, double lon);

      QAction *p_createPoint;
      QAction *p_modifyPoint;
      QAction *p_deletePoint;

      QAction *p_openGround;
      QAction *p_openDem;
      QAction *p_saveNet;
      QAction *p_closeQnetTool;

      QAction *p_saveChips;
      QAction *p_showHideTemplateEditor;
      QAction *p_openTemplateFile;
      QAction *p_saveTemplateFile;
      QAction *p_saveTemplateFileAs;
      
      QMainWindow *p_mw;
      ControlPointEdit *p_pointEditor;

      QPushButton *p_savePoint;
      QPalette p_saveDefaultPalette;

      QTextEdit *p_templateEditor;
      QWidget *p_templateEditorWidget;
      bool p_templateModified;

      QLabel *p_templateFilenameLabel;
      QLabel *p_groundFilenameLabel;
      QLabel *p_radiusFilenameLabel;
      QLabel *p_ptIdValue;
      QLabel *p_numMeasures;
      QLabel *p_pointAprioriLatitude;
      QLabel *p_pointAprioriLongitude;
      QLabel *p_pointAprioriRadius;
      QLabel *p_pointAprioriLatitudeSigma;
      QLabel *p_pointAprioriLongitudeSigma;
      QLabel *p_pointAprioriRadiusSigma;
      QLabel *p_pointLatitude;
      QLabel *p_pointLongitude;
      QLabel *p_pointRadius;
      
      QCheckBox *p_lockPoint;
      QCheckBox *p_ignorePoint;
      QCheckBox *p_fixedPoint;
      QLabel *p_leftReference;
      QLabel *p_leftMeasureType;
      QLabel *p_leftSampError;
      QLabel *p_leftLineError;
      QLabel *p_leftGoodness;
      QLabel *p_rightGoodness;
      QLabel *p_rightReference;
      QLabel *p_rightMeasureType;
      QLabel *p_rightSampError;
      QLabel *p_rightLineError;
      QCheckBox *p_lockLeftMeasure;
      QCheckBox *p_ignoreLeftMeasure;
      QCheckBox *p_lockRightMeasure;
      QCheckBox *p_ignoreRightMeasure;

      QComboBox *p_leftCombo;
      QComboBox *p_rightCombo;

      Isis::ControlPoint *p_editPoint;

      QStringList p_pointFiles;

      std::string p_leftFile;
      Isis::ControlMeasure *p_leftMeasure;
      Isis::ControlMeasure *p_rightMeasure;
      Isis::Cube *p_leftCube;
      Isis::Cube *p_rightCube;

      QString p_groundFile;
      Isis::Cube *p_groundCube;
      Isis::iString p_groundSN;
      Isis::UniversalGroundMap *p_groundGmap;
      bool p_groundOpen;
      Isis::ControlPoint::SurfacePointSource::Source p_groundSurfacePointSource;
      Isis::ControlPoint::RadiusSource::Source p_groundRadiusSource;
      //  TODO:  Combine the following p_groundSourceFile, p_radiusSourceFile
      //           with p_groundFile and p_demFile.  Is it just a matter of
      //           full path vs filename only?
      Isis::iString p_groundSourceFile;
      Isis::iString p_radiusSourceFile;
      QString p_demFile;
      bool p_demOpen;
      Isis::Cube *p_demCube;
  };
};

#endif
