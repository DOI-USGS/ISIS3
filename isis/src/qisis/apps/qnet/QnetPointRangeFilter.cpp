#include <QGridLayout>
#include <QMessageBox>
#include "QnetPointRangeFilter.h"
#include "QnetNavTool.h"
#include "ControlNet.h"
#include "SerialNumberList.h"
//#include "geos.h"
#include "qnet.h"

#include "geos/geom/Coordinate.h"
#include "geos/geom/GeometryFactory.h"
#include "geos/geom/CoordinateSequenceFactory.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/Point.h"
#include "Latitude.h"
#include "Longitude.h"
#include "SurfacePoint.h"


using namespace Qisis::Qnet;
using namespace std;

namespace Qisis {
  /**
   * Contructor for the Point Range filter.  It creates the
   * Range filter window found in the navtool
   *
   * @param parent The parent widget for the point Range
   *               filter
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null.
   *
   */
  QnetPointRangeFilter::QnetPointRangeFilter(QWidget *parent) : QnetFilter(parent) {
    p_minlat = NULL;
    p_maxlat = NULL;
    p_minlon = NULL;
    p_maxlon = NULL;

    // Create the components for the filter window
    p_minlat = new QLineEdit();
    p_maxlat = new QLineEdit();
    p_minlon = new QLineEdit();
    p_maxlon = new QLineEdit();

    // Disable all range line edits since the lat/lon range is not selected
    //  p_minlat->setEnabled(false);
    //  p_maxlat->setEnabled(false);
    //  p_minlon->setEnabled(false);
    //  p_maxlon->setEnabled(false);

    // Create labels for the latitude range values
    QLabel *minlatLabel = new QLabel("Minimum Latitude");
    QLabel *maxlatLabel = new QLabel("Maximum Latitude");
    QLabel *minlonLabel = new QLabel("Minimum Longitude");
    QLabel *maxlonLabel = new QLabel("Maximum Longitude");
    QLabel *pad = new QLabel();

    // Create the layout and add the components to it
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(minlatLabel, 0, 0, 1, 1);
    gridLayout->addWidget(p_minlat, 0, 1, 1, 1);
    gridLayout->addWidget(maxlatLabel, 1, 0, 1, 1);
    gridLayout->addWidget(p_maxlat, 1, 1, 1, 1);
    gridLayout->addWidget(minlonLabel, 2, 0, 1, 1);
    gridLayout->addWidget(p_minlon, 2, 1, 1, 1);
    gridLayout->addWidget(maxlonLabel, 3, 0, 1, 1);
    gridLayout->addWidget(p_maxlon, 3, 1, 1, 1);
    gridLayout->addWidget(pad, 4, 0);
    gridLayout->setRowStretch(4, 50);
    this->setLayout(gridLayout);
  }

  /**
   * Filters a list of points for points that are of the selected
   * Range or in the given range. The filtered list will appear in
   * the navtools point list display.
   * @internal
   *   @history 2009-01-08 Jeannie Walldren - Modified to remove
   *                          new filter points from the existing
   *                          filtered list. Previously, a new
   *                          filtered list was created from the
   *                          entire control net each time.
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::"
   *                          since "using namespace std"
   *
   */
  void QnetPointRangeFilter::filter() {
    // Make sure there is a control net loaded
    if (g_controlNetwork == NULL) {
      QMessageBox::information((QWidget *)parent(),
          "Error", "No points to filter");
      return;
    }

    // Make sure all the values we need have been entered by the user
    if ((p_minlat->text() == "") || (p_maxlat->text() == "") ||
        (p_minlon->text() == "") || (p_maxlon->text() == "")) {
      QMessageBox::information((QWidget *)parent(),
          "Error", "All lat/lon range values must be entered");
      return;
    }
    else {
      // Get the user entered values for the range
      double minlat = p_minlat->text().toDouble();
      double maxlat = p_maxlat->text().toDouble();
      double minlon = p_minlon->text().toDouble();
      double maxlon = p_maxlon->text().toDouble();

      // Make sure the lat values are in order
      if (minlat > maxlat) {
        QString msg = "The minimum latitude value must be less than the maximum latitude value";
        QMessageBox::information((QWidget *)parent(), "Error", msg);
        return;
      }
      // Make sure the lon values are in order
      else if (minlon > maxlon) {
        QString msg = "The minimum longitude value must be less than the maximum longitude value";
        QMessageBox::information((QWidget *)parent(), "Error", msg);
        return;
      }

      // Set up a polygon with the range values the user entered

      // Create all the coordinates we will need
      vector<geos::geom::Coordinate> *coords = new vector<geos::geom::Coordinate>;
      geos::geom::Coordinate *c1 = new geos::geom::Coordinate(minlat, minlon);
      geos::geom::Coordinate *c2 = new geos::geom::Coordinate(maxlat, minlon);
      geos::geom::Coordinate *c3 = new geos::geom::Coordinate(maxlat, maxlon);
      geos::geom::Coordinate *c4 = new geos::geom::Coordinate(minlat, maxlon);
      geos::geom::Coordinate *c5 = new geos::geom::Coordinate(minlat, minlon);

      // create the coordinate sequence
      coords->push_back(*c1);
      coords->push_back(*c2);
      coords->push_back(*c3);
      coords->push_back(*c4);
      coords->push_back(*c5);
      geos::geom::GeometryFactory *factory = new geos::geom::GeometryFactory();
      const geos::geom::CoordinateSequenceFactory *csFact =
        factory->getCoordinateSequenceFactory();
      geos::geom::CoordinateSequence *seq = csFact->create(coords);

      // Create the polygon with the coordinate sequence
      geos::geom::LinearRing *ring = factory->createLinearRing(seq);
      geos::geom::Polygon *poly = factory->createPolygon(ring,
          new vector<geos::geom::Geometry *>);

      // Loop through each value of the filtered points list
      // checking to see if each point is in the polygon we created above
      // Loop in reverse order since removal list of elements affects index number
      for (int i = g_filteredPoints.size() - 1; i >= 0; i--) {
        // Get the current control point
        Isis::ControlPoint &cp = *(*g_controlNetwork)[g_filteredPoints[i]];

        // Create a new point
        const geos::geom::Coordinate *coord =
          new geos::geom::Coordinate(cp.GetSurfacePoint().GetLatitude(), cp.GetSurfacePoint().GetLongitude());
        geos::geom::Point *pt = factory->createPoint(*coord);

        // See if the point is in the polygon & add it if it is
        if (poly->contains(pt)) {
          continue;
        }
        else
          g_filteredPoints.removeAt(i);
      }
    }

    // Tell the navtool that a list has been filtered and it needs to update
    emit filteredListModified();
    return;
  }
}
