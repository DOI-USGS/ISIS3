#ifndef ControlPointEdit_h
#define ControlPointEdit_h

// This is the only include allowed in this file!
#include <QWidget>


class QCheckBox;
class QDial;
class QDoubleSpinBox;
class QHBoxLayout;
class QLabel;
class QLCDNumber;
class QPushButton;
class QRadioButton;
class QScrollBar;
class QTimer;
class QToolButton;
class QVBoxLayout;

namespace Isis {
  class AutoReg;
  class Chip;
  class ControlMeasure;
  class ControlNet;
  class Cube;
  class Stretch;
  class UniversalGroundMap;
}

namespace Qisis {
  class ChipViewport;
  class CubeViewport;
  class Tool;


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
    *   @history 2010-11-22 Eric Hyer - Forwarded SIGNAL from StretchTool to
    *                          ChipViewports
    */
  class ControlPointEdit : public QWidget {
      Q_OBJECT

    public:
      ControlPointEdit(Isis::ControlNet * cnetQ, QWidget *parent = 0,
                       bool allowLeftMouse = false);
      std::string templateFilename() {
        return p_templateFilename;
      };
      void setTemplateFile();
      void allowLeftMouse(bool allowMouse);

    signals:
      void updateLeftView(double sample, double line);
      void updateRightView(double sample, double line);
      void pointSaved();
      void newControlNetwork(Isis::ControlNet *);
      void stretchChipViewport(Isis::Stretch *, Qisis::CubeViewport *);

    public slots:
      void setLeftMeasure(Isis::ControlMeasure *leftMeasure,
                          Isis::Cube *leftCube, std::string pointId);
      void setRightMeasure(Isis::ControlMeasure *rightMeasure,
                           Isis::Cube *rightCube, std::string pointId);
      void saveChips();

    protected slots:

    private slots:

      void setNoGeom();
      void setGeom();
      void setRotate();
      void setCircle(bool);

      void findPoint();
      void registerPoint();
      void savePoint();
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

      std::string p_templateFilename;
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


      ChipViewport *p_leftView;
      ChipViewport *p_rightView;

      Isis::Cube *p_leftCube;
      Isis::Cube *p_rightCube;
      Isis::ControlMeasure *p_leftMeasure;
      Isis::ControlMeasure *p_rightMeasure;
      Isis::Chip *p_leftChip;
      Isis::Chip *p_rightChip;
      Isis::UniversalGroundMap *p_leftGroundMap;
      Isis::UniversalGroundMap *p_rightGroundMap;

      Isis::AutoReg *p_autoRegFact;
      std::string p_pointId;

      int p_rotation;
      bool p_geomIt;
  };
};

#endif
