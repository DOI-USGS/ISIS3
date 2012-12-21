/**
 * @file
 * $Revision: 1.7 $
 * $Date: 2010/06/07 22:42:38 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include "CameraPointInfo.h"

#include "Brick.h"
#include "Camera.h"
#include "CubeManager.h"
#include "Distance.h"
#include "IException.h"
#include "iTime.h"
#include "Longitude.h"
#include "Projection.h"
#include "PvlGroup.h"

using namespace std;

namespace Isis {


  /**
   * Constructor, initializes CubeManager and other variables for
   * use.
   *
   */
  CameraPointInfo::CameraPointInfo() {
    usedCubes = NULL;
    usedCubes = new CubeManager();
    usedCubes->SetNumOpenCubes(50);
    currentCube = NULL;
    camera = NULL;
  }

  /**
   * Destructor, deletes CubeManager object used.
   *
   */
  CameraPointInfo::~CameraPointInfo() {
    if(usedCubes) {
      delete usedCubes;
      usedCubes = NULL;
    }
  }


  /**
   * SetCube opens the given cube in a CubeManager. The
   * CubeManager is for effeciency when working with control nets
   * where cubes are accesed multiple times.
   *
   * @param cubeFileName A cube filename
   */
  void CameraPointInfo::SetCube(const QString &cubeFileName) {
    currentCube = usedCubes->OpenCube(cubeFileName);
    camera = currentCube->getCamera();
  }


  /**
   * SetImage sets a sample, line image coordinate in the camera
   * so data can be accessed.
   *
   * @param sample A sample coordinate in or almost in the cube
   * @param line A line coordinate in or almost in the cubei
   *
   * @return PvlGroup* The pertinent data from the Camera class on
   *         the point. Ownership is passed to caller.
   */
  PvlGroup *CameraPointInfo::SetImage(const double sample, const double line,
                                      const bool outside, const bool errors) {
    if(CheckCube()) {
      bool passed = camera->SetImage(sample, line);
      return GetPointInfo(passed, outside, errors);
    }
    // Should never get here, error will be thrown in CheckCube()
    return NULL;
  }

  /**
   * SetCenter sets the image coordinates to the center of the image.
   *
   * @return PvlGroup* The pertinent data from the Camera class on
   *         the point. Ownership is passed to caller.
   */
  PvlGroup *CameraPointInfo::SetCenter(const bool outside, const bool errors) {
    if(CheckCube()) {
      bool passed = camera->SetImage(currentCube->getSampleCount() / 2.0, currentCube->getLineCount() / 2.0);
      return GetPointInfo(passed, outside, errors);
    }
    // Should never get here, error will be thrown in CheckCube()
    return NULL;
  }


  /**
   * SetSample sets the image coordinates to the center line and the
   * given sample.
   *
   * @return PvlGroup* The pertinent data from the Camera class on
   *         the point. Ownership is passed to caller.
   */
  PvlGroup *CameraPointInfo::SetSample(const double sample,
                                       const bool outside, const bool errors) {
    if(CheckCube()) {
      bool passed = camera->SetImage(sample, currentCube->getLineCount() / 2.0);
      return GetPointInfo(passed, outside, errors);
    }
    // Should never get here, error will be thrown in CheckCube()
    return NULL;
  }


  /**
   * SetLine sets the image coordinates to the center sample and the
   * given line.
   *
   * @return PvlGroup* The pertinent data from the Camera class on
   *         the point. Ownership is passed to caller.
   */
  PvlGroup *CameraPointInfo::SetLine(const double line,
                                     const bool outside, const bool errors) {
    if(CheckCube()) {
      bool passed = camera->SetImage(currentCube->getSampleCount() / 2.0, line);
      return GetPointInfo(passed, outside, errors);
    }
    // Should never get here, error will be thrown in CheckCube()
    return NULL;
  }


  /**
   * SetGround sets a latitude, longitude grrund coordinate in the
   * camera so data can be accessed.
   *
   * @param latitude A latitude coordinate in or almost in the
   *                 cube
   * @param longitude A longitude coordinate in or almost in the
   *                  cube
   *
   * @return PvlGroup* The pertinent data from the Camera class on
   *         the point. Ownership is passed to caller.
   */
  PvlGroup *CameraPointInfo::SetGround(const double latitude, const double longitude,
                                       const bool outside, const bool errors) {
    if(CheckCube()) {
      bool passed = camera->SetUniversalGround(latitude, longitude);
      return GetPointInfo(passed, outside, errors);
    }
    // Should never get here, error will be thrown in CheckCube()
    return NULL;
  }


  /**
   * CheckCube checks that a cube has been set before the data for
   * a point is accessed.
   *
   * @return bool Whether or not a cube has been set, true if it has been.
   */
  bool CameraPointInfo::CheckCube() {
    if(currentCube == NULL) {
      string msg = "Please set a cube before setting parameters";
      throw IException(IException::Programmer, msg, _FILEINFO_);
      return false;
    }
    return true;
  }

  /**
   * GetPointInfo builds the PvlGroup containing all the important
   * information derived from the Camera.
   *
   * @return PvlGroup* Data taken directly from the Camera and
   *         drived from Camera information. Ownership passed.
   */
  PvlGroup *CameraPointInfo::GetPointInfo(bool passed, bool allowOutside, bool allowErrors) {
    PvlGroup *gp = new PvlGroup("GroundPoint");
    {
      gp->AddKeyword(PvlKeyword("Filename"));
      gp->AddKeyword(PvlKeyword("Sample"));
      gp->AddKeyword(PvlKeyword("Line"));
      gp->AddKeyword(PvlKeyword("PixelValue"));
      gp->AddKeyword(PvlKeyword("RightAscension"));
      gp->AddKeyword(PvlKeyword("Declination"));
      gp->AddKeyword(PvlKeyword("PlanetocentricLatitude"));
      gp->AddKeyword(PvlKeyword("PlanetographicLatitude"));
      gp->AddKeyword(PvlKeyword("PositiveEast360Longitude"));
      gp->AddKeyword(PvlKeyword("PositiveEast180Longitude"));
      gp->AddKeyword(PvlKeyword("PositiveWest360Longitude"));
      gp->AddKeyword(PvlKeyword("PositiveWest180Longitude"));
      gp->AddKeyword(PvlKeyword("BodyFixedCoordinate"));
      gp->AddKeyword(PvlKeyword("LocalRadius"));
      gp->AddKeyword(PvlKeyword("SampleResolution"));
      gp->AddKeyword(PvlKeyword("LineResolution"));
      gp->AddKeyword(PvlKeyword("SpacecraftPosition"));
      gp->AddKeyword(PvlKeyword("SpacecraftAzimuth"));
      gp->AddKeyword(PvlKeyword("SlantDistance"));
      gp->AddKeyword(PvlKeyword("TargetCenterDistance"));
      gp->AddKeyword(PvlKeyword("SubSpacecraftLatitude"));
      gp->AddKeyword(PvlKeyword("SubSpacecraftLongitude"));
      gp->AddKeyword(PvlKeyword("SpacecraftAltitude"));
      gp->AddKeyword(PvlKeyword("OffNadirAngle"));
      gp->AddKeyword(PvlKeyword("SubSpacecraftGroundAzimuth"));
      gp->AddKeyword(PvlKeyword("SunPosition"));
      gp->AddKeyword(PvlKeyword("SubSolarAzimuth"));
      gp->AddKeyword(PvlKeyword("SolarDistance"));
      gp->AddKeyword(PvlKeyword("SubSolarLatitude"));
      gp->AddKeyword(PvlKeyword("SubSolarLongitude"));
      gp->AddKeyword(PvlKeyword("SubSolarGroundAzimuth"));
      gp->AddKeyword(PvlKeyword("Phase"));
      gp->AddKeyword(PvlKeyword("Incidence"));
      gp->AddKeyword(PvlKeyword("Emission"));
      gp->AddKeyword(PvlKeyword("NorthAzimuth"));
      gp->AddKeyword(PvlKeyword("EphemerisTime"));
      gp->AddKeyword(PvlKeyword("UTC"));
      gp->AddKeyword(PvlKeyword("LocalSolarTime"));
      gp->AddKeyword(PvlKeyword("SolarLongitude"));
      if(allowErrors) gp->AddKeyword(PvlKeyword("Error"));
    }

    bool noErrors = passed;
    QString error = "";
    if(!camera->HasSurfaceIntersection()) {
      error = "Requested position does not project in camera model; no surface intersection";
      noErrors = false;
      if(!allowErrors) throw IException(IException::Unknown, error, _FILEINFO_);
    }
    if(!camera->InCube() && !allowOutside) {
      error = "Requested position does not project in camera model; not inside cube";
      noErrors = false;
      if(!allowErrors) throw IException(IException::Unknown, error, _FILEINFO_);
    }

    if(!noErrors) {
      for(int i = 0; i < gp->Keywords(); i++) {
        QString name = (*gp)[i].Name();
        // These three keywords have 3 values, so they must have 3 N/As
        if(name == "BodyFixedCoordinate" || name == "SpacecraftPosition" ||
            name == "SunPosition") {
          (*gp)[i].AddValue("N/A");
          (*gp)[i].AddValue("N/A");
          (*gp)[i].AddValue("N/A");
        }
        else {
          (*gp)[i].SetValue("N/A");
        }
      }
      // Set all keywords that still have valid information
      gp->FindKeyword("Error").SetValue(error);
      gp->FindKeyword("FileName").SetValue(currentCube->getFileName());
      gp->FindKeyword("Sample").SetValue(toString(camera->Sample()));
      gp->FindKeyword("Line").SetValue(toString(camera->Line()));
      gp->FindKeyword("EphemerisTime").SetValue(toString(camera->time().Et()), "seconds");
      gp->FindKeyword("EphemerisTime").AddComment("Time");
      QString utc = camera->time().UTC();
      gp->FindKeyword("UTC").SetValue(utc);
      gp->FindKeyword("SpacecraftPosition").AddComment("Spacecraft Information");
      gp->FindKeyword("SunPosition").AddComment("Sun Information");
      gp->FindKeyword("Phase").AddComment("Illumination and Other");
    }

    else {

      Brick b(3, 3, 1, currentCube->getPixelType());

      int intSamp = (int)(camera->Sample() + 0.5);
      int intLine = (int)(camera->Line() + 0.5);
      b.SetBasePosition(intSamp, intLine, 1);
      currentCube->read(b);

      double pB[3], spB[3], sB[3];
      QString utc;
      double ssplat, ssplon, sslat, sslon, pwlon, oglat;

      {
        gp->FindKeyword("FileName").SetValue(currentCube->getFileName());
        gp->FindKeyword("Sample").SetValue(toString(camera->Sample()));
        gp->FindKeyword("Line").SetValue(toString(camera->Line()));
        gp->FindKeyword("PixelValue").SetValue(PixelToString(b[0]));
        gp->FindKeyword("RightAscension").SetValue(toString(camera->RightAscension()));
        gp->FindKeyword("Declination").SetValue(toString(camera->Declination()));
        gp->FindKeyword("PlanetocentricLatitude").SetValue(toString(camera->UniversalLatitude()));

        // Convert lat to planetographic
        Distance radii[3];
        camera->radii(radii);
        oglat = Isis::Projection::ToPlanetographic(camera->UniversalLatitude(),
                radii[0].kilometers(), radii[2].kilometers());
        gp->FindKeyword("PlanetographicLatitude").SetValue(toString(oglat));

        gp->FindKeyword("PositiveEast360Longitude").SetValue(toString(
          camera->UniversalLongitude()));

        //Convert lon to -180 - 180 range
        gp->FindKeyword("PositiveEast180Longitude").SetValue(toString(
          Isis::Projection::To180Domain(
            camera->UniversalLongitude())));

        //Convert lon to positive west
        pwlon = Isis::Projection::ToPositiveWest(camera->UniversalLongitude(),
                360);
        gp->FindKeyword("PositiveWest360Longitude").SetValue(toString(pwlon));

        //Convert pwlon to -180 - 180 range
        gp->FindKeyword("PositiveWest180Longitude").SetValue(toString(
          Isis::Projection::To180Domain(pwlon)));

        camera->Coordinate(pB);
        gp->FindKeyword("BodyFixedCoordinate").AddValue(toString(pB[0]), "km");
        gp->FindKeyword("BodyFixedCoordinate").AddValue(toString(pB[1]), "km");
        gp->FindKeyword("BodyFixedCoordinate").AddValue(toString(pB[2]), "km");

        gp->FindKeyword("LocalRadius").SetValue(toString(camera->LocalRadius().meters()), "meters");
        gp->FindKeyword("SampleResolution").SetValue(toString(camera->SampleResolution()), "meters/pixel");
        gp->FindKeyword("LineResolution").SetValue(toString(camera->LineResolution()), "meters/pixel");

        camera->instrumentPosition(spB);
        gp->FindKeyword("SpacecraftPosition").AddValue(toString(spB[0]), "km");
        gp->FindKeyword("SpacecraftPosition").AddValue(toString(spB[1]), "km");
        gp->FindKeyword("SpacecraftPosition").AddValue(toString(spB[2]), "km");
        gp->FindKeyword("SpacecraftPosition").AddComment("Spacecraft Information");

        gp->FindKeyword("SpacecraftAzimuth").SetValue(toString(camera->SpacecraftAzimuth()));
        gp->FindKeyword("SlantDistance").SetValue(toString(camera->SlantDistance()), "km");
        gp->FindKeyword("TargetCenterDistance").SetValue(toString(camera->targetCenterDistance()), "km");
        camera->subSpacecraftPoint(ssplat, ssplon);
        gp->FindKeyword("SubSpacecraftLatitude").SetValue(toString(ssplat));
        gp->FindKeyword("SubSpacecraftLongitude").SetValue(toString(ssplon));
        gp->FindKeyword("SpacecraftAltitude").SetValue(toString(camera->SpacecraftAltitude()), "km");
        gp->FindKeyword("OffNadirAngle").SetValue(toString(camera->OffNadirAngle()));
        double subspcgrdaz;
        subspcgrdaz = camera->GroundAzimuth(camera->UniversalLatitude(), camera->UniversalLongitude(),
                                            ssplat, ssplon);
        gp->FindKeyword("SubSpacecraftGroundAzimuth").SetValue(toString(subspcgrdaz));

        camera->sunPosition(sB);
        gp->FindKeyword("SunPosition").AddValue(toString(sB[0]), "km");
        gp->FindKeyword("SunPosition").AddValue(toString(sB[1]), "km");
        gp->FindKeyword("SunPosition").AddValue(toString(sB[2]), "km");
        gp->FindKeyword("SunPosition").AddComment("Sun Information");

        gp->FindKeyword("SubSolarAzimuth").SetValue(toString(camera->SunAzimuth()));
        gp->FindKeyword("SolarDistance").SetValue(toString(camera->SolarDistance()), "AU");
        camera->subSolarPoint(sslat, sslon);
        gp->FindKeyword("SubSolarLatitude").SetValue(toString(sslat));
        gp->FindKeyword("SubSolarLongitude").SetValue(toString(sslon));
        double subsolgrdaz;
        subsolgrdaz = camera->GroundAzimuth(camera->UniversalLatitude(), camera->UniversalLongitude(),
                                            sslat, sslon);
        gp->FindKeyword("SubSolarGroundAzimuth").SetValue(toString(subsolgrdaz));

        gp->FindKeyword("Phase").SetValue(toString(camera->PhaseAngle()));
        gp->FindKeyword("Phase").AddComment("Illumination and Other");
        gp->FindKeyword("Incidence").SetValue(toString(camera->IncidenceAngle()));
        gp->FindKeyword("Emission").SetValue(toString(camera->EmissionAngle()));
        gp->FindKeyword("NorthAzimuth").SetValue(toString(camera->NorthAzimuth()));

        gp->FindKeyword("EphemerisTime").SetValue(toString(camera->time().Et()), "seconds");
        gp->FindKeyword("EphemerisTime").AddComment("Time");
        utc = camera->time().UTC();
        gp->FindKeyword("UTC").SetValue(utc);
        gp->FindKeyword("LocalSolarTime").SetValue(toString(camera->LocalSolarTime()), "hour");
        gp->FindKeyword("SolarLongitude").SetValue(toString(camera->solarLongitude().degrees()));
        if(allowErrors) gp->FindKeyword("Error").SetValue("N/A");
      }
    }
    return gp;
  }
}
