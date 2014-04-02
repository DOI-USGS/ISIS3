#include <string>
#include <iostream>

#include "boost/numeric/ublas/symmetric.hpp"

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "IException.h"
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
  *   @history 2011-06-07 Debbie A. Cook and Tracie Sucharski - Modified point types
  *                         Ground ------> Fixed
  *                         Tie----------> Free
  *
  */
int main() {
  Preference::Preferences(true);

  cout << "ControlPoint unitTest" << endl;

  ControlPoint cp("C151");

  cp.SetType(ControlPoint::Fixed);
  cp.SetIgnored(true);
  cp.SetAprioriSurfacePointSource(ControlPoint::SurfacePointSource::Basemap);
  cp.SetAprioriSurfacePointSourceFile("/work1/tsucharski/basemap.cub");
  cp.SetAprioriRadiusSource(ControlPoint::RadiusSource::DEM);
  cp.SetAprioriRadiusSourceFile("$base/dems/molaMarsPlanetaryRadius0003.cub");

  SurfacePoint point(Displacement(-424.024048, Displacement::Meters),
      Displacement(734.4311949, Displacement::Meters),
      Displacement(529.919264, Displacement::Meters),
      Distance(10, Distance::Meters),
      Distance(50, Distance::Meters),
      Distance(20, Distance::Meters));
  cp.SetAdjustedSurfacePoint(point);
  cp.SetAprioriSurfacePoint(point);
  cp.SetEditLock(true);

  ControlMeasure *cm1 = new ControlMeasure;
  cm1->SetCubeSerialNumber("Test1");
  cm1->SetIgnored(true);
  cm1->SetCoordinate(1.0, 2.0);
  cm1->SetResidual(-3.0, 4.0);
  cm1->SetDiameter(15.0);
  cm1->SetAprioriSample(2.0);
  cm1->SetAprioriLine(5.0);
  cm1->SetSampleSigma(.01);
  cm1->SetLineSigma(.21);
  cm1->SetChooserName("seedgrid");
  cm1->SetDateTime("2005-05-03T00:00:00");

  cout << "Adding ControlMeasure with cube serial number [" << cm1->GetCubeSerialNumber() << "]" << endl; // Cube Serial Number "Test1"
  cp.Add(cm1);

  printPoint(cp);

  ControlMeasure *cm2 = new ControlMeasure;
  cm2->SetCubeSerialNumber("Test2");
  cm2->SetIgnored(true);
  cm2->SetCoordinate(100.0, 200.0);
  cm2->SetDiameter(15.0);
  cm2->SetAprioriSample(2.0);
  cm2->SetAprioriLine(5.0);
  cm2->SetSampleSigma(.01);
  cm2->SetLineSigma(.21);
//  cm2->SetType(Isis::ControlMeasure::Reference);
  cm2->SetResidual(-2.0, 2.0);
  cm2->SetChooserName("seedgrid");
  cm2->SetDateTime("2005-05-03T00:00:00");
  cout << "Adding ControlMeasure with cube serial number [" << cm2->GetCubeSerialNumber() << "]" << endl; // Cube Serial Number "Test2"
  cp.Add(cm2);
  cout << "Testing Edit Locking... ";
  cp.SetRefMeasure(cm2);
  if (cp.GetRefMeasure() != cm2) {
    cp.SetEditLock(false);
    cp.SetRefMeasure(cm2);
    if (cp.GetRefMeasure() == cm2)
      cout << "ok!\n";
    else
      cout << "Failed!\n";
  }
  else {
    cout << "Failed!\n";
  }
  cout << "\n";
  cp.SetEditLock(false);
  cp.SetRefMeasure(cm2);
  cp.SetEditLock(true);

  printPoint(cp);

  cout << "Testing copy constructor...\n";
  ControlPoint copy(cp);
  cout << "\t Also testing == operator" << endl;
  bool equal = (cp == cp);
  cout << "\t\t original == its self (yes)? " << equal << endl;
  copy.SetEditLock(false);
  copy.SetRefMeasure(0);
  equal = (cp == copy);
  cout << "\t\t original == copy (no)? " << equal << endl;
  printPoint(cp);
  cout << "Testing assignment operator...\n";
  ControlPoint assignment = copy;
  printPoint(assignment);

  // Should be successful
  cout << "Deleting ControlMeasure with cube serial number [" << cp.getCubeSerialNumbers().at(0).toStdString() << "]" << endl;
  cout << "Measure type: " << ControlMeasure::MeasureTypeToString(cp.GetMeasure(0)->GetType()) << endl;
  cp.Delete(0);
  printPoint(cp);
//  cout << "ReferenceIndex = " << cp.GetReferenceIndex() << endl;

//  cout << "ReferenceIndex = " << cp.GetReferenceIndex() << endl;

  cout << endl << "Test adding control measures with identical serial numbers ..." << endl;
  try {
    cp.Add(cm2);
  }
  catch (IException &e) {
    e.print();
  }

  cout << endl << "Test SetAdjustedSurfacePoint ... " << endl;
  SurfacePoint surfPt(cp.GetAdjustedSurfacePoint());
  cout << "X = " << surfPt.GetX().meters() << endl;
  cout << "Y = " << surfPt.GetY().meters() << endl;
  cout << "Z = " << surfPt.GetZ().meters() << endl;
  cout << "Latitude = " << surfPt.GetLatitude().degrees() << endl;
  cout << "Longitude = " << surfPt.GetLongitude().degrees() << endl;
  cout << "Radius = " << surfPt.GetLocalRadius().meters() << endl;
  surfPt.SetSpherical(Latitude(32, Angle::Degrees),
      Longitude(120, Angle::Degrees),
      Distance(1000, Distance::Meters));
  cp.SetAdjustedSurfacePoint(surfPt);
  surfPt = cp.GetAdjustedSurfacePoint();
  cout << "X = " << surfPt.GetX().meters() << endl;
  cout << "Y = " << surfPt.GetY().meters() << endl;
  cout << "Z = " << surfPt.GetZ().meters() << endl;
  cout << "Latitude = " << surfPt.GetLatitude().degrees() << endl;
  cout << "Longitude = " << surfPt.GetLongitude().degrees() << endl;
  cout << "Radius = " << surfPt.GetLocalRadius().meters() << endl;

  cout << endl << "Test conversions for apriori/adjusted covariance matrices ... " << endl;

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
  cp.SetAprioriSurfacePoint(point);

  //c.SetAprioriCovariance();
  point = cp.GetAprioriSurfacePoint();
  cout << "Apriori Sigma X = " << point.GetXSigma().meters() << endl;
  cout << "Apriori Sigma Y = " << point.GetYSigma().meters() << endl;
  cout << "Apriori Sigma Z = " << point.GetZSigma().meters() << endl;

  point = cp.GetAdjustedSurfacePoint();
  cout << "Adjusted Sigma X = " << point.GetXSigma().meters() << endl;
  cout << "Adjusted Sigma Y = " << point.GetYSigma().meters() << endl;
  cout << "Adjusted Sigma Z = " << point.GetZSigma().meters() << endl;

  cout << endl;

  cout << "Testing IsReferenceExplicit..." << endl;
  cout << "cp:                    " << cp.IsReferenceExplicit() << endl;

  ControlMeasure *cm3 = new ControlMeasure;
  cm3->SetCubeSerialNumber("Test1");
  cm3->SetIgnored(true);
  cm3->SetCoordinate(1.0, 2.0);
  cm3->SetResidual(-3.0, 4.0);
  cm3->SetDiameter(15.0);
  cm3->SetAprioriSample(2.0);
  cm3->SetAprioriLine(5.0);
  cm3->SetSampleSigma(.01);
  cm3->SetLineSigma(.21);
  cm3->SetChooserName("seedgrid");
  cm3->SetDateTime("2005-05-03T00:00:00");

  ControlMeasure *cm4 = new ControlMeasure;
  cm4->SetCubeSerialNumber("Test2");
  cm4->SetIgnored(true);
  cm4->SetCoordinate(1.0, 2.0);
  cm4->SetResidual(-3.0, 4.0);
  cm4->SetDiameter(15.0);
  cm4->SetAprioriSample(2.0);
  cm4->SetAprioriLine(5.0);
  cm4->SetSampleSigma(.01);
  cm4->SetLineSigma(.21);
  cm4->SetChooserName("seedgrid");
  cm4->SetDateTime("2005-05-03T00:00:00");

  ControlPoint newCp;
  cout << "newCp:                 " << newCp.IsReferenceExplicit() << endl;
  newCp.Add(cm3);
  cout << "newCp with implicit:   " << newCp.IsReferenceExplicit() << endl;
  newCp.Add(cm4);
  newCp.SetRefMeasure(cm3);
  cout << "newCp with explicit:   " << newCp.IsReferenceExplicit() << endl;
  newCp.Delete(cm3);
  cout << "newCp reverted to implicit:   " << newCp.IsReferenceExplicit() << endl;

  cout << "\ntesting getMeasures method...\n";
  ControlMeasure * alpha = new ControlMeasure;
  alpha->SetCubeSerialNumber("alpha");
  ControlMeasure * beta = new ControlMeasure;
  beta->SetCubeSerialNumber("beta");
  ControlPoint getMeasuresTestPoint;
  getMeasuresTestPoint.Add(alpha);
  getMeasuresTestPoint.Add(beta);
  QList< ControlMeasure * > measures = getMeasuresTestPoint.getMeasures();
  foreach (ControlMeasure * measure, measures)
    cout << measure->GetCubeSerialNumber() << "\n";
  beta->SetIgnored(true);
  measures = getMeasuresTestPoint.getMeasures(true);
  foreach (ControlMeasure * measure, measures)
    cout << measure->GetCubeSerialNumber() << "\n";

  cout << "\ntesting error handling for StringToPointType...\n";
  try {
    ControlPoint::StringToPointType("aoeu");
  }
  catch (IException &e) {
    cout << "  " << e.toString() << "\n";
  }
}

