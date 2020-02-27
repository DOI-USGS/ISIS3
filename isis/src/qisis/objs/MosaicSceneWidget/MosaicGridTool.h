#ifndef MosaicGridTool_h
#define MosaicGridTool_h

#include "MosaicTool.h"

#include <QPointer>

#include "Angle.h"
#include "Latitude.h"
#include "Longitude.h"

class QAction;
class QCheckBox;
class QDialog;
class QGraphicsItem;
class GridGraphicsItem;
class QLabel;
class Projection;
class QPushButton;
class QToolButton;

namespace Isis {
  /**
   * @brief This controls the 'Grid' abilities in the MosaicSceneWidget
   *
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Stacy Alley
   *
   * @internal
   *   @history 2011-05-07 Steven Lambright - Refactored along with all of
   *                           'qmos'
   *   @history 2011-05-11 Steven Lambright - Added project settings
   *   @history 2012-04-16 Jeannie Backer - Added #include for PvlObject class
   *                           in implementation file.
   *   @history 2012-07-10 Kimberly Oyama and Steven Lambright - Added an auto grid option that
   *                           draws a grid with lat/lon increments and extents based on either
   *                           the map projection, the bounding rectangle of the open cubes, or the
   *                           user entered extents. Also, added accessors, mutators, and other
   *                           support for a new options dialog that lets the user configure the
   *                           grid. Changed the draw/clear grid buttons to a show grid checkbox.
   *                           Fixes #604.
   *   @history 2013-02-01 Steven Lambright - Fixed a problem with setLonInc() bounding the maximum
   *                           longitude increment incorrectly, which caused a bad increment. This
   *                           resulted in freezing or an invalid grid. Fixes #1060.
   *   @history 2013-03-06 Steven Lambright - Added support for getting target radii from TargetName
   *                           if the mapping radii keywords are missing.
   *   @history 2013-03-19 Steven Lambright - Auto grid now remembers its last setting and defaults
   *                           to it.
   *   @history 2013-09-11 Tracie Sucharski - Check for existence of scene widget before calling
   *                           methods on it in domainMinLon() and domainMaxLon().  Fixes #1748.
   */
  class MosaicGridTool : public MosaicTool {
      Q_OBJECT

    public:
      enum GridExtentSource {
        /**
         * The grid will be drawn using the extents from the map projection.
         */
        Map,
        /**
         * The grid will be drawn using the extents from the bounding rectangle
         *   of the open cubes.
         */
        Cubes,
        /**
         * The grid will be drawn using the extents that the user specifies.
         */
        Manual
      };

      MosaicGridTool(MosaicSceneWidget *);
      void addToMenu(QMenu *menu);

      //Accessors
      bool autoGridCheckBox();
      Latitude baseLat();
      Longitude baseLon();
      int density();
      Angle latInc();
      GridExtentSource latExtents();
      QString  latType();
      QString lonDomain();
      GridExtentSource lonExtents();
      Angle lonInc();
      Latitude maxLat();
      Longitude maxLon();
      Latitude minLat();
      Longitude minLon();
      MosaicSceneWidget *sceneWidget();
      bool showGrid();

      //Mutators
      void setAutoGridCheckBox(bool checked);
      void setBaseLat(Latitude baseLat);
      void setBaseLon(Longitude baseLon);
      void setDensity(int density);
      void setLatExtents(GridExtentSource source, Latitude minLat, Latitude maxLat);
      void setLatInc(Angle latInc);
      void setLonExtents(GridExtentSource source, Longitude minLon, Longitude maxLon);
      void setLonInc(Angle lonInc);
      void setShowGrid(bool show);

      //Processors
      void fromPvl(const PvlObject &obj);
      QString projectPvlObjectName() const;
      PvlObject toPvl() const;

      Longitude domainMinLon();
      Longitude domainMaxLon();

    signals:
      void boundingRectChanged();


    public slots:
      void autoGrid(bool draw);
      void clearGrid();
      void configure();
      void drawGrid();
      void drawGrid(bool draw);
      void onCubesChanged();
      void onToolOpen(bool check);
      void onProjectionChanged();
    
    protected:
      QWidget *createToolBarWidget();
      QAction *getPrimaryAction();
      QWidget *getToolBarWidget();

    private:
      QPointer<QLabel> m_autoGridLabel; //!< Enabled and diabled with the autoGrid checkbox.
      QPointer<QCheckBox> m_autoGridCheckBox; //!< True if grid properties come from the open cubes
      QPointer<QCheckBox> m_drawGridCheckBox; //!< True if grid properties come from the open cubes
      bool m_shouldCheckBoxes; //!< True when the tool is first opened to check the checkboxes.

      Latitude m_baseLat; //!< Base latitude for drawing the grid.
      Longitude m_baseLon; //!< Base longitude for drawing the grid.

      Angle m_latInc; //!< Latitude increment for drawing the grid.
      Angle m_lonInc; //!< Longitude increment for drawing the grid.

      GridExtentSource m_latExtents; //!< Used for the state of the options dialog.
      Latitude m_maxLat; //!< Maximum latitude of the grid.
      Latitude m_minLat; //!< Minimum latitude of the grid.

      GridExtentSource m_lonExtents; //!< Used for the state of the options dialog.
      Longitude m_maxLon; //!< Maximum longitude of the grid.
      Longitude m_minLon; //!< Minimum longitude of the grid.

      int m_density; //!< Grid density for drawing the grid.

      QAction *m_action;
      QGraphicsItem *m_gridItem;
      QRectF m_previousBoundingRect; //!< The bounding rectangle of the previous set of open cubes.
  };
};

#endif

