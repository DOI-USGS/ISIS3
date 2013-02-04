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
#include "ShapeModel.h"
#include "SurfacePoint.h"
#include "Target.h"

using namespace std;
using namespace Isis;


int main() {
  Preference::Preferences(true);
  QString inputFile = "$mgs/testData/ab102401.cub";
  Cube cube;
  cube.open(inputFile);
  Camera *c = cube.camera();
  std::vector<Distance> radii = c->target()->radii();
  Pvl pvl = *cube.label();
  Spice spi(pvl);
  Target targ(&spi, pvl);
  targ.setRadii(radii);

  cout << "Begin testing Ellipsoid Shape Model class...." << endl;

  cout << endl << "  Testing constructors..."  << endl;
  EllipsoidShape shape(&targ, pvl);
  EllipsoidShape shape2(&targ);
  EllipsoidShape shape3;

  cout << "    Shape1  name is " << shape.name() << endl;
  cout << "    Shape2  name is " << shape2.name() << endl;
  cout << "    Shape3  name is " << shape3.name() << endl;

  std::vector<double> sB(3);
  sB[0] = -2399.54;
  sB[1] = -2374.03;
  sB[2] = 1277.68;
  std::vector<double> lookB(3, 1.);
  lookB[0] = -1.;
  cout << endl << "  Testing method intersectSurface with failure..." << endl; 
  cout << "    Do we have an intersection? " << shape.hasIntersection() << endl;
  shape.intersectSurface(sB, lookB);
  if (!shape.hasIntersection()) cout << "    Intersection failed " << endl;

  cout << endl << "Testing method intersectSurface..." << endl; 
  cout << "    Do we have an intersection? " << shape.hasIntersection() << endl;
  cout << "   Set a pixel in the image and check again." << endl;
  double line = 453.0;
  double sample = 534.0;
  c->SetImage(sample, line);
  c->instrumentPosition((double *) &sB[0]);
  std::vector<double> uB(3);
  c->sunPosition((double *) &uB[0]);
  c->SpacecraftSurfaceVector((double *) &lookB[0]);
  /*
Sample/Line = 534/453
surface normal = -0.623384, -0.698838, 0.350738
Local normal = -0.581842, -0.703663, 0.407823
  Phase                      = 40.787328112158
  Incidence                  = 85.341094499768
  Emission                   = 46.966269013795
  */
  if (!shape.intersectSurface(sB, lookB)) { 
      cout << "    ...  intersectSurface method failed" << endl;
      return -1;
  }
  cout << "    Do we have an intersection? " << shape.hasIntersection() << endl;
  SurfacePoint *sp = shape.surfaceIntersection();
  cout << "     surface point = (" << sp->GetX().kilometers() << ", " << 
    sp->GetY().kilometers() << ", " << sp->GetZ().kilometers() << endl;

  cout << endl << "  Testing class method calculateLocalNormal..." << endl;
  QVector<double *>  notUsed(4);

  for (int i = 0; i < notUsed.size(); i ++)
      notUsed[i] = new double[3];

  shape.calculateLocalNormal(notUsed);
  vector<double> myNormal(3);
  myNormal = shape.normal();
  cout << "    local normal = (" << myNormal[0] << ", " << myNormal[1] << ", " << myNormal[2] << endl;

  cout << endl << "  Testing class method calculateSurfaceNormal..." << endl;
  shape.calculateSurfaceNormal(); 
  myNormal = shape.normal();
  cout << "    surface normal = (" << myNormal[0] << ", " << myNormal[1] << ", " << myNormal[2] << endl;

  cout << endl << "  Testing class method calculateDefaultNormal..." << endl;
  shape.calculateDefaultNormal();
  myNormal = shape.normal();
  cout << "    default normal = (" << myNormal[0] << ", " << myNormal[1] << ", " << myNormal[2] << endl;

  cout << endl << "  Testing localRadius method ..." << endl;
  cout  << "    Local radius = " << shape.localRadius(Latitude(20.532461495381, Angle::Degrees),
                                                  Longitude(228.26609149754, Angle::Degrees)).kilometers() << endl;
  // Mars radii = 3397.      3397.         3375.

  cout << endl << "  Testing setHasIntersection method" << endl;
  shape.setHasIntersection(false);
  cout << "    Do we have an intersection? " << shape.hasIntersection() << endl;

  cout << endl << "  Testing setSurfacePoint method ..." << endl;
  shape.setSurfacePoint(*sp);
  cout << "     Do we have an intersection? " << shape.hasIntersection() << endl;
  cout << "     surface point = (" << sp->GetX().kilometers() << ", " << 
    sp->GetY().kilometers() << ", " << sp->GetZ().kilometers() << endl;

  cube.close();
}
