#include <string>
#include <iostream>

#include "boost/numeric/ublas/symmetric.hpp"

#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "iException.h"
#include "Preference.h"
#include "SurfacePoint.h"

using namespace std;
using namespace boost::numeric::ublas;
using namespace Isis;

void printPoint(ControlPoint &p);

/**
  * @brief Test ControlPoint object for accuracy and correct behavior.
  *
  * @history 2010-06-30  Tracie Sucharski, Updated for binary control net and 
  *                         new keywords.
  * @history 2010-08-12  Tracie Sucharski,  Keywords changed AGAIN.. Added many 
  *                         more tests for conversions between lat/lon/radius
  *                         and x/y/z and between simgas and covariance matrices.
  *  
*/
int main () {
  Preference::Preferences(true);

  cout << "ControlPoint unitTest" << endl;

  ControlPoint c("C151");

  c.SetType(ControlPoint::Ground);
  c.SetIgnore(true);
  c.SetAprioriSurfacePointSource(ControlPoint::SurfacePointSource::Basemap);
  c.SetAprioriSurfacePointSourceFile("/work1/tsucharski/basemap.cub");
  c.SetAprioriRadiusSource(ControlPoint::RadiusSource::DEM);
  c.SetAprioriRadiusSourceFile("$base/dems/molaMarsPlanetaryRadius0003.cub");

  SurfacePoint point(Displacement(-424.024048),
                     Displacement(734.4311949),
                     Displacement(529.919264),
                     Distance(10), Distance(50), Distance(20));
  c.SetSurfacePoint(point);
  c.SetAprioriSurfacePoint(point);
  c.SetEditLock(true);

  ControlMeasure d;
  d.SetCubeSerialNumber("Test1");
  d.SetIgnore(true);
  d.SetCoordinate(1.0,2.0);
  d.SetResidual(-3.0,4.0);
  d.SetDiameter(15.0);
  d.SetAprioriSample(2.0);
  d.SetAprioriLine(5.0);
  d.SetSampleSigma(.01);
  d.SetLineSigma(.21);
  d.SetChooserName("seedgrid");
  d.SetDateTime("2005-05-03T00:00:00");

  c.Add(d);

  cout << "test PointTypeString(): " << c.PointTypeString() << "\n";
  printPoint(c);

  d.SetCubeSerialNumber("Test2");
  d.SetCoordinate(100.0,200.0);
  d.SetType(Isis::ControlMeasure::Reference);
  d.SetResidual(-2.0,2.0);
  d.SetChooserName("seedgrid");
  d.SetDateTime("2005-05-03T00:00:00");
  c.Add(d);
  printPoint(c);
  cout <<"ReferenceIndex = " << c.ReferenceIndex() << endl;

  c.Delete(0);
  printPoint(c);
  cout <<"ReferenceIndex = " << c.ReferenceIndex() << endl;

  cout << endl << "Test adding control measures with identical serial numbers ..." << endl;
  try {
    c.Add(d);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }

  cout << endl << "Test SetUniversalGround ... " << endl;
  cout << "X = " << c.GetSurfacePoint().GetX() << endl;
  cout << "Y = " << c.GetSurfacePoint().GetY() << endl;
  cout << "Z = " << c.GetSurfacePoint().GetZ() << endl;
  cout << "Latitude = " << c.UniversalLatitude() << endl;
  cout << "Longitude = " << c.UniversalLongitude() << endl;
  cout << "Radius = " << c.Radius() << endl;
  c.SetUniversalGround(32.,120.,1000.);
  cout << "X = " << c.GetSurfacePoint().GetX() << endl;
  cout << "Y = " << c.GetSurfacePoint().GetY() << endl;
  cout << "Z = " << c.GetSurfacePoint().GetZ() << endl;


  cout << endl << "Test conversions for apriori/aposteriori covariance matrices ... " << endl;

  symmetric_matrix<double,upper> covar;
  covar.resize(3);
  covar.clear();
  covar(0,0) = 100.;
  covar(0,1) = 3.55271368e-15;
  covar(0,2) = -1.81188398e-13;
  covar(1,0) = 3.55271368e-15;
  covar(1,1) = 2500.;
  covar(1,2) = 3.41060513e-13;
  covar(2,0) = -1.81188398e-13;
  covar(2,1) = 3.41060513e-13;
  covar(2,2) = 400.;

  point.SetRectangularMatrix(covar);
  c.SetAprioriSurfacePoint(point);

  //c.SetAprioriCovariance();
  point = c.GetAprioriSurfacePoint();
  cout << "Apriori Sigma X = " << point.GetXSigma() << endl;
  cout << "Apriori Sigma Y = " << point.GetYSigma() << endl;
  cout << "Apriori Sigma Z = " << point.GetZSigma() << endl;

  point = c.GetSurfacePoint();
  cout << "Aposteriori Sigma X = " << point.GetXSigma() << endl;
  cout << "Aposteriori Sigma Y = " << point.GetYSigma() << endl;
  cout << "Aposteriori Sigma Z = " << point.GetZSigma() << endl;

  cout << endl;
}

void printPoint(Isis::ControlPoint &p) {
  bool wasLocked = p.EditLock();
  p.SetEditLock(false);
  p.SetChooserName("cnetref");
  p.SetDateTime("2005-05-03T00:00:00");
  p.SetEditLock(wasLocked);

  Pvl tmp;
  tmp.AddObject(p.CreatePvlObject());
  cout << tmp << endl;
}
