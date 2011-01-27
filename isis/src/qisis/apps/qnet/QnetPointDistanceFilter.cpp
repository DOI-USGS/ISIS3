#include "QnetPointDistanceFilter.h"

#include <QGridLayout>
#include <QList>
#include <QMessageBox>

#include "Camera.h"
#include "ControlNet.h"
#include "Distance.h"
#include "Latitude.h"
#include "Longitude.h"
#include "qnet.h"
#include "QnetNavTool.h"
#include "SerialNumberList.h"
#include "SurfacePoint.h"

using namespace Qisis::Qnet;

namespace Qisis {
  /**
   * Contructor for the Point Distance filter.  It creates the
   * Distance filter window found in the navtool
   *
   * @param parent The parent widget for the point distance
   *               filter
   * @internal
   *   @history 2008-11-26 Jeannie Walldren - Clarified label for
   *                          distance filter value.
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null.
   *
   *
   */
  QnetPointDistanceFilter::QnetPointDistanceFilter(QWidget *parent) : QnetFilter(parent) {
    p_lineEdit = NULL;

    // Create the labels and widgets to be added to the main window
    QLabel *label = new QLabel("Filter points that are within given distance of some other point.");
    QLabel *lessThan = new QLabel("Minimum distance is less than");
    p_lineEdit = new QLineEdit();
    QLabel *meters = new QLabel("meters");
    QLabel *pad = new QLabel();

    // Create the layout and add the widgets to the window
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(label, 0, 0, 1, 2);
    gridLayout->addWidget(lessThan, 1, 0);
    gridLayout->addWidget(p_lineEdit, 1, 1);
    gridLayout->addWidget(meters, 1, 2);
    gridLayout->addWidget(pad, 2, 0);
    gridLayout->setRowStretch(2, 50);
    this->setLayout(gridLayout);
  }

  /**
   * Filters a list of points for points that are less than the user entered
   * distance from another point in the control net.  The filtered list will
   * appear in the navtools point list display.
   * @internal
   *   @history 2008-11-26 Jeannie Walldren - Modified code to
   *                          handle case in which the lat/lon of
   *                          the point is Null. In this event,
   *                          the Camera class will be used to
   *                          determine lat/lon/rad for the
   *                          reference measure or for the first
   *                          measure. Changed variable names for
   *                          clarity.  Adjusted inner "for" loop
   *                          to reduce number of iterations.
   *   @history 2009-01-08 Jeannie Walldren - Modified to replace
   *                          existing filtered list with a subset
   *                          of that list. Previously, a new
   *                          filtered list was created from the
   *                          entire control net each time.
   */
  void QnetPointDistanceFilter::filter() {
    // Make sure we have a control network to filter through
    if (g_controlNetwork == NULL) {
      QMessageBox::information((QWidget *)parent(),
          "Error", "No points to filter");
      return;
    }

    // Make sure the user entered a filtering value
    if (p_lineEdit->text() == "") {
      QMessageBox::information((QWidget *)parent(),
          "Error", "Distance value must be entered");
      return;
    }
    // Get the user entered value for filtering
    int userEntered = p_lineEdit->text().toInt();

    // create temporary QList to contain new filtered images
    QList <int> temp;
    // Loop through each value of the filtered points list
    // Loop in reverse order for consistency with other filter methods
    for (int i = g_filteredPoints.size() - 1; i >= 0; i--) {
      Isis::ControlPoint &cp1 = *(*g_controlNetwork)[g_filteredPoints[i]];
      // Get necessary info from the control point for later use
      double rad = cp1.GetSurfacePoint().GetLocalRadius();
      double lat1 = cp1.GetSurfacePoint().GetLatitude();
      double lon1 = cp1.GetSurfacePoint().GetLongitude();

      // If no lat/lon for this point, use lat/lon of first measure
      if ((lat1 == Isis::Null) || (lon1 == Isis::Null)) {
        Isis::Camera *cam1;
        Isis::ControlMeasure cm1;
        // first try to get info from reference measure, if one exists
        if (cp1.HasReference()) {
          cm1 = *cp1.GetReferenceMeasure();
        }
        // if no reference measure exists, use the first control measure of the point
        else
          cm1 = *cp1[0];
        int camIndex1 = g_serialNumberList->SerialNumberIndex(cm1.GetCubeSerialNumber());
        cam1 = g_controlNetwork->Camera(camIndex1);
        cam1->SetImage(cm1.GetSample(), cm1.GetLine());
        rad = cam1->LocalRadius();
        lat1 = cam1->UniversalLatitude();
        lon1 = cam1->UniversalLongitude();
      }
      // Loop through each control point, comparing it to the initial point
      // from the filtered list
      for (int j = 0; j < g_controlNetwork->GetNumPoints(); j++) {
        if (j == g_filteredPoints[i]) {
          // cp1 = cp2, go to next value of j
          continue;
        }
        Isis::ControlPoint cp2 = *(*g_controlNetwork)[j];
        double lat2 = cp2.GetSurfacePoint().GetLatitude();
        double lon2 = cp2.GetSurfacePoint().GetLongitude();

        // If no lat/lon for this point, use lat/lon of first measure
        if ((lat2 == Isis::Null) || (lon2 == Isis::Null)) {
          Isis::Camera *cam2;
          Isis::ControlMeasure cm2;
          // first try to get info from reference measure, if one exists
          if (cp2.HasReference()) {
            cm2 = *cp2.GetReferenceMeasure();
          }
          // if no reference measure exists, use the first control measure of the point
          else
            cm2 = *cp2[0];
          int camIndex2 = g_serialNumberList->SerialNumberIndex(cm2.GetCubeSerialNumber());
          cam2 = g_controlNetwork->Camera(camIndex2);
          cam2->SetImage(cm2.GetSample(), cm2.GetLine());
          lat2 = cam2->UniversalLatitude();
          lon2 = cam2->UniversalLongitude();
        }
        // Get the distance from the camera class
        double dist = Isis::Camera::Distance(lat1, lon1, lat2, lon2, rad);
        // If the distance found is less than the input number, add the
        // control points' indices to the new filtered points list
        if (dist < userEntered) {
          if (!temp.contains(g_filteredPoints[i])) {
            temp.push_back(g_filteredPoints[i]);
          }
          break;
        }
      }
    }

    // Sort QList of filtered points before displaying list to user
    qSort(temp.begin(), temp.end());
    // replace existing filter list with this one
    g_filteredPoints = temp;

    // Tell the nav tool that a list has been filtered and needs to be updated
    emit filteredListModified();
    return;
  }
}
