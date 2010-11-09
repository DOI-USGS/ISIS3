#include "Camera.h"

#include <iostream>
#include <iomanip>

#include "CameraFactory.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

class MyCamera : public Isis::Camera {
  public:
    MyCamera(Isis::Pvl &lab) : Isis::Camera(lab) { }

    bool IsBandIndependent() {
      cout << "IsBandIndependent called..." << endl;
      return true;
    }

    void SetBand(const int band) {
      cout << "SetBand called, band: " << band << endl;
    }

    virtual CameraType GetCameraType() const {
      return Framing;
    }
};

int main() {
  Isis::Preference::Preferences(true);
  string inputFile = "$ISIS3DATA/mgs/testData/ab102401.lev2.cub";
  Cube cube;
  cube.Open(inputFile);
  Camera *c = cube.Camera();
  Pvl &pvl = *cube.Label();
  MyCamera cam(pvl);

  double line = 453.0;
  double sample = 534.0;
  Latitude lat(18.221, Angle::Degrees);
  Longitude lon(226.671, Angle::Degrees);
  double ra = 347.016;
  double dec = -51.2677;

  cout << endl << "Line: " << line << ", Sample: " << sample << endl;
  cout << "Lat: " << lat.GetDegrees() << ", Lon: " << lon.GetDegrees() << endl;
  cout << "RightAscension: " << ra << ", Declination: " << dec << endl;
  cout << "Camera* from: " << inputFile << endl << endl;

  cout << "SetImage (sample, line): " << c->SetImage(sample, line)
       << endl << endl;

  cout << "NorthAzimuth: " << c->NorthAzimuth() << endl;
  cout << "SunAzimuth: " << c->SunAzimuth() << endl;
  cout << "SpacecraftAzimuth: " << c->SpacecraftAzimuth() << endl;
  cout << "OffNadirAngle: " << c->OffNadirAngle() << endl << endl;

  cout << "SetUniversalGround(lat, lon): "
       << c->SetGround(lat, lon) << endl;
  cout << "SetRightAscensionDeclination(ra, dec): "
       << c->SetRightAscensionDeclination(ra, dec) << endl;
  cout << "HasProjection: " << c->HasProjection() << endl;
  cam.IsBandIndependent();
  cout << "ReferenceBand: " << c->ReferenceBand() << endl;
  cout << "HasReferenceBand: " << c->HasReferenceBand() << endl;
  cam.SetBand(7);
  cout << "Sample: " << setprecision(3) << c->Sample() << endl;
  cout << "Line: " << setprecision(3) << c->Line() << endl;

  try {
    double lat = 0, lon = 0;
    cout << "GroundRange: "
         << c->GroundRange(lat, lat, lon, lon, pvl) << endl;
    cout << "IntersectsLongitudeDomain: "
         << c->IntersectsLongitudeDomain(pvl) << endl;
  }
  catch(iException &e) {
    e.Clear();
    cout << "No mapping group found, so GroundRange and " << endl
         << "IntersectsLongitudeDomain cannot run." << endl;
  }

  cout << "PixelResolution: " << c->PixelResolution() << endl;
  cout << "LineResolution: " << c->LineResolution() << endl;
  cout << "SampleResolution: " << c->SampleResolution() << endl;
  cout << "DetectorResolution: " << c->DetectorResolution() << endl;
  cout << "LowestImageResolution: " << setprecision(4)
       << c->LowestImageResolution() << endl;
  cout << "HighestImageResolution: " << setprecision(3)
       << c->HighestImageResolution() << endl;
  cout << "Calling BasicMapping (pvl)..." << endl;
  c->BasicMapping(pvl);
  cout << "BasicMapping PVL: " << endl << pvl << endl << endl;
  cout << "FocalLength: " << c->FocalLength() << endl;
  cout << "PixelPitch: " << c->PixelPitch() << endl;
  cout << "Samples: " << c->Samples() << endl;
  cout << "Lines: " << c->Lines() << endl;
  cout << "Bands: " << c->Bands() << endl;
  cout << "ParentLines: " << c->ParentLines() << endl;
  cout << "ParentSamples: " << c->ParentSamples() << endl;


  try {
    cout << c->RaDecRange(ra, ra, dec, dec) << endl;
  }
  catch(iException &e) {
    e.Report(false);
    e.Clear();
  }

  try {
    cout << c->RaDecResolution() << endl;
  }
  catch(iException &e) {
    e.Report(false);
    e.Clear();
  }

  cout << "Calling Distortion, FocalPlane, ";
  cout << "Detector, Ground, and Sky Map functions... ";
  c->DistortionMap();
  c->FocalPlaneMap();
  c->DetectorMap();
  c->GroundMap();
  c->SkyMap();
  cout << "Done." << endl;

  cout << "Calling IgnoreProjection (false)..." << endl;
  c->IgnoreProjection(false);

  cout << endl << "Testing SetUniversalGround(lat,lon,radius)..." << endl;
  lat.SetDegrees(18.221);
  lon.SetDegrees(226.671);
  double radius = 3420.;
  c->SetUniversalGround(lat.GetDegrees(), lon.GetDegrees(), radius);
  //c->SetGround(SurfacePoint(lat, lon, radius));
  cout << "Has intersection " << c->HasSurfaceIntersection() << endl;
  cout << "Latitude = " << c->UniversalLatitude() << endl;
  cout << "Longitude = " << c->UniversalLongitude() << endl;
  cout << "Radius = " << c->LocalRadius() << endl;
  double p[3];
  c->Coordinate(p);
  cout << "Point = " << setprecision(4)
       << p[0] << " " << p[1] << " " << p[2] << endl << endl;

  cout << "Test Forward/Reverse Camera Calculations At Center Of Image..."
       << endl;
  sample = c->Samples() / 2.0;
  line = c->Lines() / 2.0;
  cout << "Sample = " << setprecision(3) << sample << endl;
  cout << "Line = " << line << endl;
  cout << "SetImage (sample, line): " << c->SetImage(sample, line) << endl;
  cout << "Latitude = " << c->UniversalLatitude() << endl;
  cout << "Longitude = " << c->UniversalLongitude() << endl;
  cout << "Radius = " << c->LocalRadius() << endl;
  c->Coordinate(p);
  cout << "Point = " << setprecision(4)
       << p[0] << " " << p[1] << " " << p[2] << endl;
  cout << "SetUniversalGround (lat, lon, radius): "
       << c->SetUniversalGround(c->UniversalLatitude(), c->UniversalLongitude(),
                                c->LocalRadius())
       << endl;
  cout << "Sample = " << c->Sample() << endl;
  cout << "Line = " << c->Line() << endl << endl;

  cout << "Test Polar Boundary Conditions" << endl;
  inputFile = "$clementine1/testData/lub5992r.292.lev1.phot.cub";
  cube.Close();
  cube.Open(inputFile);
  pvl = *cube.Label();
  Camera *cam2 = CameraFactory::Create(pvl);
  cube.Close();

  cout << endl;
  cout << "Basic Mapping: " << endl;
  Pvl camMap;
  cam2->BasicMapping(camMap);

  double minLat = camMap.FindGroup("Mapping")["MinimumLatitude"];
  minLat *= 100;
  minLat = round(minLat);
  minLat /= 100;
  camMap.FindGroup("Mapping")["MinimumLatitude"] = minLat;

  double pixRes = camMap.FindGroup("Mapping")["PixelResolution"];
  pixRes *= 100;
  pixRes = round(pixRes);
  pixRes /= 100;
  camMap.FindGroup("Mapping")["PixelResolution"] = pixRes;

  cout << camMap << endl;

  cout << endl;
  cout << "180 Domain Range: " << endl;
  double minlat, maxlat, minlon, maxlon;
  camMap.FindGroup("Mapping")["LongitudeDomain"][0] = "180";
  cam2->GroundRange(minlat, maxlat, minlon, maxlon, camMap);
  cout << "Latitude Range: " << minlat << " to " << maxlat << endl;
  cout << "Longitude Range: " << minlon << " to " << maxlon
            << endl << endl;

  cout << "Test Forward/Reverse Camera Calculations At Center Of Image..."
       << endl;
  sample = cam2->Samples() / 2.0;
  line = cam2->Lines() / 2.0;
  cout << "Sample = " << sample << endl;
  cout << "Line = " << line << endl;
  cout << "SetImage (sample, line): " << cam2->SetImage(sample, line) << endl;
  cout << "Latitude = " << cam2->UniversalLatitude() << endl;
  cout << "Longitude = " << cam2->UniversalLongitude() << endl;
  cout << "Radius = " << cam2->LocalRadius() << endl;
  cam2->Coordinate(p);
  cout << "Point = " << p[0] << " " << p[1] << " " << p[2] << endl;
  cout << "SetUniversalGround (cam2->UniversalLatitude(), "
          "cam2->UniversalLongitude()): "
       << cam2->SetUniversalGround(cam2->UniversalLatitude(),
                                   cam2->UniversalLongitude())
       << endl;
  cout << "Sample = " << cam2->Sample() << endl;
  cout << "Line = " << cam2->Line() << endl << endl;

  cube.Close();
  delete cam2;
}
