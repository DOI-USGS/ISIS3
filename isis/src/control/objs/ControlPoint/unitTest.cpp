#include <string>
#include <iostream>

#include "boost/numeric/ublas/symmetric.hpp"

#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "iException.h"
#include "Latitude.h"
#include "Longitude.h"
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
int main() {
  Preference::Preferences(true);

  cout << "ControlPoint unitTest" << endl;

  ControlPoint c("C151");

  c.SetType(ControlPoint::Ground);
  c.SetIgnored(true);
  c.SetAprioriSurfacePointSource(ControlPoint::SurfacePointSource::Basemap);
  c.SetAprioriSurfacePointSourceFile("/work1/tsucharski/basemap.cub");
  c.SetAprioriRadiusSource(ControlPoint::RadiusSource::DEM);
  c.SetAprioriRadiusSourceFile("$base/dems/molaMarsPlanetaryRadius0003.cub");

  SurfacePoint point(Displacement(-424.024048, Displacement::Meters),
                     Displacement(734.4311949, Displacement::Meters),
                     Displacement(529.919264, Displacement::Meters),
                     Distance(10, Distance::Meters),
                     Distance(50, Distance::Meters),
                     Distance(20, Distance::Meters));
  c.SetSurfacePoint(point);
  c.SetAprioriSurfacePoint(point);
  c.SetEditLock(true);

  ControlMeasure *d = new ControlMeasure;
  d->SetCubeSerialNumber("Test1");
  d->SetIgnored(true);
  d->SetCoordinate(1.0, 2.0);
  d->SetResidual(-3.0, 4.0);
  d->SetDiameter(15.0);
  d->SetAprioriSample(2.0);
  d->SetAprioriLine(5.0);
  d->SetSampleSigma(.01);
  d->SetLineSigma(.21);
  d->SetChooserName("seedgrid");
  d->SetDateTime("2005-05-03T00:00:00");

  cout << "Adding ControlMeasure with cube serial number [" << d->GetCubeSerialNumber() << "]" << endl; // Cube Serial Number "Test1"
  c.Add(d);

  printPoint(c);

  d = new ControlMeasure;
  d->SetCubeSerialNumber("Test2");
  d->SetIgnored(true);
  d->SetCoordinate(100.0, 200.0);
  d->SetDiameter(15.0);
  d->SetAprioriSample(2.0);
  d->SetAprioriLine(5.0);
  d->SetSampleSigma(.01);
  d->SetLineSigma(.21);
  d->SetType(Isis::ControlMeasure::Reference);
  d->SetResidual(-2.0, 2.0);
  d->SetChooserName("seedgrid");
  d->SetDateTime("2005-05-03T00:00:00");
  cout << "Adding ControlMeasure with cube serial number [" << d->GetCubeSerialNumber() << "]" << endl; // Cube Serial Number "Test2"
  c.Add(d);
  printPoint(c);

  // Should be successful
  cout << "Deleting ControlMeasure with cube serial number [" << c.GetCubeSerialNumbers().at(0).toStdString() << "]" << endl;
  cout << "Measure type: " << ControlMeasure::MeasureTypeToString(c.GetMeasure(0)->GetType()) << endl;
  c.Delete(0);
  printPoint(c);
  cout << "ReferenceIndex = " << c.GetReferenceIndex() << endl;

  // Should fail
  cout << "Deleting ControlMeasure with cube serial number [" << c.GetCubeSerialNumbers().at(0).toStdString() << "]" << endl;
  cout << "Measure type: " << ControlMeasure::MeasureTypeToString(c.GetMeasure(0)->GetType()) << endl;
  try {
    c.Delete(0);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << "ReferenceIndex = " << c.GetReferenceIndex() << endl;

  cout << endl << "Test adding control measures with identical serial numbers ..." << endl;
  try {
    c.Add(d);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }

  cout << endl << "Test SetSurfacePoint ... " << endl;
  SurfacePoint surfPt(c.GetSurfacePoint());
  cout << "X = " << surfPt.GetX().GetMeters() << endl;
  cout << "Y = " << surfPt.GetY().GetMeters() << endl;
  cout << "Z = " << surfPt.GetZ().GetMeters() << endl;
  cout << "Latitude = " << surfPt.GetLatitude().GetDegrees() << endl;
  cout << "Longitude = " << surfPt.GetLongitude().GetDegrees() << endl;
  cout << "Radius = " << surfPt.GetLocalRadius().GetMeters() << endl;
  surfPt.SetSpherical(Latitude(32, Angle::Degrees),
                      Longitude(120, Angle::Degrees),
                      Distance(1000, Distance::Meters));
  c.SetSurfacePoint(surfPt);
  surfPt = c.GetSurfacePoint();
  cout << "X = " << surfPt.GetX().GetMeters() << endl;
  cout << "Y = " << surfPt.GetY().GetMeters() << endl;
  cout << "Z = " << surfPt.GetZ().GetMeters() << endl;
  cout << "Latitude = " << surfPt.GetLatitude().GetDegrees() << endl;
  cout << "Longitude = " << surfPt.GetLongitude().GetDegrees() << endl;
  cout << "Radius = " << surfPt.GetLocalRadius().GetMeters() << endl;

  cout << endl << "Test conversions for apriori/aposteriori covariance matrices ... " << endl;

  symmetric_matrix<double, upper> covar;
  covar.resize(3);
  covar.clear();
  covar(0, 0) = 100.;
  covar(0, 1) = 3.55271368e-15;
  covar(0, 2) = -1.81188398e-13;
  covar(1, 0) = 3.55271368e-15;
  covar(1, 1) = 2500.;
  covar(1, 2) = 3.41060513e-13;
  covar(2, 0) = -1.81188398e-13;
  covar(2, 1) = 3.41060513e-13;
  covar(2, 2) = 400.;

  point.SetRectangularMatrix(covar);
  c.SetAprioriSurfacePoint(point);

  //c.SetAprioriCovariance();
  point = c.GetAprioriSurfacePoint();
  cout << "Apriori Sigma X = " << point.GetXSigma().GetMeters() << endl;
  cout << "Apriori Sigma Y = " << point.GetYSigma().GetMeters() << endl;
  cout << "Apriori Sigma Z = " << point.GetZSigma().GetMeters() << endl;

  point = c.GetSurfacePoint();
  cout << "Aposteriori Sigma X = " << point.GetXSigma().GetMeters() << endl;
  cout << "Aposteriori Sigma Y = " << point.GetYSigma().GetMeters() << endl;
  cout << "Aposteriori Sigma Z = " << point.GetZSigma().GetMeters() << endl;

  cout << endl;
}

void printPoint(Isis::ControlPoint &p) {
  bool wasLocked = p.IsEditLocked();
  p.SetEditLock(false);
  p.SetChooserName("cnetref");
  p.SetDateTime("2005-05-03T00:00:00");
  p.SetEditLock(wasLocked);

  Pvl tmp;
  tmp.AddObject(p.ToPvlObject());
  cout << endl << "Printing point:\n" << tmp << "\nDone printing point." << endl << endl;
}
