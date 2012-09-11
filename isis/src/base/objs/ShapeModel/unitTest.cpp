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

class MyShape : public ShapeModel {
  public:
  MyShape(Target *target, Pvl &lab) : ShapeModel (target, lab) {
    setName("Test");
 }

   bool intersectSurface(std::vector<double> observerPos,
                                    std::vector<double> lookDirection)  {
      cout << "intersectSurface called with observer position = " << 
        observerPos[0] << ", " << observerPos[1] << ", " <<
        observerPos[2] << endl << "                             lookDirection = " <<
        lookDirection[0] << ", " <<  lookDirection[1] << ", " << lookDirection[2] << endl;
      SpiceDouble intersectionPoint[3] = {-2123.362258286, -2380.3717812236, 1194.6783966636};
      
      surfaceIntersection()->FromNaifArray(intersectionPoint);
      setHasIntersection(true);
      return true;
   }

  virtual void calculateDefaultNormal()  {
      calculateSurfaceNormal();
   }

   virtual void calculateLocalNormal(QVector<double *> cornerNeighborPoints) {
     std::vector<double> myNormal(3);
      myNormal[0] = -0.581842;
      myNormal[1] = -0.703663;
      myNormal[2] = 0.407823;
      setNormal(myNormal);
      setHasNormal(true);
   }
      
  virtual void calculateSurfaceNormal() {
     std::vector<double> myNormal(3);
      myNormal[0] =  -0.623384;
      myNormal[1] = -0.698838;
      myNormal[2] = 0.350738;
      setNormal(myNormal);
      setHasNormal(true);
  }

  Distance localRadius(const Latitude &lat, const Longitude &lon) {
    double a = 6378.14;
    double b = 6378.14;
    double c = 6356.75;
    
    double rlat = lat.degrees();
    double rlon = lon.degrees();
    
    double xyradius = a * b / sqrt(pow(b * cos(rlon), 2) +
                      pow(a * sin(rlon), 2));
    
    const double &radius = xyradius * c / sqrt(pow(c * cos(rlat), 2) +
                           pow(xyradius * sin(rlat), 2));

    return Distance(radius, Distance::Kilometers);
  }

  bool normalStatus() {
    return hasNormal();
  }

};

int main() {
  Preference::Preferences(true);
  string inputFile = "$mgs/testData/ab102401.cub";
  // string inputFile = "ab102401.cub";
  Cube cube;
  cube.open(inputFile);
  Camera *c = cube.getCamera();
  std::vector<Distance> radii = c->target()->radii();
  Pvl pvl = *cube.getLabel();
  Spice spi(pvl);
  Target targ(&spi, pvl);
  targ.setRadii(radii);

  cout << "Begin testing Shape Model base class...." << endl;

  MyShape shape(&targ, pvl);

  cout << "Shape name is " << shape.name() << endl;
  cout << "  Do we have an intersection? " << shape.hasIntersection() << endl;
  cout << " Set a pixel in the image and check again." << endl;
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
  cout << "Testing pure virtual method intersectSurface..." << endl; 
  if (!shape.intersectSurface(sB, lookB)) { 
      cout << "...  intersectSurface method failed" << endl;
      return -1;
  }
  cout << "  Do we have an intersection? " << shape.hasIntersection() << endl;
  SurfacePoint *sp = shape.surfaceIntersection();
  cout << "   surface point = (" << sp->GetX().kilometers() << ", " << 
    sp->GetY().kilometers() << ", " << sp->GetZ().kilometers() << endl;

  cout << "Testing class method calculateLocalNormal..." << endl;
  cout << "  Do we have a normal? " << shape.normalStatus() << endl;
  QVector<double *>  notUsed(4);

  for (int i = 0; i < notUsed.size(); i ++)
      notUsed[i] = new double[3];

  shape.calculateLocalNormal(notUsed);
  cout << "  Do we have a normal? " << shape.normalStatus() << endl;
  vector<double> myNormal(3);
  myNormal = shape.normal();
  cout << "  local normal = (" << myNormal[0] << ", " << myNormal[1] << ", " << myNormal[2] << endl;

  cout << "Testing class method calculateSurfaceNormal..." << endl;
  shape.calculateSurfaceNormal(); 
  myNormal = shape.normal();
  cout << "  surface normal = (" << myNormal[0] << ", " << myNormal[1] << ", " << myNormal[2] << endl;

  cout << "Testing class method calculateDefaultNormal..." << endl;
  shape.calculateDefaultNormal();
  myNormal = shape.normal();
  cout << "  default normal = (" << myNormal[0] << ", " << myNormal[1] << ", " << myNormal[2] << endl;

  cout << "Testing photometric angle calculations..." << endl;
  cout << "  Emission angle = " << shape.emissionAngle(sB);
  cout << " Incidence angle = " << shape.incidenceAngle(uB);
  cout << " Phase angle = "  << shape.phaseAngle(sB, uB);
  cout << endl;

  cout << "Testing localRadius method ..." << endl;
  cout  << "Local radius = " << shape.localRadius(Latitude(20.532461495381, Angle::Degrees),
                                                  Longitude(228.26609149754, Angle::Degrees)).kilometers();
  // Mars radii = 3397.      3397.         3375.

  cout << "Testing setHasIntersection method" << endl;
  shape.setHasIntersection(false);
  cout << "  Do we have an intersection? " << shape.hasIntersection() << endl;

  cout << "Testing setSurfacePoint method ..." << endl;
  shape.setSurfacePoint(*sp);
  cout << "   Do we have an intersection? " << shape.hasIntersection() << endl;
  cout << "   surface point = (" << sp->GetX().kilometers() << ", " << 
    sp->GetY().kilometers() << ", " << sp->GetZ().kilometers() << ")" << endl;

  // TODO 
  // Test alternate constructors ShapeModel() and ShapeModel(target)
  cube.close();
}
