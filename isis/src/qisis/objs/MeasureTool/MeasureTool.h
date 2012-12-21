#ifndef Qisis_MeasureTool_h
#define Qisis_MeasureTool_h

#include "Tool.h"

// FIXME: remove these includes
#include <QAction>
#include "RubberBandComboBox.h"
#include "TableMainWindow.h"
#include "IString.h"


class QComboBox;
class QDialog;
class QMenu;
class QLineEdit;
class QString;

namespace Isis {
  class MdiCubeViewport;

  /**
  * @brief Tool for measuring distances
  *
  * @ingroup Visualization Tools
  *
  * @author ????-??-?? Unknown
  *
  * @internal
  *   @history 2007-11-19 Stacy Alley added the capability of the qisis windows
  *                           to remember size and location
  *   @history 2007-11-29 Stacy Alley replaced all the table stuff with the new
  *                           TableMainWindow class.
  *   @history 2008-08-18 Christopher Austin Upgraded to geos3.0.0
  *   @history 2008-09-26 Steven Lambright Added Segmented line
  *   @history 2009-02-12 Steven Lambright Fixed bug where measure tool would
  *                           not measure pixels for non-camera, non-projection
  *                           cubes.
  *   @history 2010-02-17 Sharmila Prasad Fixed bug where the distance was
  *                           calculated twice for a cube with both camera and
  *                           projection. Also made changes to save the most
  *                           recent selection (km,m,pixels) when different tool
  *                           is selected
  *   @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport instead of
  *                           CubeViewport.  Fixed some include issues (some
  *                           still remain!)
  *   @history 2011-09-20 Steven Lambright - Added some abstraction, fixed
  *                           problems with segmented line and rectangle.
  *                           Fixes #218.
   *  @history 2011-11-01 Steven Lambright - Fixed possible seg fault.
   *                          References #205.
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
      QAction *m_action;          //!< Measure tool's action
      QLineEdit *m_distLineEdit;  //!< Distance line edit
      QComboBox *m_unitsComboBox; //!< Units selection
      int miComboUnit;            // Store the previous unit chosen

      void addRow();
      void updateDist(MdiCubeViewport *cvp, int row);
      void setDistances(MdiCubeViewport *cvp, QPoint lineStart, QPoint lineEnd);
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
        Path, //!< FileName path
        FileName, //!< FileName
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
        PathIndex,//!< FileName path index
        FileNameIndex//!< FileName index
      };

      double m_startSamp;//!< starting sample
      double m_endSamp;//!< ending sample
      double m_startLine;//!< starting line
      double m_endLine;//!< ending line
      double m_startLat;//!< starting latitude
      double m_endLat;//!< ending latitude
      double m_startLon;//!< starting longitude
      double m_endLon;//!< ending longitude
      double m_kmDist;//!< distance in kilometers
      double m_mDist;//!< distance in meters
      double m_pixDist;//!< distance in pixels
      double m_radAngle;//!< angle in radians
      double m_degAngle;//!< angle in degrees
      double m_kmArea;//!< area in kilometers
      double m_mArea;//!< area in meters
      double m_pixArea;//!< area in pixels

      QList<double> m_distanceSegments;
      QList<double> m_pixDistSegments;
      QList<double> m_startSampSegments;
      QList<double> m_endSampSegments;
      QList<double> m_startLineSegments;
      QList<double> m_endLineSegments;
      QList<double> m_startLatSegments;
      QList<double> m_endLatSegments;
      QList<double> m_startLonSegments;
      QList<double> m_endLonSegments;


      int m_numLinked;//!< number of linked viewports
      QString m_path;//!< filename path
      QString m_fname;//!< filename

      TableMainWindow *m_tableWin;//!< table window
      RubberBandComboBox *m_rubberBand;//!< rubberband combo box
      QCheckBox *m_showAllSegments;
  };

};

#endif
