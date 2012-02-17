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

#include "Angle.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "Cube.h"
#include "iException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Preference.h"
#include "Pvl.h"
#include "SurfacePoint.h"

using namespace std;
using namespace Isis;

class MyCamera : public Camera {
  public:
    MyCamera(Pvl &lab) : Camera(lab) { }

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

    virtual int CkFrameId() const { return (-94000); }   
    virtual int CkReferenceId() const { return (1); }
    virtual int SpkReferenceId() const { return (1); }
};

int main() {
  Preference::Preferences(true);
  string inputFile = "$mgs/testData/ab102401.lev2.cub";
  Cube cube;
  cube.open(inputFile);
  Camera *c = NULL;
  c = cube.getCamera();
  Pvl pvl = *cube.getLabel();
  MyCamera cam(pvl);
  double line = 453.0;
  double sample = 534.0;
  Latitude lat(18.221, Angle::Degrees);
  Longitude lon(226.671, Angle::Degrees);
  double ra = 347.016;
  double dec = -51.2677;

  cout << endl << "Camera* from: " << inputFile << endl;
  cout << "Line: " << line << ", Sample: " << sample << endl;
  cout << "Lat: " << lat.degrees() << ", Lon: " << lon.degrees() << endl;
  cout << "RightAscension: " << ra << ", Declination: " << dec << endl << endl;

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

  double pixRes2 = pvl.FindGroup("Mapping")["PixelResolution"];
  pixRes2 *= 10000000;
  pixRes2 = round(pixRes2);
  pixRes2 /= 10000000;
  pvl.FindGroup("Mapping")["PixelResolution"] = pixRes2;

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
  lat.setDegrees(18.221);
  lon.setDegrees(226.671);
  double radius = 3414033.72108798;
  c->SetUniversalGround(lat.degrees(), lon.degrees(), radius);
  c->SetGround(SurfacePoint(lat, lon, Distance(radius, Distance::Meters)));
  cout << "Has intersection " << c->HasSurfaceIntersection() << endl;
  cout << "Latitude = " << c->UniversalLatitude() << endl;
  cout << "Longitude = " << c->UniversalLongitude() << endl;
  cout << "Radius = " << c->LocalRadius().meters() << endl;
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
  cout << "Radius = " << c->LocalRadius().meters() << endl;
  c->Coordinate(p);
  cout << "Point = " << setprecision(4)
       << p[0] << " " << p[1] << " " << p[2] << endl;
  cout << "SetUniversalGround (lat, lon, radius): "
       << c->SetUniversalGround(c->UniversalLatitude(), c->UniversalLongitude(),
                                c->LocalRadius().meters())
       << endl;
  cout << "Sample = " << c->Sample() << endl;
  cout << "Line = " << c->Line() << endl << endl;

  cout << endl << "/---------- Test Polar Boundary Conditions" << endl;
  inputFile = "$clementine1/testData/lub5992r.292.lev1.phot.cub";
  cube.close();
  cube.open(inputFile);
  pvl = *cube.getLabel();
  Camera *cam2 = CameraFactory::Create(pvl);
  cube.close();

  cout << endl;
  cout << "Camera* from: " << inputFile << endl;
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

  double minLon = camMap.FindGroup("Mapping")["MinimumLongitude"];
  minLon *= 100000000000.0;
  minLon = round(minLon);
  minLon /= 100000000000.0;
  camMap.FindGroup("Mapping")["MinimumLongitude"] = minLon;

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
  cout << "Radius = " << cam2->LocalRadius().meters() << endl;
  cam2->Coordinate(p);
  cout << "Point = " << p[0] << " " << p[1] << " " << p[2] << endl;
  cout << "SetUniversalGround (cam2->UniversalLatitude(), "
          "cam2->UniversalLongitude()): "
       << cam2->SetUniversalGround(cam2->UniversalLatitude(),
                                   cam2->UniversalLongitude())
       << endl;
  cout << "Sample = " << cam2->Sample() << endl;
  cout << "Line = " << cam2->Line() << endl << endl;

  cube.close();
  delete cam2;

  cube.close();
  cout << endl << "/---------- Test Local Photometric Angles..." << endl << endl;
  cout << "Flat DEM Surface..." << endl;
  inputFile = "$base/testData/f319b18_ideal_flat.cub";
  cube.open(inputFile);
  pvl = *cube.getLabel();
  Camera *cam3 = CameraFactory::Create(pvl);
  cube.close();

  sample = cam3->Samples() / 2.0;
  line = cam3->Lines() / 2.0;
  cout << "Camera* from: " << inputFile << endl;
  cout << "Sample = " << sample << endl;
  cout << "Line = " << line << endl;
  cout << "SetImage (sample, line): " << cam3->SetImage(sample, line) << endl;
  double normal[3];
  cam3->GetLocalNormal(normal);
  cout << "Normal = " << normal[0] << ", " << normal[1] << ", " << normal[2] << endl;
  Angle phase;
  Angle incidence;
  Angle emission;
  bool success;
  cam3->LocalPhotometricAngles(phase,emission,incidence,success);
  if (success) {
    cout << "Phase = " << phase.degrees() << endl;
    cout << "Emission = " << emission.degrees() << endl;
    cout << "Incidence = " << incidence.degrees() << endl;
  } else {
    cout << "Angles could not be calculated." << endl;
  }
  delete cam3;

  cout << endl << "45 Degree DEM Surface Facing Left..." << endl;
  inputFile = "$base/testData/f319b18_ideal_45left.cub";
  cube.open(inputFile);
  pvl = *cube.getLabel();
  Camera *cam4 = CameraFactory::Create(pvl);
  cube.close();

  sample = cam4->Samples() / 2.0;
  line = cam4->Lines() / 2.0;
  cout << "Camera* from: " << inputFile << endl;
  cout << "Sample = " << sample << endl;
  cout << "Line = " << line << endl;
  cout << "SetImage (sample, line): " << cam4->SetImage(sample, line) << endl;
  cam4->GetLocalNormal(normal);
  cout << "Normal = " << normal[0] << ", " << normal[1] << ", " << normal[2] << endl;
  cam4->LocalPhotometricAngles(phase,emission,incidence,success);
  if (success) {
    cout << "Phase = " << phase.degrees() << endl;
    cout << "Emission = " << emission.degrees() << endl;
    cout << "Incidence = " << incidence.degrees() << endl;
  } else {
    cout << "Angles could not be calculated." << endl;
  }
  delete cam4;

  cout << endl << "45 Degree DEM Surface Facing Top..." << endl;
  inputFile = "$base/testData/f319b18_ideal_45top.cub";
  cube.open(inputFile);
  pvl = *cube.getLabel();
  Camera *cam5 = CameraFactory::Create(pvl);
  cube.close();

  sample = cam5->Samples() / 2.0;
  line = cam5->Lines() / 2.0;
  cout << "Camera* from: " << inputFile << endl;
  cout << "Sample = " << sample << endl;
  cout << "Line = " << line << endl;
  cout << "SetImage (sample, line): " << cam5->SetImage(sample, line) << endl;
  cam5->GetLocalNormal(normal);
  cout << "Normal = " << normal[0] << ", " << normal[1] << ", " << normal[2] << endl;
  cam5->LocalPhotometricAngles(phase,emission,incidence,success);
  if (success) {
    cout << "Phase = " << phase.degrees() << endl;
    cout << "Emission = " << emission.degrees() << endl;
    cout << "Incidence = " << incidence.degrees() << endl;
  } else {
    cout << "Angles could not be calculated." << endl;
  }
  delete cam5;

  cout << endl << "45 Degree DEM Surface Facing Right..." << endl;
  inputFile = "$base/testData/f319b18_ideal_45right.cub";
  cube.open(inputFile);
  pvl = *cube.getLabel();
  Camera *cam6 = CameraFactory::Create(pvl);
  cube.close();

  sample = cam6->Samples() / 2.0;
  line = cam6->Lines() / 2.0;
  cout << "Camera* from: " << inputFile << endl;
  cout << "Sample = " << sample << endl;
  cout << "Line = " << line << endl;
  cout << "SetImage (sample, line): " << cam6->SetImage(sample, line) << endl;
  cam6->GetLocalNormal(normal);
  cout << "Normal = " << normal[0] << ", " << normal[1] << ", " << normal[2] << endl;
  cam6->LocalPhotometricAngles(phase,emission,incidence,success);
  if (success) {
    cout << "Phase = " << phase.degrees() << endl;
    cout << "Emission = " << emission.degrees() << endl;
    cout << "Incidence = " << incidence.degrees() << endl;
  } else {
    cout << "Angles could not be calculated." << endl;
  }
  delete cam6;

  cout << endl << "45 Degree DEM Surface Facing Bottom..." << endl;
  inputFile = "$base/testData/f319b18_ideal_45bottom.cub";
  cube.open(inputFile);
  pvl = *cube.getLabel();
  Camera *cam7 = CameraFactory::Create(pvl);
  cube.close();

  sample = cam7->Samples() / 2.0;
  line = cam7->Lines() / 2.0;
  cout << "Camera* from: " << inputFile << endl;
  cout << "Sample = " << sample << endl;
  cout << "Line = " << line << endl;
  cout << "SetImage (sample, line): " << cam7->SetImage(sample, line) << endl;
  cam7->GetLocalNormal(normal);
  cout << "Normal = " << normal[0] << ", " << normal[1] << ", " << normal[2] << endl;
  cam7->LocalPhotometricAngles(phase,emission,incidence,success);
  if (success) {
    cout << "Phase = " << phase.degrees() << endl;
    cout << "Emission = " << emission.degrees() << endl;
    cout << "Incidence = " << incidence.degrees() << endl;
  } else {
    cout << "Angles could not be calculated." << endl;
  }
  delete cam7;

  cout << endl << "80 Degree DEM Surface Facing Left..." << endl;
  inputFile = "$base/testData/f319b18_ideal_80left.cub";
  cube.open(inputFile);
  pvl = *cube.getLabel();
  Camera *cam8 = CameraFactory::Create(pvl);
  cube.close();

  sample = cam8->Samples() / 2.0;
  line = cam8->Lines() / 2.0;
  cout << "Camera* from: " << inputFile << endl;
  cout << "Sample = " << sample << endl;
  cout << "Line = " << line << endl;
  cout << "SetImage (sample, line): " << cam8->SetImage(sample, line) << endl;
  cam8->GetLocalNormal(normal);
  cout << "Normal = " << normal[0] << ", " << normal[1] << ", " << normal[2] << endl;
  cam8->LocalPhotometricAngles(phase,emission,incidence,success);
  if (success) {
    cout << "Phase = " << phase.degrees() << endl;
    cout << "Emission = " << emission.degrees() << endl;
    cout << "Incidence = " << incidence.degrees() << endl;
  } else {
    cout << "Angles could not be calculated." << endl;
  }
  delete cam8;

  cout << endl << "80 Degree DEM Surface Facing Top..." << endl;
  inputFile = "$base/testData/f319b18_ideal_80top.cub";
  cube.open(inputFile);
  pvl = *cube.getLabel();
  Camera *cam9 = CameraFactory::Create(pvl);
  cube.close();

  sample = cam9->Samples() / 2.0;
  line = cam9->Lines() / 2.0;
  cout << "Camera* from: " << inputFile << endl;
  cout << "Sample = " << sample << endl;
  cout << "Line = " << line << endl;
  cout << "SetImage (sample, line): " << cam9->SetImage(sample, line) << endl;
  cam9->GetLocalNormal(normal);
  cout << "Normal = " << normal[0] << ", " << normal[1] << ", " << normal[2] << endl;
  cam9->LocalPhotometricAngles(phase,emission,incidence,success);
  if (success) {
    cout << "Phase = " << phase.degrees() << endl;
    cout << "Emission = " << emission.degrees() << endl;
    cout << "Incidence = " << incidence.degrees() << endl;
  } else {
    cout << "Angles could not be calculated." << endl;
  }
  delete cam9;

  cout << endl << "80 Degree DEM Surface Facing Right..." << endl;
  inputFile = "$base/testData/f319b18_ideal_80right.cub";
  cube.open(inputFile);
  pvl = *cube.getLabel();
  Camera *cam10 = CameraFactory::Create(pvl);
  cube.close();

  sample = cam10->Samples() / 2.0;
  line = cam10->Lines() / 2.0;
  cout << "Camera* from: " << inputFile << endl;
  cout << "Sample = " << sample << endl;
  cout << "Line = " << line << endl;
  cout << "SetImage (sample, line): " << cam10->SetImage(sample, line) << endl;
  cam10->GetLocalNormal(normal);
  cout << "Normal = " << normal[0] << ", " << normal[1] << ", " << normal[2] << endl;
  cam10->LocalPhotometricAngles(phase,emission,incidence,success);
  if (success) {
    cout << "Phase = " << phase.degrees() << endl;
    cout << "Emission = " << emission.degrees() << endl;
    cout << "Incidence = " << incidence.degrees() << endl;
  } else {
    cout << "Angles could not be calculated." << endl;
  }
  delete cam10;

  cout << endl << "80 Degree DEM Surface Facing Bottom..." << endl;
  inputFile = "$base/testData/f319b18_ideal_80bottom.cub";
  cube.open(inputFile);
  pvl = *cube.getLabel();
  Camera *cam11 = CameraFactory::Create(pvl);
  cube.close();

  sample = cam11->Samples() / 2.0;
  line = cam11->Lines() / 2.0;
  cout << "Camera* from: " << inputFile << endl;
  cout << "Sample = " << sample << endl;
  cout << "Line = " << line << endl;
  cout << "SetImage (sample, line): " << cam11->SetImage(sample, line) << endl;
  cam11->GetLocalNormal(normal);
  cout << "Normal = " << normal[0] << ", " << normal[1] << ", " << normal[2] << endl;
  cam11->LocalPhotometricAngles(phase,emission,incidence,success);
  if (success) {
    cout << "Phase = " << phase.degrees() << endl;
    cout << "Emission = " << emission.degrees() << endl;
    cout << "Incidence = " << incidence.degrees() << endl;
  } else {
    cout << "Angles could not be calculated." << endl;
  }
  delete cam11;

  cout << endl << "Point Does Not Intersect DEM..." << endl;
  inputFile = "$base/testData/f319b18_ideal_flat.cub";
  cube.open(inputFile);
  pvl = *cube.getLabel();
  Camera *cam12 = CameraFactory::Create(pvl);
  cube.close();

  sample = 1.0;
  line = 1.0;
  cout << "Camera* from: " << inputFile << endl;
  cout << "Sample = " << sample << endl;
  cout << "Line = " << line << endl;
  cout << "SetImage (sample, line): " << cam12->SetImage(sample, line) << endl;
  cam12->GetLocalNormal(normal);
  cout << "Normal = " << normal[0] << ", " << normal[1] << ", " << normal[2] << endl;
  cam12->LocalPhotometricAngles(phase,emission,incidence,success);
  if (success) {
    cout << "Phase = " << phase.degrees() << endl;
    cout << "Emission = " << emission.degrees() << endl;
    cout << "Incidence = " << incidence.degrees() << endl;
  } else {
    cout << "Angles could not be calculated." << endl;
  }
  delete cam12;
}
