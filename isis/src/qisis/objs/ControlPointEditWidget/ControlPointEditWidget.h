#ifndef ControlPointEditWidget_h
#define ControlPointEditWidget_h


#include "ControlPoint.h"

#include <QCloseEvent>
#include <QHideEvent>
#include <QPalette>
#include <QPointer>
#include <QStatusBar>
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
class QString;
class QTableWidget;
class QTextEdit;
class QWidget;

namespace Isis {
  class ControlMeasureEditWidget;
  class ControlMeasure;
  class ControlNet;
  class Cube;
  class CubeViewport;
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
   */
  class ControlPointEditWidget : public QWidget {
    Q_OBJECT

    public:
      ControlPointEditWidget(QWidget *parent, bool addMeasures = false);
      virtual ~ControlPointEditWidget();

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
      void setControlNet(ControlNet *cnet, QString cnetFilename);
      void setEditPoint(ControlPoint *controlPoint);

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
//    void updateSurfacePointInfo ();

      void selectLeftMeasure (int index);
      void selectRightMeasure (int index);
      void updateLeftMeasureInfo ();
      void updateRightMeasureInfo ();
      void updateSurfacePointInfo ();

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



    private:

      QPointer<QWidget> m_parent;
      bool m_addMeasuresButton;

      QString m_cnetFileName;
      QPointer<QLabel> m_cnetFileNameLabel;
      bool m_netChanged;

      QPointer<QAction> m_closePointEditor;

      QPointer<QAction> m_saveChips;
      QPointer<QAction> m_showHideTemplateEditor;
      QPointer<QAction> m_openTemplateFile;
      QPointer<QAction> m_saveTemplateFile;
      QPointer<QAction> m_saveTemplateFileAs;

      QPointer<ControlMeasureEditWidget> m_measureEditor;

      QPointer<QPushButton> m_savePoint;
      QPalette m_saveDefaultPalette;

      QPointer<QPushButton> m_saveNet;

      QPointer<QTextEdit> m_templateEditor;
      QPointer<QWidget> m_templateEditorWidget;
      bool m_templateModified;

      QPointer<QLabel> m_templateFileNameLabel;
      QPointer<QLabel> m_ptIdValue;
      QPointer<QComboBox> m_pointType;
      QPointer<QLabel> m_numMeasures;
      QPointer<QLabel> m_pointAprioriLatitude;
      QPointer<QLabel> m_pointAprioriLongitude;
      QPointer<QLabel> m_pointAprioriRadius;
      QPointer<QLabel> m_pointAprioriLatitudeSigma;
      QPointer<QLabel> m_pointAprioriLongitudeSigma;
      QPointer<QLabel> m_pointAprioriRadiusSigma;
      QPointer<QLabel> m_pointLatitude;
      QPointer<QLabel> m_pointLongitude;
      QPointer<QLabel> m_pointRadius;
      
      QPointer<QCheckBox> m_lockPoint;
      QPointer<QCheckBox> m_ignorePoint;
      QPointer<QLabel> m_leftReference;
      QPointer<QLabel> m_leftMeasureType;
      QPointer<QLabel> m_leftSampError;
      QPointer<QLabel> m_leftLineError;
      QPointer<QLabel> m_leftSampShift;
      QPointer<QLabel> m_leftLineShift;
      QPointer<QLabel> m_leftGoodness;
      QPointer<QLabel> m_rightGoodness;
      QPointer<QLabel> m_rightReference;
      QPointer<QLabel> m_rightMeasureType;
      QPointer<QLabel> m_rightSampError;
      QPointer<QLabel> m_rightLineError;
      QPointer<QLabel> m_rightSampShift;
      QPointer<QLabel> m_rightLineShift;
      QPointer<QCheckBox> m_lockLeftMeasure;
      QPointer<QCheckBox> m_ignoreLeftMeasure;
      QPointer<QCheckBox> m_lockRightMeasure;
      QPointer<QCheckBox> m_ignoreRightMeasure;

      QPointer<QComboBox> m_leftCombo;
      QPointer<QComboBox> m_rightCombo;

      QPointer<QMainWindow> m_measureWindow;
      QPointer<QTableWidget> m_measureTable;

      QPointer<ControlPoint> m_editPoint;
      SerialNumberList *m_serialNumberList;
      QPointer<ControlNet> m_controlNet;

//    QPointer<NewPointDialog> m_newPointDialog;
      QPointer<ControlPoint> m_newPoint;
      QString m_lastUsedPointId;

      QStringList m_pointFiles;

      QString m_leftFile;
      QPointer<ControlMeasure> m_leftMeasure;
      QPointer<ControlMeasure> m_rightMeasure;
      QScopedPointer<Cube> m_leftCube;
      QScopedPointer<Cube> m_rightCube;

  };
};
#endif