void printPoint(Isis::ControlPoint &p) {
  bool wasLocked = p.IsEditLocked();
  p.SetEditLock(false);
  p.SetChooserName("cnetref");
  p.SetDateTime("2005-05-03T00:00:00");
  p.SetEditLock(wasLocked);

  ControlNet net;

  ControlPoint *copyPoint = new ControlPoint(p);
  wasLocked = copyPoint->IsEditLocked();
  copyPoint->SetEditLock(false);
  for (int i = 0; i < copyPoint->GetNumMeasures(); i++) {
    ControlMeasure *measure = copyPoint->GetMeasure(i);
    bool explicitLock = measure->IsEditLocked();
    measure->SetEditLock(false);
    measure->SetChooserName("seedgrid");
    measure->SetDateTime("2005-05-03T00:00:00");
    measure->SetEditLock(explicitLock);
  }
  copyPoint->SetEditLock(wasLocked);

  net.AddPoint(copyPoint);
  net.SetNetworkId("Identifier");
  net.SetCreatedDate("Yesterday");
  net.SetModifiedDate("Yesterday");
  net.Write("./tmp.net", true);
  Pvl tmp("./tmp.net");
  cout << "Printing point:\n" << tmp << "\nDone printing point." << endl << endl;
  remove("./tmp.net");
}
