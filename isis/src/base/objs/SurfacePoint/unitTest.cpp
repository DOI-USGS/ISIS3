#include <iostream>
#include <iomanip>
#include <vector>

#include "Constants.h"
#include "Camera.h"
#include "Displacement.h"
#include "Distance.h"
#include "IException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Preference.h"
#include "SurfacePoint.h"

#include "boost/numeric/ublas/symmetric.hpp"

using namespace Isis;
using namespace std;
using namespace boost::numeric::ublas;

/**
 *
 * @author 2010-07-30 Tracie Sucharski, Ken L. Edmundson, and Debbie A. Cook
 *
 * @internal
 *   @history 2015-02-20 Jeannie Backer - Added print statement for
 *            latitude, longitude and radius weight accessor methods.
 *   @history 2018-06-28 Debbie A.Cook - Removed test of obsolete method
 *            SetRadii
 */
int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);
  symmetric_matrix<double,upper> cvRect,cvOc;

  try {
    cout << "UnitTest for SurfacePoint" << endl << endl;
    cout << "1-Test rectangular set of point and variance only ..." << endl;
    cout << " with x=-424.024048 m, y=734.4311949 m, z=529.919264 m,"
          "sigmaX=10. m, sigmaY=50. m, sigmaZ=20. m" << endl << endl;
    Isis::SurfacePoint spRec;

    symmetric_matrix<double,upper> covar; // Units are m**2
    covar.resize(3);
    covar.clear();
    covar(0,0) = 100.;
    covar(1,1) = 2500.;
    covar(2,2) = 400.;

    spRec.SetRectangular(Displacement(-424.024048, Displacement::Meters),
                         Displacement(734.4311949, Displacement::Meters),
                         Displacement(529.919264, Displacement::Meters), covar);

    double lat = spRec.GetLatitude().degrees();
    double lon = spRec.GetLongitude().degrees();
    double radius = spRec.GetLocalRadius().meters();
    double latSig = spRec.GetLatSigma().degrees();
    double lonSig = spRec.GetLonSigma().degrees();
    double radSig = spRec.GetLocalRadiusSigma().meters();
    symmetric_matrix<double,upper> covarSphere(3);
    covarSphere.clear();
    covarSphere = spRec.GetSphericalMatrix();
    
    cout << setprecision(9);
    cout << "  Output spherical..." << endl;
    cout << "    lat = " << lat << " degrees, lon = " << lon <<
            " degrees, radius = " << radius << " meters" << endl;
    cout << "    lat = " << spRec.GetLatitude().radians() <<
            " radians, lon = " << spRec.GetLongitude().radians() 
         << " radians, radius = " << spRec.GetLocalRadius().meters() <<
            " meters" << endl;
    cout << "    latitude sigma=" << latSig << " degrees, longitude sigma=" <<
            lonSig << " degrees, radius sigma=" << radSig << " m" << endl;
    cout << "    latitude sigma=" << spRec.GetLatSigma().radians() <<
            " radians, longitude sigma=" << spRec.GetLonSigma().radians()
         << " radians, radius sigma=" << spRec.GetLocalRadiusSigma().meters()
         << " m" << endl;
    cout << "    latitude sigma=" << spRec.GetLatSigmaDistance().meters() <<
         " m, longitude sigma=" << spRec.GetLonSigmaDistance().meters() <<
         " m, radius sigma=" << spRec.GetLocalRadiusSigma().meters()
         <<  " m" << endl;
    cout << "    latitude weight =" << spRec.GetLatWeight() <<
         ", longitude weight =" << spRec.GetLonWeight() <<
         ", radius weight =" << spRec.GetLocalRadiusWeight() << endl;

    cout << "    spherical covariance matrix = " << covarSphere(0,0) << "  " <<
         covarSphere(0,1) << "  " << covarSphere(0,2) << endl;
    cout << "                                  " << covarSphere(1,0) << "  " <<
         covarSphere(1,1) << "  " << covarSphere(1,2) << endl;
    cout << "                                  " << covarSphere(2,0) << "  " <<
         covarSphere(2,1) << "  " << covarSphere(2,2) << endl;
    cout << "  Input rectangular sigmas = " << spRec.GetXSigma().meters()
         << "/" << spRec.GetYSigma().meters() << "/"
         << spRec.GetZSigma().meters() << std::endl;



    cout << endl;
    cout << "2-Testing spherical set of point and variance/covariance matrix ..."
         << endl;
    cout << " with lat=" << lat << " degrees, lon=" << lon << " degrees, radius="
         << radius << " m" << endl;
    cout << " latitude sigma=" << latSig << " m, longitude sigma=" << lonSig
         << " m, radiusSig=" << radSig << " m" << endl;
    Isis::SurfacePoint spSphere;
    spSphere.SetSpherical(Latitude(lat, Angle::Degrees),
                          Longitude(lon, Angle::Degrees),
                          Distance(radius, Distance::Meters),
                          covarSphere );
    symmetric_matrix<double,upper> covarRec(3);
    covarRec.clear();
    covarRec = spSphere.GetRectangularMatrix();

    if(fabs(covarRec(0,1)) < 1E-12) covarRec(0,1) = 0.0;
    if(fabs(covarRec(0,2)) < 1E-12) covarRec(0,2) = 0.0;
    if(fabs(covarRec(1,0)) < 1E-12) covarRec(1,0) = 0.0;
    if(fabs(covarRec(1,2)) < 1E-12) covarRec(1,2) = 0.0;
    if(fabs(covarRec(2,0)) < 1E-12) covarRec(2,0) = 0.0;
    if(fabs(covarRec(2,2)) < 1E-12) covarRec(2,2) = 0.0;

    cout << "  Output rectangular..." << endl;
    cout << "    x=" << spSphere.GetX().meters()
         << " m, y=" << spSphere.GetY().meters()
         << " m, z=" << spSphere.GetZ().meters() << " m" << endl;
    cout << "    X sigma=" << spSphere.GetXSigma().meters() << " m, Y sigma="
         << spSphere.GetYSigma().meters() << " m, Z sigma=" <<
            spSphere.GetZSigma().meters() << " m" << endl;
    cout << "    rectangular covariance matrix = " 
         << setw(10) << covarRec(0,0) << setw(10) << covarRec(0,1)
         << setw(10) << covarRec(0,2) << endl;
    cout << "                                    "
         << setw(10) << covarRec(1,0) << setw(10) << covarRec(1,1)
         << setw(10) << covarRec(1,2) << endl;
    cout << "                                    "
         << setw(10) << covarRec(2,0) << setw(10) << covarRec(2,1)
         << setw(10) << covarRec(2,2) << endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }

    try {
    cout << "3-Testing rectangular set with point and sigmas only..." << endl;
    Isis::SurfacePoint spRec;
    spRec.SetRectangular(Displacement(-424.024048, Displacement::Meters),
                         Displacement(734.4311949, Displacement::Meters),
                         Displacement(529.919264, Displacement::Meters),
                         Distance(10., Distance::Meters),
                         Distance(50., Distance::Meters),
                         Distance(20., Distance::Meters));
    double lat = (spRec.GetLatitude()).degrees();
    double lon = (spRec.GetLongitude()).degrees();
    double radius = spRec.GetLocalRadius().meters();
    double latSig = (spRec.GetLatSigma()).degrees();
    double lonSig = (spRec.GetLonSigma()).degrees();
    double radSig = spRec.GetLocalRadiusSigma().meters();
    symmetric_matrix<double,upper> covarSphere(3);
    covarSphere.clear();
    covarSphere = spRec.GetSphericalMatrix();
    
    cout << setprecision(9);
    cout << "  Output spherical..." << endl;
    cout << "    lat=" << lat << " degrees, lon=" << lon << " degrees, radius="
         << radius << "m" << endl;
    cout << "    latitude sigma=" << latSig << " degrees, longitude sigma=" 
         << lonSig
         << " degrees, radius sigma=" << radSig << "m" << endl;
    cout << "    ocentric covariance matrix = " << covarSphere(0,0) << "  " 
         << covarSphere(0,1) << "  " << covarSphere(0,2) << endl;
    cout << "                                 " << covarSphere(1,0) << "  "
         << covarSphere(1,1) << "  " << covarSphere(1,2) << endl;
    cout << "                                 " << covarSphere(2,0) << "  "
         << covarSphere(2,1) << "  " << covarSphere(2,2) << endl;

    // The next test will not match previous results because only the sigmas are set
    // and not the whole variance/covariance matrix
    cout << endl;
    cout << "4-Testing planetocentric set with point and sigmas only in degrees ..."
         << endl;
    Isis::SurfacePoint spSphere1,spSphere2;
    spSphere1.SetSpherical(Latitude(32., Angle::Degrees),
                         Longitude(120., Angle::Degrees),
                         Distance(1000., Distance::Meters),
                         Angle(1.64192315,Angle::Degrees),
                         Angle(1.78752107, Angle::Degrees),
                         Distance(38.454887335682053718134171237789,
                                  Distance::Meters));
    symmetric_matrix<double,upper> covarRec(3);
    covarRec.clear();
    covarRec = spSphere1.GetRectangularMatrix();
    spSphere2.SetSpherical(Latitude(0.55850536, Angle::Radians),
                              Longitude(2.0943951, Angle::Radians),
                              Distance(1000., Distance::Meters),
                              Angle(0.028656965, Angle::Radians),
                              Angle(0.0311981281, Angle::Radians),
                              Distance(38.4548873, Distance::Meters));

    // TODO try to convert ocentric sigmas to meters and get error to test error

    cout << "  4a-Output rectangular from degrees..." << endl;
    cout << "    x=" << spSphere1.GetX().meters() << " m, y=" <<
            spSphere1.GetY().meters()
         << " m, z=" << spSphere1.GetZ().meters() << " m" << endl;
    cout << "    X sigma =" << spSphere1.GetXSigma().meters()
         << " m, Y sigma = " << spSphere1.GetYSigma().meters()
         << " m, Z sigma = " << spSphere1.GetZSigma().meters() << "m"
         << endl;
    cout << "    rectangular covariance matrix = " << covarRec(0,0) << "  "
         << covarRec(0,1) << "  " << covarRec(0,2) << endl;
    cout << "                                    " << covarRec(1,0) << "  "
         << covarRec(1,1) << "  " << covarRec(1,2) << endl;
    cout << "                                    " << covarRec(2,0) << "  "
         << covarRec(2,1) << "  " << covarRec(2,2) << endl;

    covarRec.clear();
    covarRec = spSphere2.GetRectangularMatrix();
    cout << "  4b-Output rectangular from radians..." << endl;
    cout << "    x=" << spSphere2.GetX().meters()
         << " m, y=" << spSphere2.GetY().meters()
         << " m, z=" << spSphere2.GetZ().meters() << " m" << endl;
    cout << "    X sigma = " << spSphere2.GetXSigma().meters()
         << " m, Y sigma = " << spSphere2.GetYSigma().meters()
         << " m, Z sigma = " << spSphere2.GetZSigma().meters()
         << "m" << endl;
    cout << "    rectangular covariance matrix = " << covarRec(0,0) << "  "
         << covarRec(0,1) << "  " << covarRec(0,2) << endl;
    cout << "                                    " << covarRec(1,0) << "  "
         << covarRec(1,1) << "  " << covarRec(1,2) << endl;
    cout << "                                    " << covarRec(2,0) << "  "
         << covarRec(2,1) << "  " << covarRec(2,2) << endl;


    cout << endl << "5-Testing copy constructor" << endl;
    Isis::SurfacePoint spRec2(spSphere1);
    lat = (spRec2.GetLatitude()).degrees();
    lon = (spRec2.GetLongitude()).degrees();
    radius = spRec2.GetLocalRadius().meters();
    latSig = (spRec2.GetLatSigma()).degrees();
    lonSig = (spRec2.GetLonSigma()).degrees();
    radSig = spRec2.GetLocalRadiusSigma().meters();
    
    cout << setprecision(9);
    cout << "  Output spherical..." << endl;
    cout << "    lat=" << lat << " degrees, lon=" << lon << " degrees, radius="
         << radius << " m" << endl;
    cout << "    latitude sigma = " << latSig << " degrees, longitude sigma = "
         << lonSig << " degrees, radius sigma = " << radSig << " m" << endl;
    cout << "    ocentric covariance matrix = " << covarSphere(0,0) << "  "
         << covarSphere(0,1) << "  " << covarSphere(0,2) << endl;
    cout << "                                 " << covarSphere(1,0) << "  "
         << covarSphere(1,1) << "  " << covarSphere(1,2) << endl;
    cout << "                                 " << covarSphere(2,0) << "  "
         << covarSphere(2,1) << "  " << covarSphere(2,2) << endl << endl;

    cout << "Testing Longitude Accessor..." << endl;
    Isis::SurfacePoint spSphere3(Latitude(15, Angle::Degrees),
        Longitude(-45, Angle::Degrees), Distance(10, Distance::Kilometers));
    cout << "Longitude (from -45): " << spSphere3.GetLongitude().degrees()
         << endl << endl;

  }
  catch(Isis::IException &e) {
    e.print();
  }

  cout << "Test computational methods..." << endl;
  cout << "  SphericalDistanceToPoint (i.e. haversine): ";

  SurfacePoint point1(Latitude(0, Angle::Degrees),
      Longitude(90, Angle::Degrees), Distance(1.5, Distance::Kilometers));
  SurfacePoint point2(Latitude(0, Angle::Degrees),
      Longitude(180, Angle::Degrees), Distance(0.5, Distance::Kilometers));

  Distance result = point1.GetDistanceToPoint(point2);
  cout << result.meters() << " meters" << endl;
}







