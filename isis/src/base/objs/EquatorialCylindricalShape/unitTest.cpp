/**
 * @file
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include <iostream>
#include <iomanip>

#include <QVector>

#include "Angle.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "Cube.h"
#include "EllipsoidShape.h"
#include "IException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Preference.h"
#include "Pvl.h"
#include "EquatorialCylindricalShape.h"
#include "SurfacePoint.h"
#include "Target.h"

using namespace std;
using namespace Isis;

int main() {
  Preference::Preferences(true);
  QString inputFile = "$mgs/testData/ab102401.cub";
  // string inputFile = "/work/projects/isis/latest/m00775/test/M123149061RE.lev1.cub";
  Cube cube;
  cube.open(inputFile);
  Camera *c = cube.getCamera();
  std::vector<Distance> radii = c->target()->radii();
  Pvl pvl = *cube.getLabel();
  Spice spi(pvl);
  Target targ(&spi, pvl);
  targ.setRadii(radii);

  cout << "Begin testing Dem Shape Model class...." << endl;

  EquatorialCylindricalShape shape(&targ, pvl);

  cout << "Shape name is " << shape.name() << endl;

  cout << "Testing method intersectSurface..." << endl; 
  cout << "  Do we have an intersection? " << shape.hasIntersection() << endl;
  cout << " Set a pixel in the image and check again." << endl;
  double line = 453.0;
  double sample = 534.0;
  // The next point goes with the LRO image /work/projects/isis/latest/m00775/test/M123149061RE.lev1.cub
  // double line = 1255.62;
  // double sample = 26112.5;
  c->SetImage(sample, line);
  std::vector<double> sB(3);
  c->instrumentPosition((double *) &sB[0]);
  std::vector<double> uB(3);
  c->sunPosition((double *) &uB[0]);
  std::vector<double> lookB(3);
  c->SpacecraftSurfaceVector((double *) &lookB[0]);
  /*
Find a point that fails in the DemShape intersect method and use instead. for a better test
Sample/Line = 534/453
surface normal = -0.623384, -0.698838, 0.350738
Local normal = -0.581842, -0.703663, 0.407823
  Phase                      = 40.787328112158
  Incidence                  = 85.341094499768
  Emission                   = 46.966269013795
  */
  if (!shape.intersectSurface(sB, lookB)) { 
      cout << "...  intersectSurface method failed" << endl;
      return -1;
  }
  cout << "  Do we have an intersection? " << shape.hasIntersection() << endl;
  SurfacePoint *sp = shape.surfaceIntersection();
  cout << "   surface point = (" << sp->GetX().kilometers() << ", " << 
    sp->GetY().kilometers() << ", " << sp->GetZ().kilometers() << endl;

 // TODO 
  // Test alternate constructors DemShape() and DemShape(target)


  cube.close();
}
