#ifndef Qisis_MeasureTool_h
#define Qisis_MeasureTool_h

#include "Tool.h"

// FIXME: remove these includes
#include <QAction>
#include "RubberBandComboBox.h"
#include "TableMainWindow.h"
#include "iString.h"


class QDialog;
class QMenu;
class QLineEdit;
class QComboBox;

namespace Isis {
  class iString;
}

namespace Qisis {
  class MdiCubeViewport;

  /**
  * @brief Tool for measuring distances
  *
  * @ingroup Visualization Tools
  *
  * @author Unknown
  *
  * @internal
  *   @history 2007-11-19 Stacy Alley
  *                       added the capability of the qisis
  *                       windows to remember size and location
  *   @history 2007-11-29 Stacy Alley
  *                      replaced all the table stuff with the new
  *                      TableMainWindow class.
  *   @history 2008-08-18 Christopher Austin
  *                      Upgraded to geos3.0.0
  *   @history 2008-09-26 Steven Lambright Added Segmented line
  *   @history 2009-02-12 Steven Lambright Fixed bug where measure tool would
  *                      not measure pixels for non-camera, non-projection
  *                      cubes.
  *   @history 2010-02-17 Sharmila Prasad Fixed bug where the
  *            distance was calculated twice for a cube with both
  *            camera and projection
  *            Also made changes to save the most recent selection
  *           (km,m,pixels) when different tool is selected
  *   @history 2010-06-26 - Eric Hyer - Now uses MdiCubeViewport instead of
  *            CubeViewport.  Fixed some include issues (some still remain!)
  */
  class MeasureTool : public Tool {
      Q_OBJECT

    public:
      MeasureTool(QWidget *parent);
      void addTo(QMenu *menu);

    protected slots:
      virtual void rubberBandComplete();
      virtual void updateMeasure();
      virtual void mouseLeave();

    protected:
      //! Returns the menu name.
      QString menuName() const {
        return "&Options";
      };
      QWidget *createToolBarWidget(QStackedWidget *parent);
      QAction *toolPadAction(ToolPad *pad);
      void updateTool();
      void removeConnections(MdiCubeViewport *cvp);
      void enableRubberBandTool();

    private slots:

      void updateUnitsCombo();
      void updateDistEdit();

    private:
      QAction *p_action;          //!< Measure tool's action
      QLineEdit *p_distLineEdit;  //!< Distance line edit
      QComboBox *p_unitsComboBox; //!< Units selection
      int miComboUnit;            // Store the previous unit chosen

      void updateDist(MdiCubeViewport *cvp, int row);
      void updateRow(int row);
      void updateRows(int row);
      void initData(void);

      /**
       * Enum for all possible table columns.
       */
      enum TableColumnsMap {
        FeatureName, //!< Feature name.
        FeatureType, //!< Feature type.
        LatitudeLongitude, //!< Latitude Longitude.
        SampleLine, //!< Sample Line
        DistanceKm, //!< Distance in kilometers.
        DistanceM, //!< Distance in meters.
        DistancePix, //!< Distance in pixels.
        AngleDeg, //!< Angle in degrees.
        AngleRad, //!< Angle in radians.
        AreaKm, //!< Area in kilometers.
        AreaM, //!< Area in meters.
        AreaPix, //!< Area in pixels.
        Segments, //!< Segment lengths in kilometers.
        Path, //!< Filename path
        Filename, //!< Filename
        Notes //!< User input
      };

      /**
       * Enum for storing all the indexes.
       */
      enum TableColumnIndex {
        StartLatIndex = 2,//!< Starting latitude index
        StartLonIndex,//!< Starting longitude index
        EndLatIndex,//!< Ending latitude index
        EndLonIndex,//!< Ending longitude index
        StartSampIndex,//!< Starting sample index
        StartLineIndex,//!< Starting line index
        EndSampIndex,//!< Ending sample index
        EndLineIndex,//!< Ending line index
        DistanceKmIndex,//!< Distance in kilometers index
        DistanceMIndex,//!< Distance in meters index
        DistancePixIndex,//!< Distance in pixels index
        AngleDegIndex,//!< Angle in degrees index
        AngleRadIndex,//!< Angle in radians index
        AreaKmIndex,//!< Area in kilometers index
        AreaMIndex,//!< Area in meters index
        AreaPixIndex,//!< Area in pixels index
        SegmentsSumIndex,//!< Segment lengths in kilometers
        SegmentNumberIndex, //!< Segment number
        PathIndex,//!< Filename path index
        FilenameIndex//!< Filename index
      };

      double p_startSamp;//!< starting sample
      double p_endSamp;//!< ending sample
      double p_startLine;//!< starting line
      double p_endLine;//!< ending line
      double p_startLat;//!< starting latitude
      double p_endLat;//!< ending latitude
      double p_startLon;//!< starting longitude
      double p_endLon;//!< ending longitude
      double p_kmDist;//!< distance in kilometers
      double p_mDist;//!< distance in meters
      double p_pixDist;//!< distance in pixels
      double p_radAngle;//!< angle in radians
      double p_degAngle;//!< angle in degrees
      double p_kmArea;//!< area in kilometers
      double p_mArea;//!< area in meters
      double p_pixArea;//!< area in pixels

      QList<double> p_distanceSegments;
      QList<double>p_pixDistSegments;
      QList<double>p_startSampSegments;
      QList<double>p_endSampSegments;
      QList<double>p_startLineSegments;
      QList<double>p_endLineSegments;
      QList<double>p_startLatSegments;
      QList<double>p_endLatSegments;
      QList<double>p_startLonSegments;
      QList<double>p_endLonSegments;


      int p_numLinked;//!< number of linked viewports
      Isis::iString p_path;//!< filename path
      Isis::iString p_fname;//!< filename

      Qisis::TableMainWindow *p_tableWin;//!< table window
      RubberBandComboBox *p_rubberBand;//!< rubberband combo box
      QCheckBox *p_showAllSegments;
  };

};

#endif
