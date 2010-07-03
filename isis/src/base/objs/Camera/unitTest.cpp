#include "Camera.h"
#include "Preference.h"
#include "CameraFactory.h"

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
  double lat = 18.221;
  double lon = 226.671;
  double ra = 347.016;
  double dec = -51.2677;

  cout << endl << "Line: " << line << ", Sample: " << sample << endl;
  cout << "Lat: " << lat << ", Lon: " << lon << endl;
  cout << "RightAscension: " << ra << ", Declination: " << dec << endl;
  cout << "Camera* from: " << inputFile << endl << endl;

  cout << "SetImage (sample, line): " << c->SetImage(sample, line) << endl << endl;

  cout << "NorthAzimuth: " << c->NorthAzimuth() << endl;
  cout << "SunAzimuth: " << c->SunAzimuth() << endl;
  cout << "SpacecraftAzimuth: " << c->SpacecraftAzimuth() << endl;
  cout << "OffNadirAngle: " << c->OffNadirAngle() << endl << endl;

  cout << "SetUniversalGround(lat, lon): "
       << c->SetUniversalGround(lat, lon) << endl;
  cout << "SetRightAscensionDeclination(ra, dec): "
       << c->SetRightAscensionDeclination(ra, dec) << endl;
  cout << "HasProjection: " << c->HasProjection() << endl;
  cam.IsBandIndependent();
  cout << "ReferenceBand: " << c->ReferenceBand() << endl;
  cout << "HasReferenceBand: " << c->HasReferenceBand() << endl;
  cam.SetBand(7);
  cout << "Sample: " << c->Sample() << endl;
  cout << "Line: " << c->Line() << endl;

  try {
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
  cout << "LowestImageResolution: " << c->LowestImageResolution() << endl;
  cout << "HighestImageResolution: " << c->HighestImageResolution() << endl;
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
  lat = 18.221;
  lon = 226.671;
  double radius = 3420.;
  c->SetUniversalGround(lat, lon, radius);
  cout << "Has intersection " << c->HasSurfaceIntersection() << endl;
  cout << "Latitude = " << c->UniversalLatitude() << endl;
  cout << "Longitude = " << c->UniversalLongitude() << endl;
  cout << "Radius = " << c->LocalRadius() << endl;
  double p[3];
  c->Coordinate(p);
  cout << "Point = " << p[0] << " " << p[1] << " " << p[2] << endl;

  std::cout << std::endl;
  std::cout << "Test Polar Boundary Conditions" << std::endl;
  inputFile = "$clementine1/testData/lub5992r.292.lev1.phot.cub";
  cube.Close();
  cube.Open(inputFile);
  pvl = *cube.Label();
  Camera *cam2 = CameraFactory::Create(pvl);
  cube.Close();

  std::cout << std::endl;
  std::cout << "Basic Mapping: " << std::endl;
  Pvl camMap;
  cam2->BasicMapping(camMap);
  std::cout << camMap << std::endl;

  std::cout << std::endl;
  std::cout << "180 Domain Range: " << std::endl;
  double minlat, maxlat, minlon, maxlon;
  camMap.FindGroup("Mapping")["LongitudeDomain"][0] = "180";
  cam2->GroundRange(minlat, maxlat, minlon, maxlon, camMap);
  std::cout << "Latitude Range: " << minlat << " to " << maxlat << std::endl;
  std::cout << "Longitude Range: " << minlon << " to " << maxlon << std::endl;

  cube.Close();
  delete cam2;
}
