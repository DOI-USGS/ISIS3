#ifndef SunShadowTool_h
#define SunShadowTool_h

#include "Tool.h"

#include <QAction>

#include "IString.h"
#include "TableMainWindow.h"

class QComboBox;
class QDialog;
class QLineEdit;
class QMenu;
class QString;

namespace Isis {
  class Angle;
  class Distance;
  class MdiCubeViewport;
  class SurfacePoint;

  /**
  * @brief Tool for measuring shadow heights
  *
  * @ingroup Visualization Tools
  *
  * @author 2012-03-12 Steven Lambright
  *
  * @internal
  *  @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
  *                         coding standards. References #972.
  */
  class SunShadowTool : public Tool {
      Q_OBJECT

    public:
      SunShadowTool(QWidget *parent);
      void addTo(QMenu *menu);

      void paintViewport(MdiCubeViewport *vp, QPainter *painter);

    protected slots:
      virtual void mouseMove(QPoint p);
      virtual void mouseButtonPress(QPoint p, Qt::MouseButton s);
      virtual void mouseButtonRelease(QPoint p, Qt::MouseButton s);

    protected:
      QWidget *createToolBarWidget(QStackedWidget *parent);
      QAction *toolPadAction(ToolPad *pad);

      void updateTool();

    private slots:
      void updateShadowHeightEdit();
      void reinitialize();

    private:
      void addRow();
      void recalculateShadowHeight();
      void updateRow(int row);

    private:
      //! This displays the currently calculated height of the measured shadow
      QLineEdit *m_shadowHeightLineEdit;
      //! User can choose the height line edit's units (M or KM)
      QComboBox *m_unitsComboBox;
      //! Check box to enable/disable confining tracking to sun direction
      QCheckBox *m_drawInSunDirection;

      /**
       * Enum for storing all the indexes.
       */
      enum TableColumnIndex {
        //! Start lat table column index
        StartLatIndex = 2,
        //! Start lon table column index
        StartLonIndex,
        //! End lat table column index
        EndLatIndex,
        //! End lon table column index
        EndLonIndex,
        //! Start cube sample table column index
        StartSampIndex,
        //! Start cube line table column index
        StartLineIndex,
        //! End cube sample table column index
        EndSampIndex,
        //! End cube line table column index
        EndLineIndex,
        //! Shadow length in kilometers table column index
        ShadowLengthKmIndex,
        //! Shadow length in meters table column index
        ShadowLengthMIndex,
        //! Shadow height in kilometers table column index
        ShadowHeightKmIndex,
        //! Shadow height in meters table column index
        ShadowHeightMIndex,
        //! Incidence angle in degrees table column index
        IncidenceAngleDegreesIndex,
        //! Incidence angle in radians table column index
        IncidenceAngleRadiansIndex,
        //! Cube file path table column index
        PathIndex,
        //! Cube file name table column index
        FileNameIndex
      };

      //! Start sample of the measurement
      double m_startSamp;
      //! Start line of the measurement
      double m_startLine;
      //! Start ground coordinate of the measurement
      SurfacePoint *m_startSurfacePoint;

      //! End sample of the measurement
      double m_endSamp;
      //! End line of the measurement
      double m_endLine;
      //! End ground coordinate of the measurement
      SurfacePoint *m_endSurfacePoint;

      //! Incidence angle from the normal at the end point
      Angle *m_incidenceAngle;

      //! Calculated shadow height
      Distance *m_shadowHeight;
      //! Calculated shadow length
      Distance *m_shadowLength;

      //! Current cube file path
      QString m_path;
      //! Current cube file name
      QString m_fileName;

      //! Table window for displaying all of the table information
      TableMainWindow *m_tableWin;
      //! True if this tool is enabled (capable of working). Requires a camera.
      bool m_enabled;
      /**
       * True if currently tracking the user's mouse position and calculating
       *   values on every mouse move.
       */
      bool m_tracking;
      //! The angle that we want mouse tracking to be in.
      Angle *m_trackingAngle;
  };

};

#endif
