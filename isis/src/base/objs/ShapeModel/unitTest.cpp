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
#include "Distance.h"
#include "IException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Preference.h"
#include "Pvl.h"
#include "ShapeModel.h"
#include "SurfacePoint.h"
#include "Target.h"

  /**
   * This application tests the ShapeModel class
   *
   * @author 2010-10-11 Debbie A. Cook
   *
   * @internal
   *  @history
   */

using namespace std;
using namespace Isis;

class MyShape : public ShapeModel {
  public:
  MyShape(Target *target, Pvl &lab) : ShapeModel (target, lab) {
    setName(IString("Test"));
  }

   bool intersectSurface(std::vector<double> observerPos,
                                    std::vector<double> lookDirection)  {
      cout << "    intersectSurface called with observer position = " << 
        observerPos[0] << ", " << observerPos[1] << ", " <<
        observerPos[2] << endl << "                                 lookDirection = " <<
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

  void setSmallNormal() {
    std::vector<double> normal(3, 10.);
      setNormal(normal);
      setHasNormal(true);
  }

  void setBigNormal() {
    std::vector<double> normal(3, -10.);
      setNormal(normal);
      setHasNormal(true);
  }

  double resolution() {
    return ShapeModel::resolution();
  }
};



class MyEllipse : public ShapeModel {
  public:
  MyEllipse(Target *target) : ShapeModel (target) {
    setName(IString("Ellipsoid"));
  }

  MyEllipse() : ShapeModel() {
    setName(IString("DefaultConstructor"));
  }

  bool intersectSurface(std::vector<double> observerPos,
                                    std::vector<double> lookDirection)  {
     cout << "    intersectSurface called with observer position = " << 
       observerPos[0] << ", " << observerPos[1] << ", " <<
       observerPos[2] << endl << "                                 lookDirection = " <<
       lookDirection[0] << ", " <<  lookDirection[1] << ", " << lookDirection[2] << endl;
     return (intersectEllipsoid(observerPos, lookDirection));
  }

  virtual void calculateLocalNormal(QVector<double *> cornerNeighborPoints) {
    calculateEllipsoidalSurfaceNormal();
  }
      
  virtual void calculateSurfaceNormal() {
      std::vector<Distance> radii = targetRadii();
      std::vector<double> normal(3, 0.);
      SpiceDouble point[3];
      surfaceIntersection()->ToNaifArray(point);
      surfnm_c(radii[0].kilometers(), radii[1].kilometers(), radii[2].kilometers(), point, (SpiceDouble *) &normal[0]);
      setNormal(normal);
      setHasNormal(true);
  }

  virtual void calculateDefaultNormal()  {
     setHasNormal(true);
  }

