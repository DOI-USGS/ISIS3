#include "QnetPointDistanceFilter.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMessageBox>

#include "Camera.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Distance.h"
#include "Latitude.h"
#include "Longitude.h"
#include "QnetNavTool.h"
#include "SerialNumberList.h"
#include "SurfacePoint.h"

namespace Isis {
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
   *   @history 2011-03-17 Tracie Sucharski - Changed text on filter gui.
   *
   */
  QnetPointDistanceFilter::QnetPointDistanceFilter(QnetNavTool *navTool,
                                                   QWidget *parent) : QnetFilter(navTool, parent) {
    m_lineEdit = NULL;

    // Create the labels and widgets to be added to the main window
    QLabel *label = new QLabel("Filter points that are within given distance of another point.");
    QLabel *lessThan = new QLabel("Distance to another point is less than");
    m_lineEdit = new QLineEdit;
    QLabel *meters = new QLabel("meters");
    QLabel *pad = new QLabel;

    // Create the layout and add the widgets to the window
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(label, 0, 0, 1, 2);
    gridLayout->addWidget(lessThan, 1, 0);
    gridLayout->addWidget(m_lineEdit, 1, 1);
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
   *   @history 2011-03-17 Tracie Sucharski - Use surface point from camera
   *                          and updated for changes to SurfacePoint.
   */
  void QnetPointDistanceFilter::filter() {
    // Make sure we have a control network to filter through
    if (controlNet() == NULL) {
      QMessageBox::information((QWidget *)parent(),
          "Error", "No points to filter");
      return;
    }

    // Make sure the user entered a filtering value
    if (m_lineEdit->text() == "") {
      QMessageBox::information((QWidget *)parent(),
          "Error", "Distance value must be entered");
      return;
    }
    // Get the user entered value for filtering
    double userEntered = m_lineEdit->text().toDouble();

    // create temporary QList to contain new filtered images
    QList <int> temp;
    // Loop through each value of the filtered points list
    // Loop in reverse order for consistency with other filter methods
    for (int i = filteredPoints().size() - 1; i >= 0; i--) {
      ControlPoint &cp1 = *(*controlNet())[filteredPoints()[i]];
      // Get necessary info from the control point for later use
      //  First check if an adjusted point from jigsaw exists.  If not, use
      //  the apriori values.
      
      SurfacePoint sp1 = cp1.GetBestSurfacePoint();

      // If no lat/lon for this point, use lat/lon of first measure
      if (!sp1.Valid()) {
        Camera *cam1;
        ControlMeasure cm1;

        cm1 = *cp1.GetRefMeasure();

        int camIndex1 = serialNumberList()->serialNumberIndex(cm1.GetCubeSerialNumber());
        cam1 = controlNet()->Camera(camIndex1);
        cam1->SetImage(cm1.GetSample(), cm1.GetLine());
        sp1 = cam1->GetSurfacePoint();
      }
      // Loop through each control point, comparing it to the initial point
      // from the filtered list
      for (int j = 0; j < controlNet()->GetNumPoints(); j++) {
        if (j == filteredPoints()[i]) {
          // cp1 = cp2, go to next value of j
          continue;
        }
        ControlPoint cp2 = *(*controlNet())[j];
        SurfacePoint sp2 = cp2.GetBestSurfacePoint();

        // If no lat/lon for this point, use lat/lon of first measure
        if (!sp2.Valid()) {
          Camera *cam2;
          ControlMeasure cm2;
          cm2 = *cp2.GetRefMeasure();
          int camIndex2 = serialNumberList()->serialNumberIndex(cm2.GetCubeSerialNumber());
          cam2 = controlNet()->Camera(camIndex2);
          cam2->SetImage(cm2.GetSample(), cm2.GetLine());
          sp2 = cam2->GetSurfacePoint();
        }
        // Get the distance from the camera class
        double dist = sp1.GetDistanceToPoint(sp2,sp1.GetLocalRadius()).meters();

        // If the distance found is less than the input number, add the
        // control points' indices to the new filtered points list
        if (dist < userEntered) {
          if (!temp.contains(filteredPoints()[i])) {
            temp.push_back(filteredPoints()[i]);
          }
          break;
        }
      }
    }

    // Sort QList of filtered points before displaying list to user
    std::sort(temp.begin(), temp.end());
    // replace existing filter list with this one
    filteredPoints() = temp;

    // Tell the nav tool that a list has been filtered and needs to be updated
    emit filteredListModified();
    return;
  }
}
