#ifndef MosaicControlNetTool_h
#define MosaicControlNetTool_h

#include "MosaicTool.h"

class QDialog;
class QLabel;
class QPushButton;

namespace Isis {
  class ControlNet;
  class ControlNetGraphicsItem;
  class ControlPoint;
//   class ControlPointEdit;
  class Image;
  class ImageList;

  /**
   * //TODO: Remove debug printout & comment
   * //         2016-08-25 Tracie Sucharski - Checking Directory pointer for IPCE code not ideal.
   *                           Is there a better design?  This might go away if we emit signals,
   *                           which only IPCE classes would connect to.
   * @brief Handles Control Net displays
   *
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Christopher Austin
   *
   * @internal
   *   @history 2010-06-24 Christopher Austin - Added |x| functionality and
   *                           fixed control net loading
   *   @history 2011-05-07 Steven Lambright - Refactored.
   *   @history 2011-05-10 Steven Lambright - Reduced useless code, open cnet
   *                           button is now always enabled.
   *   @history 2011-05-10 Steven Lambright - Added label for currently open
   *                           network.
   *   @history 2011-09-27 Steven Lambright - Improved user documentation. Made
   *                           the open control network button more obvious.
   *   @history 2013-01-02 Steven Lambright - Implemented movement arrow colorization. This is a
   *                           quick and dirty implementation designed to get the most basic
   *                           functionality working with minimal options. Added the enum
   *                           MovementColorSource and the methods setMovementArrowColorSource(),
   *                           movementArrowColorSource(), maxMovementColorMeasureCount(),
   *                           maxMovementColorResidualMagnitude(), toString(),
   *                           and fromMovementColorSourceString(). Fixes #479.
   *   @history 2013-01-31 Steven Lambright - Removed some debugging statements that were left
   *                           around from the last change. Fixes #1459.
   *   @history 2016-09-14 Tracie Sucharski - Added signals for mouse clicks for modifying, deleting
   *                           and creating control points.  These are passed on to
   *                           MosaicSceneWidget signals, then on to Directory slots.
   *   @history 2016-08-25 Tracie Sucharski - Fixed IPCE code which caused qmos to segfault.  Added
   *                           checks for the existence of a Directory pointer in the openControlNet
   *                           and mouseButtonRelease methods.  If Directory point is NULL, IPCE
   *                           code not executed.  Fixes #4063.
   *   @history 2017-01-03 Tracie Sucharski - If IPCE, set m_controlNet to active control net in the
   *                           loadNetwork method and do not display Load/Close Network button if
   *                           IPCE.
   *   @history 2017-05-11 Tracie Sucharski - Tools can be created with a Null value for the
   *                           MosaicSceneWidget, so need to check for Null value getWidget() in
   *                           Tool constructor.  This was causing a segfault when accessing qmos
   *                           help and a Qt connection error.
   *   @history 2017-07-06 Tracie Sucharski - Added documentation for conditional code necessary for
   *                           ipce application.
   *   @history 2017-08-02 Tracie Sucharski - Added slot to rebuild the graphics items that depict
   *                           the control points.  This is used to update these items if a control
   *                           point was added or deleted.  Fixes #5007, #5008.
   *   @history 2017-08-15 Tracie Sucharski - Added check in ::rebuildPointGraphics() to check for
   *                           existence of graphics items.  Fixes #4984.
   *   @history 2018-07-12 Tracie Sucharski - Made the slot loadNetwork public so that ipce can
   *                           load a new network.
   */
  class MosaicControlNetTool : public MosaicTool {
      Q_OBJECT

    public:
      /**
       * This enum defines how to draw the movement arrows (arrows from CP A Priori location to
       *   adjusted location). These settings include whether the arrows are shown and how to color
       *   them.
       *
       * NOTE: It's important to start at zero. Also, if you add to this enumeration, be sure to
       *       update NUM_MOVEMENT_COLOR_SOURCE_VALUES.
       */
      enum MovementColorSource {
        //! Do not show movement arrows
        NoMovement = 0,
        //! Show black movement arrows
        NoColor,
        //! Show movement arrows colored by measure count
        MeasureCount,
        //! Show movement arrows colored by residual magnitude
        ResidualMagnitude
      };
      //! This is the count of possible values of MovementColorSource (useful for loops).
      static const int NUM_MOVEMENT_COLOR_SOURCE_VALUES = 4;

      MosaicControlNetTool(MosaicSceneWidget *);
      ~MosaicControlNetTool();

      void addToMenu(QMenu *menu);

      PvlObject toPvl() const;
      void fromPvl(const PvlObject &obj);
      QString projectPvlObjectName() const;

      void setMovementArrowColorSource(MovementColorSource, int, double);
      MovementColorSource movementArrowColorSource() const;

      int maxMovementColorMeasureCount() const;
      double maxMovementColorResidualMagnitude() const;

      static QString toString(MovementColorSource);
      static MovementColorSource fromMovementColorSourceString(QString);

    signals:
      void modifyControlPoint(ControlPoint *controlPoint);
      void deleteControlPoint(ControlPoint *controlPoint);
      void createControlPoint(double latitude, double longitude);

      void controlPointSelected(ControlPoint *);
      void deleteControlPoint(QString controlPointId);

    public slots:
      void loadNetwork();
      void rebuildPointGraphics();
      void displayNewControlPoint(QString pointId);
      void displayChangedControlPoint(QString pointId);
      void displayUponControlPointDeletion();

    protected:
      QAction *getPrimaryAction();
      QWidget *getToolBarWidget();
      void mouseButtonRelease(QPointF, Qt::MouseButton s);

    private slots:
      void configMovement();
      void updateTool();
      void openControlNet();
      void displayControlNet();
      void displayConnectivity();
      void closeNetwork();
      void randomizeColors();

      void objectDestroyed(QObject *);

    private:
      void createDialog();

      Image *takeImage(QString sn, ImageList &images);

      QPushButton *m_loadControlNetButton;
      QPushButton *m_displayControlNetButton;
      QPushButton *m_displayConnectivity;
      QPushButton *m_configMovement;
      QPushButton *m_closeNetwork;
      QPushButton *m_randomizeColors;
      QAction *m_connectivity;
      ControlNet *m_controlNet;
      ControlNetGraphicsItem *m_controlNetGraphics;
      QLabel *m_controlNetFileLabel;
      QString m_controlNetFile;
//       ControlPointEdit *m_pointEditor;

      //! This defines the drawing mode of the apriori to adjusted arrows
      MovementColorSource m_movementArrowColorSource;
      //! This is the measure count at which we start coloring the movement arrows
      int m_measureCount;
      //! This is the residual magnitude at which we coloring the movement arrows
      double m_residualMagnitude;
  };
};

#endif