  Distance localRadius(const Latitude &lat, const Longitude &lon) {
    std::vector<Distance> radii = targetRadii();
    double a = radii[0].kilometers();
    double b = radii[1].kilometers();
    double c = radii[2].kilometers();
    
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

  double resolution() {
    return ShapeModel::resolution();
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

  cout << "Begin testing Shape Model base class...." << endl;

  MyShape shape(&targ, pvl);

  cout << endl << "  Shape name is " << shape.name() << endl;
  cout << "    Do we have an intersection? " << shape.hasIntersection() << endl;
  try {
    cout << "    Get the resolution:         " << shape.resolution() << endl;
  }
  catch (IException &e) {
    cout << "    Test resolution() error message when there is no intersection:" << endl;
    e.print();
  }
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
  cout << endl << "    Testing pure virtual method intersectSurface..." << endl; 
  if (!shape.intersectSurface(sB, lookB)) { 
      cout << "    ...  intersectSurface method failed" << endl;
      return -1;
  }
  cout << "    Do we have an intersection? " << shape.hasIntersection() << endl;
  try {
    cout << "    Get the resolution:         " << shape.resolution() << endl;
  }
  catch (IException &e) {
    cout << "    Test resolution() error message when there is no intersection:" << endl;
    e.print();
  }
  SurfacePoint *sp = shape.surfaceIntersection();
  cout << "    surface point = (" << sp->GetX().kilometers() << ", " << 
    sp->GetY().kilometers() << ", " << sp->GetZ().kilometers() << endl;

  try {
    cout << endl << "  Testing class method normal() when no normal exists..." << endl;
    cout << "    Do we have a normal? " << shape.normalStatus() << endl;
    std::vector<double> badnormal = shape.normal();
  }
  catch(Isis::IException &e) {
    e.print();
  }

  cout << endl << "  Testing photometric angle calculations before normal computation..." << endl;
  cout << "    Do we have a normal? " << shape.normalStatus() << endl;
  cout << "    Emission angle = " << shape.emissionAngle(sB);
  cout << "    Incidence angle = " << shape.incidenceAngle(uB);
  cout << endl;

  cout << endl << "  Testing class method calculateLocalNormal..." << endl;
  QVector<double *>  notUsed(4);

  for (int i = 0; i < notUsed.size(); i ++)
      notUsed[i] = new double[3];

  shape.calculateLocalNormal(notUsed);
  cout << "    Do we have a normal? " << shape.normalStatus() << endl;
  vector<double> myNormal(3);
  myNormal = shape.normal();
  cout << "    local normal = (" << myNormal[0] << ", " << myNormal[1] << ", " << myNormal[2] << endl;

  cout << endl << "  Testing class method calculateSurfaceNormal..." << endl;
  shape.calculateSurfaceNormal(); 
  myNormal = shape.normal();
  cout << "    surface normal = (" << myNormal[0] << ", " << myNormal[1] << ", " << myNormal[2] << endl;

  cout << endl << "  Testing photometric angle calculations with undersize normal..." << endl;
  shape.setSmallNormal();
  cout << "    Emission angle = " << shape.emissionAngle(sB);
  cout << "    Incidence angle = " << shape.incidenceAngle(uB);
  cout << endl;

  cout << endl << "  Testing photometric angle calculations with oversize normal..." << endl;
  shape.setBigNormal();
  cout << "    Emission angle = " << shape.emissionAngle(sB);
  cout << "    Incidence angle = " << shape.incidenceAngle(uB);
  cout << endl;

  cout << "  Testing class method calculateDefaultNormal..." << endl;
  shape.calculateDefaultNormal();
  myNormal = shape.normal();
  cout << "    default normal = (" << myNormal[0] << ", " << myNormal[1] << ", " << myNormal[2] << endl;

  cout << endl << "  Testing photometric angle calculations..." << endl;
  cout << "    Emission angle = " << shape.emissionAngle(sB);
  cout << "    Incidence angle = " << shape.incidenceAngle(uB);
  cout << "    Phase angle = "  << shape.phaseAngle(sB, uB);
  cout << endl;

  cout << endl << "  Testing localRadius method ..." << endl;
  cout  << "    Local radius = " << shape.localRadius(Latitude(20.532461495381, Angle::Degrees),
                                                      Longitude(228.26609149754, Angle::Degrees)).kilometers() << endl;
  // Mars radii = 3397.      3397.         3375.

  cout << endl << "  Testing setHasIntersection method" << endl;
  shape.setHasIntersection(false);
  cout << "    Do we have an intersection? " << shape.hasIntersection() << endl;
  try {
    cout << "    Get the resolution:         " << shape.resolution() << endl;
  }
  catch (IException &e) {
    cout << "    Test resolution() error message when there is no intersection:" << endl;
    e.print();
  }

  cout << endl << "  Testing setSurfacePoint method ..." << endl;
  shape.setSurfacePoint(*sp);
  cout << "     Do we have an intersection? " << shape.hasIntersection() << endl;
  try {
    cout << "    Get the resolution:         " << shape.resolution() << endl;
  }
  catch (IException &e) {
    cout << "    Test resolution() error message when there is no intersection:" << endl;
    e.print();
  }
  cout << "     surface point = (" << sp->GetX().kilometers() << ", " << 
    sp->GetY().kilometers() << ", " << sp->GetZ().kilometers() << ")" << endl;
  
  // Test ellipse methods in base class
  MyEllipse eshape(&targ); 
  try {
    cout << endl << "  Testing ellipsoid methods in base class" << endl;
    cout << "    Do we have an intersection? " << eshape.hasIntersection() << endl;
    try {
      cout << "    Get the resolution:         " << eshape.resolution() << endl;
    }
    catch (IException &e) {
      cout << "    Test resolution() error message when there is no intersection:" << endl;
      e.print();
    }
    cout << endl << "    Testing  failing of method intersectEllipsoid..." << endl;
    std::vector<double> badlook(3,1.);
    badlook[0] = -1.; 

    if (!eshape.intersectSurface(sB, badlook)) 
      cout << "    ...  intersectSurface method failed -- no intersection" << endl;
    
    cout << "    Do we have an intersection? " << eshape.hasIntersection() << endl;
    try {
      cout << "    Get the resolution:         " << eshape.resolution() << endl;
    }
    catch (IException &e) {
      cout << "    Test resolution() error message when there is no intersection:" << endl;
      e.print();
    }
    cout << endl << "    Testing  method intersectEllipsoid..." << endl; 

    if (eshape.intersectSurface(sB, lookB)) { 
      SurfacePoint *sp = eshape.surfaceIntersection();
      cout << "    surface point = (" << sp->GetX().kilometers() << ", " << 
        sp->GetY().kilometers() << ", " << sp->GetZ().kilometers() << endl;
    }
    cout << "    Do we have an intersection? " << eshape.hasIntersection() << endl;
    try {
      cout << "    Get the resolution:         " << eshape.resolution() << endl;
    }
    catch (IException &e) {
      cout << "    Test resolution() error message when there is no intersection:" << endl;
      e.print();
    }
    SurfacePoint *sp = eshape.surfaceIntersection();
    cout << "    surface point = (" << sp->GetX().kilometers() << ", " << 
      sp->GetY().kilometers() << ", " << sp->GetZ().kilometers() << endl;

    try {
      cout << endl << "    Testing  method calculateEllipsoidalSurfaceNormal with bad intersection..." << endl;
      SurfacePoint badsp;
      eshape.setSurfacePoint(badsp);
      eshape.setHasIntersection(true);
      eshape.calculateLocalNormal(notUsed);
    }
    catch(Isis::IException &e) {
      e.print();
    }

    cout << endl << "    Testing  method setHasIntersection false..." << endl;
    eshape.setHasIntersection(false); 
    cout << "    Do we have an intersection? " << eshape.hasIntersection() << endl;
    try {
      cout << "    Get the resolution:         " << eshape.resolution() << endl;
    }
    catch (IException &e) {
      cout << "    Test resolution() error message when there is no intersection:" << endl;
      e.print();
    }
    try {
      cout << endl << "    Testing  method calculateEllipsoidalSurfaceNormal with no intersection..." << endl;
      eshape.calculateLocalNormal(notUsed);
    }
    catch(Isis::IException &e) {
      e.print();
    }
    cout << endl << "    Testing  method calculateEllipsoidalSurfaceNormal with valid intersection..." << endl; 
    if (eshape.intersectSurface(sB, lookB)) cout << "    Intersection set" << endl;
    cout << "      Do we have a normal? " << eshape.normalStatus() << endl;
    eshape.calculateLocalNormal(notUsed);
    cout << "      Do we have a normal? " << eshape.normalStatus() << endl;
    myNormal = eshape.normal();
    cout << "      local normal = (" << myNormal[0] << ", " << myNormal[1] << ", " << myNormal[2] << endl;
    cout << endl << "    Testing  method targetRadii..." << endl;
    eshape.calculateSurfaceNormal(); 
    myNormal = eshape.normal();
    cout << "      true normal = (" << myNormal[0] << ", " << myNormal[1] << ", " << myNormal[2] << endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }
 
  // Test 
  MyEllipse defaultShape;
  cout << endl << "  Testing default constructor..." << endl;
  cout << "    Shape is " << defaultShape.name() << endl;
  cout << "    Do we have an an intersection? " << defaultShape.hasIntersection() << endl;
  try {
    cout << "    Get the resolution:       " << defaultShape.resolution() << endl;
  }
  catch (IException &e) {
    cout << "    Test resolution() error message when there is no intersection:" << endl;
    e.print();
  }
  cout << "    Is there an normal? " << defaultShape.normalStatus() << endl;
  defaultShape.setHasIntersection(true);
  defaultShape.calculateDefaultNormal();
  cout << "    Number of normal components = " << defaultShape.normal().size() << endl;

  cube.close();
}
