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
#include "DemShape.h"
#include "EllipsoidShape.h"
#include "IException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Preference.h"
#include "Pvl.h"
#include "SurfacePoint.h"
#include "Target.h"

using namespace std;
using namespace Isis;
 
 /**
   * This application tests the DemShape base class
   *
   *
   * @author 2010-10-11 Debbie A. Cook
   *
   * @internal
   *  @history
   */

class MyShape : public DemShape {
  public:
  MyShape(Target *target, Pvl &lab) : DemShape (target, lab) {
    setName("TestDem");
 }

   void testDemCube()  {
     // Cube *demCube();
     IString fileName = demCube()->getFileName();
     cout << "    Using dem cube file = " << demCube()->getFileName() << endl;
   }
};


int main() {
  Preference::Preferences(true);
  string inputFile = "$mgs/testData/ab102401.cub";
  Cube cube;
  cube.open(inputFile);
  Camera *c = cube.getCamera();
  std::vector<Distance> radii = c->target()->radii();
  Pvl pvl = *cube.getLabel();
  Spice spi(pvl);
  Target targ(&spi, pvl);
  targ.setRadii(radii);

  cout << "Begin testing Dem Shape Model class...." << endl;

  cout << endl << "  Testing constructors..." << endl;
  DemShape shape(&targ, pvl);
  DemShape defaultShape;

  cout << "    Shape name is " << shape.name() << endl;
  cout << "    Shape name is " << defaultShape.name() << endl;

  cout << endl << "  Testing method intersectSurface..." << endl; 
  cout << "    Do we have an intersection? " << shape.hasIntersection() << endl;
  cout << "    Set a pixel in the image and check again." << endl;
  double line = 453.0;
  double sample = 534.0;
  c->SetImage(sample, line);
  std::vector<double> sB(3);
  c->instrumentPosition((double *) &sB[0]);
  std::vector<double> uB(3);
  c->sunPosition((double *) &uB[0]);
  std::vector<double> lookB(3);
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
  cout << "    surface point = (" << sp->GetX().kilometers() << ", " << 
    sp->GetY().kilometers() << ", " << sp->GetZ().kilometers() << endl;

  cout << endl << "  Testing class method calculateLocalNormal with correction for inward pointing vector" << endl;
  QVector<double *>  neighborPoints(4);

// neighbor 0 -2123.07 -2380.15 1195.48
// neighbor 1 -2123.66 -2380.59 1193.88
// neighbor 2 -2123.83 -2379.95 1194.73
// neighbor 3 -2122.89 -2380.79 1194.63
  for (int i = 0; i < neighborPoints.size(); i ++)
    neighborPoints[i] = new double[3];

  neighborPoints[0][0]  = -2123.07;
  neighborPoints[0][1]  = -2380.15;
  neighborPoints[0][2]  = 1195.48;
  neighborPoints[1][0]  = -2123.66;
  neighborPoints[1][1]  = -2380.59;
  neighborPoints[1][2]  = 1193.88;
  neighborPoints[2][0]  = -2123.83;
  neighborPoints[2][1]  = -2379.95;
  neighborPoints[2][2]  = 1194.73;
  neighborPoints[3][0]  = -2122.89;
  neighborPoints[3][1]  = -2380.79;
  neighborPoints[3][2]  = 1194.63;

  shape.calculateLocalNormal(neighborPoints);
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

  cout << endl << "  Testing localRadius method with good lat/lon values..." << endl;
  cout  << "    Local radius = " << shape.localRadius(Latitude(20.532461495381, Angle::Degrees),
                                                  Longitude(228.26609149754, Angle::Degrees)).kilometers() << endl;
  // Mars radii = 3397.      3397.         3375.

  cout << endl << "  Testing localRadius method with bad lat/lon values..." << endl;
  cout  << "    Local radius = " << shape.localRadius(Latitude(Null, Angle::Degrees),
                                                  Longitude(228.26609149754, Angle::Degrees)).kilometers() << endl;

  cout << endl << "  Testing setHasIntersection method" << endl;
  shape.setHasIntersection(false);
  cout << "    Do we have an intersection? " << shape.hasIntersection() << endl;

  cout << endl << "  Testing setSurfacePoint method ..." << endl;
  shape.setSurfacePoint(*sp);
  cout << "    Do we have an intersection? " << shape.hasIntersection() << endl;
  cout << "    surface point = (" << sp->GetX().kilometers() << ", " << 
    sp->GetY().kilometers() << ", " << sp->GetZ().kilometers() << endl;

  cout << endl << "  Testing demScale method..." << endl;
  cout << "    The map scale of the dem file is " << shape.demScale() << " pixels/degree" << endl;

  cout << endl << "  Testing protected methods demCube ..." << endl;

  MyShape shape2(&targ, pvl);
  shape2.testDemCube();

  // Test pvl with ElevationModel instead of ShapeModel
  cout << endl << "  Testing input of dem file with keyword ElevationModel" << endl;
  try {
    Pvl elPvl;
    PvlGroup kernels = pvl.FindGroup("Kernels", Pvl::Traverse);
    IString demCubeFile;
    demCubeFile = (std::string) kernels["ShapeModel"];
    kernels.DeleteKeyword("ShapeModel");
    PvlKeyword shapeKey("ElevationModel", demCubeFile);
    kernels.AddKeyword(shapeKey);
    elPvl.AddGroup(kernels);
    DemShape elShape (&targ, elPvl);
  }
  catch(Isis::IException &e) {
    e.print();
  }


  // Test calculateLocalNormal with dotprod > 0
  neighborPoints[2][0]  = -2123.07;
  neighborPoints[2][1]  = -2380.15;
  neighborPoints[2][2]  = 1195.48;
  neighborPoints[3][0]  = 2123.66;
  neighborPoints[3][1]  = 2380.59;
  neighborPoints[3][2]  = 1193.88;
  cout << endl << "  Testing method calculateLocalNormal with vector pointing outward" << endl;
  shape.calculateLocalNormal(neighborPoints);
  myNormal = shape.normal();
  cout << "    local normal = (" << myNormal[0] << ", " << myNormal[1] << ", " << myNormal[2] << endl;

  // Test calculateLocalNormal with magnitude = 0
  try {
  neighborPoints[3][0]  = -2123.66;
  neighborPoints[3][1]  = -2380.59;
  cout << endl << "  Testing method calculateLocalNormal with magnitude = 0" << endl;
  shape.calculateLocalNormal(neighborPoints);
  myNormal = shape.normal();
  cout << "    local normal = (" << myNormal[0] << ", " << myNormal[1] << ", " << myNormal[2] << endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }


  cube.close();
}
