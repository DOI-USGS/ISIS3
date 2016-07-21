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

#include <QDebug>

#include <iomanip>

#include "Brick.h"
#include "Camera.h"
#include "CameraFocalPlaneMap.h"
#include "Cube.h"
#include "CubeManager.h"
#include "Distance.h"
#include "IException.h"
#include "iTime.h"
#include "Longitude.h"
#include "PvlGroup.h"
#include "SpecialPixel.h"
#include "TProjection.h"

using namespace Isis;
using namespace std;

namespace Isis {


  /**
   * Constructor, initializes CubeManager and other variables for use.
   *
   */
  CameraPointInfo::CameraPointInfo() {
    m_usedCubes = NULL;
    m_usedCubes = new CubeManager();
    m_usedCubes->SetNumOpenCubes(50);
    m_currentCube = NULL;
    m_camera = NULL;
  }


  /**
   * Destructor, deletes CubeManager object used.
   *
   */
  CameraPointInfo::~CameraPointInfo() {
    if (m_usedCubes) {
      delete m_usedCubes;
      m_usedCubes = NULL;
    }
  }


  /**
   * SetCube opens the given cube in a CubeManager. 
   * The CubeManager is for effeciency when working with control 
   * nets where cubes are accesed multiple times. 
   *
   * @param cubeFileName A cube file name.
   */
  void CameraPointInfo::SetCube(const QString &cubeFileName) {
    m_currentCube = m_usedCubes->OpenCube(cubeFileName);
    m_camera = m_currentCube->camera();
  }


  /**
   * SetImage sets a sample, line image coordinate in the camera
   * so data can be accessed.
   *
   * @param sample A sample coordinate in or almost in the cube.
   * @param line A line coordinate in or almost in the cube.
   * @param allowOutside Indicates whether to allow extrapolation.
   * @param allowErrors  Indicates whether to allow the program to 
   *                     throw an error if a problem occurs.
   *
   * @return @b PvlGroup* The pertinent data from the Camera class on the point. 
   *                      Ownership is passed to caller.
   */
  PvlGroup *CameraPointInfo::SetImage(const double sample, const double line,
                                      const bool allowOutside, const bool allowErrors) {
    if (CheckCube()) {
      bool passed = m_camera->SetImage(sample, line);
      return GetPointInfo(passed, allowOutside, allowErrors);
    }
    // Should never get here, error will be thrown in CheckCube()
    return NULL;
  }


  /**
   * SetCenter sets the image coordinates to the center of the image. 
   *  
   * @param allowOutside Indicates whether to allow extrapolation.
   * @param allowErrors  Indicates whether to allow the program to 
   *                     throw an error if a problem occurs.
   *
   * @return @b PvlGroup* The pertinent data from the Camera class on the point. 
   *                      Ownership is passed to caller.
   */
  PvlGroup *CameraPointInfo::SetCenter(const bool allowOutside, const bool allowErrors) {
    if (CheckCube()) {
      bool passed = m_camera->SetImage(m_currentCube->sampleCount() / 2.0, 
                                       m_currentCube->lineCount() / 2.0);
      return GetPointInfo(passed, allowOutside, allowErrors);
    }
    // Should never get here, error will be thrown in CheckCube()
    return NULL;
  }


  /**
   * SetSample sets the image coordinates to the center line and the
   * given sample.
   *
   * @param sample A sample coordinate in or almost in the cube.
   * @param allowOutside Indicates whether to allow extrapolation.
   * @param allowErrors  Indicates whether to allow the program to 
   *                     throw an error if a problem occurs.
   *  
   * @return @b PvlGroup* The pertinent data from the Camera class on the point. 
   *                      Ownership is passed to caller.
   */
  PvlGroup *CameraPointInfo::SetSample(const double sample,
                                       const bool allowOutside, 
                                       const bool allowErrors) {
    if (CheckCube()) {
      bool passed = m_camera->SetImage(sample, m_currentCube->lineCount() / 2.0);
      return GetPointInfo(passed, allowOutside, allowErrors);
    }
    // Should never get here, error will be thrown in CheckCube()
    return NULL;
  }


  /**
   * SetLine sets the image coordinates to the center sample and the
   * given line.
   *
   * @param line A line coordinate in or almost in the cube.
   * @param allowOutside Indicates whether to allow extrapolation.
   * @param allowErrors  Indicates whether to allow the program to 
   *                     throw an error if a problem occurs.
   *  
   * @return @b PvlGroup* The pertinent data from the Camera class on the point. 
   *                      Ownership is passed to caller.
   */
  PvlGroup *CameraPointInfo::SetLine(const double line,
                                     const bool allowOutside,
                                     const bool allowErrors) {
    if (CheckCube()) {
      bool passed = m_camera->SetImage(m_currentCube->sampleCount() / 2.0, line);
      return GetPointInfo(passed, allowOutside, allowErrors);
    }
    // Should never get here, error will be thrown in CheckCube()
    return NULL;
  }


  /**
   * SetGround sets a latitude, longitude grrund coordinate in the
   * camera so data can be accessed.
   *
   * @param latitude A latitude coordinate in or almost in the cube
   * @param longitude A longitude coordinate in or almost in the cube
   * @param allowOutside Indicates whether to allow extrapolation.
   * @param allowErrors  Indicates whether to allow the program to 
   *                     throw an error if a problem occurs.
   *
   * @return @b PvlGroup* The pertinent data from the Camera class on the point. 
   *                      Ownership is passed to caller.
   */
  PvlGroup *CameraPointInfo::SetGround(const double latitude, const double longitude,
                                       const bool allowOutside, const bool allowErrors) {
    if (CheckCube()) {
      bool passed = m_camera->SetUniversalGround(latitude, longitude);
      return GetPointInfo(passed, allowOutside, allowErrors);
    }
    // Should never get here, error will be thrown in CheckCube()
    return NULL;
  }


  /**
   * CheckCube checks that a cube has been set before the data for
   * a point is accessed.
   *
   * @return @b bool Indicates whether a cube has been set.
   */
  bool CameraPointInfo::CheckCube() {
    if (m_currentCube == NULL) {
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
   * @param passed Indicates whether the call to SetImage() was successful.
   * @param allowOutside Indicates whether to allow extrapolation.
   * @param allowErrors  Indicates whether to allow the program to 
   *                     throw an error if a problem occurs.
   *
   * @return @b PvlGroup* Data taken directly from the Camera and 
   *                      derived from Camera information.
   *                      Ownership is passed to caller.
   */
  PvlGroup *CameraPointInfo::GetPointInfo(bool passed, bool allowOutside, bool allowErrors) {
    PvlGroup *gp = new PvlGroup("GroundPoint");
    {
      gp->addKeyword(PvlKeyword("Filename"));
      gp->addKeyword(PvlKeyword("Sample"));
      gp->addKeyword(PvlKeyword("Line"));
      gp->addKeyword(PvlKeyword("PixelValue"));
      gp->addKeyword(PvlKeyword("RightAscension"));
      gp->addKeyword(PvlKeyword("Declination"));
      gp->addKeyword(PvlKeyword("PlanetocentricLatitude"));
      gp->addKeyword(PvlKeyword("PlanetographicLatitude"));
      gp->addKeyword(PvlKeyword("PositiveEast360Longitude"));
      gp->addKeyword(PvlKeyword("PositiveEast180Longitude"));
      gp->addKeyword(PvlKeyword("PositiveWest360Longitude"));
      gp->addKeyword(PvlKeyword("PositiveWest180Longitude"));
      gp->addKeyword(PvlKeyword("BodyFixedCoordinate"));
      gp->addKeyword(PvlKeyword("LocalRadius"));
      gp->addKeyword(PvlKeyword("SampleResolution"));
      gp->addKeyword(PvlKeyword("LineResolution"));
      gp->addKeyword(PvlKeyword("SpacecraftPosition"));
      gp->addKeyword(PvlKeyword("SpacecraftAzimuth"));
      gp->addKeyword(PvlKeyword("SlantDistance"));
      gp->addKeyword(PvlKeyword("TargetCenterDistance"));
      gp->addKeyword(PvlKeyword("SubSpacecraftLatitude"));
      gp->addKeyword(PvlKeyword("SubSpacecraftLongitude"));
      gp->addKeyword(PvlKeyword("SpacecraftAltitude"));
      gp->addKeyword(PvlKeyword("OffNadirAngle"));
      gp->addKeyword(PvlKeyword("SubSpacecraftGroundAzimuth"));
      gp->addKeyword(PvlKeyword("SunPosition"));
      gp->addKeyword(PvlKeyword("SubSolarAzimuth"));
      gp->addKeyword(PvlKeyword("SolarDistance"));
      gp->addKeyword(PvlKeyword("SubSolarLatitude"));
      gp->addKeyword(PvlKeyword("SubSolarLongitude"));
      gp->addKeyword(PvlKeyword("SubSolarGroundAzimuth"));
      gp->addKeyword(PvlKeyword("Phase"));
      gp->addKeyword(PvlKeyword("Incidence"));
      gp->addKeyword(PvlKeyword("Emission"));
      gp->addKeyword(PvlKeyword("NorthAzimuth"));
      gp->addKeyword(PvlKeyword("EphemerisTime"));
      gp->addKeyword(PvlKeyword("UTC"));
      gp->addKeyword(PvlKeyword("LocalSolarTime"));
      gp->addKeyword(PvlKeyword("SolarLongitude"));
      gp->addKeyword(PvlKeyword("LookDirectionBodyFixed"));
      gp->addKeyword(PvlKeyword("LookDirectionJ2000"));
      gp->addKeyword(PvlKeyword("LookDirectionCamera"));
      if (allowErrors) gp->addKeyword(PvlKeyword("Error"));
    }

    bool noErrors = passed;
    QString error = "";
    if (!m_camera->HasSurfaceIntersection()) {
      error = "Requested position does not project in camera model; no surface intersection";
      noErrors = false;
      if (!allowErrors) throw IException(IException::Unknown, error, _FILEINFO_);
    }
    if (!m_camera->InCube() && !allowOutside) {
      error = "Requested position does not project in camera model; not inside cube";
      noErrors = false;
      if (!allowErrors) throw IException(IException::Unknown, error, _FILEINFO_);
    }

    if (!noErrors) {
      for (int i = 0; i < gp->keywords(); i++) {
        QString name = (*gp)[i].name();
        // These three keywords have 3 values, so they must have 3 NULLs
        if (name == "BodyFixedCoordinate" || name == "SpacecraftPosition" ||
            name == "SunPosition") {
          (*gp)[i].addValue("NULL");
          (*gp)[i].addValue("NULL");
          (*gp)[i].addValue("NULL");
        }
        else {
          (*gp)[i].setValue("NULL");
        }
      }
      // Set all keywords that still have valid information
      gp->findKeyword("Error").setValue(error);
      gp->findKeyword("FileName").setValue(m_currentCube->fileName());
      gp->findKeyword("Sample").setValue(toString(m_camera->Sample()));
      gp->findKeyword("Line").setValue(toString(m_camera->Line()));
      gp->findKeyword("EphemerisTime").setValue(
                      toString(m_camera->time().Et()), "seconds");
      gp->findKeyword("EphemerisTime").addComment("Time");
      QString utc = m_camera->time().UTC();
      gp->findKeyword("UTC").setValue(utc);
      gp->findKeyword("SpacecraftPosition").addComment("Spacecraft Information");
      gp->findKeyword("SunPosition").addComment("Sun Information");
      gp->findKeyword("Phase").addComment("Illumination and Other");
    }
    else {

      Brick b(3, 3, 1, m_currentCube->pixelType());

      int intSamp = (int)(m_camera->Sample() + 0.5);
      int intLine = (int)(m_camera->Line() + 0.5);
      b.SetBasePosition(intSamp, intLine, 1);
      m_currentCube->read(b);

      double pB[3], spB[3], sB[3];
      QString utc;
      double ssplat, ssplon, sslat, sslon, ocentricLat, ographicLat, pe360Lon, pw360Lon;
    
      {
        gp->findKeyword("FileName").setValue(m_currentCube->fileName());
        gp->findKeyword("Sample").setValue(toString(m_camera->Sample()));
        gp->findKeyword("Line").setValue(toString(m_camera->Line()));
        gp->findKeyword("PixelValue").setValue(PixelToString(b[0]));
        gp->findKeyword("RightAscension").setValue(toString(
                        m_camera->RightAscension()), "degrees");
        gp->findKeyword("Declination").setValue(toString(
                        m_camera->Declination()), "degrees");
        ocentricLat = m_camera->UniversalLatitude();
        gp->findKeyword("PlanetocentricLatitude").setValue(toString(ocentricLat), "degrees");

        // Convert lat to planetographic
        Distance radii[3];
        m_camera->radii(radii);
        ographicLat = TProjection::ToPlanetographic(ocentricLat, 
                                              radii[0].kilometers(), 
                                              radii[2].kilometers());
        gp->findKeyword("PlanetographicLatitude").setValue(toString(ographicLat), "degrees");
       
        pe360Lon = m_camera->UniversalLongitude();
        gp->findKeyword("PositiveEast360Longitude").setValue(toString(pe360Lon), "degrees");
       
        //Convert lon to -180 - 180 range
        gp->findKeyword("PositiveEast180Longitude").setValue(toString(
                                              TProjection::To180Domain(pe360Lon)), "degrees");

        //Convert lon to positive west
        pw360Lon = TProjection::ToPositiveWest(pe360Lon, 360);
        gp->findKeyword("PositiveWest360Longitude").setValue(toString(pw360Lon), "degrees");

        //Convert pwlon to -180 - 180 range
        gp->findKeyword("PositiveWest180Longitude").setValue(
                                   toString( TProjection::To180Domain(pw360Lon)), "degrees");
        
        m_camera->Coordinate(pB);
        gp->findKeyword("BodyFixedCoordinate").addValue(toString(pB[0]), "km");
        gp->findKeyword("BodyFixedCoordinate").addValue(toString(pB[1]), "km");
        gp->findKeyword("BodyFixedCoordinate").addValue(toString(pB[2]), "km");

        gp->findKeyword("LocalRadius").setValue(toString(
                        m_camera->LocalRadius().meters()), "meters");

        gp->findKeyword("SampleResolution").setValue(toString(
                        m_camera->SampleResolution()), "meters/pixel");
        gp->findKeyword("LineResolution").setValue(toString(
                        m_camera->LineResolution()), "meters/pixel");

        //body fixed
        m_camera->instrumentPosition(spB);
        gp->findKeyword("SpacecraftPosition").addValue(toString(spB[0]), "km");
        gp->findKeyword("SpacecraftPosition").addValue(toString(spB[1]), "km");
        gp->findKeyword("SpacecraftPosition").addValue(toString(spB[2]), "km");
        gp->findKeyword("SpacecraftPosition").addComment("Spacecraft Information");
        
        double spacecraftAzi = m_camera->SpacecraftAzimuth();
        if (Isis::IsValidPixel(spacecraftAzi)) {
          gp->findKeyword("SpacecraftAzimuth").setValue(toString(spacecraftAzi), "degrees");
        }
        else {
          gp->findKeyword("SpacecraftAzimuth").setValue("NULL");
        }

        gp->findKeyword("SlantDistance").setValue(toString(
                        m_camera->SlantDistance()), "km");
        gp->findKeyword("TargetCenterDistance").setValue(toString(
                        m_camera->targetCenterDistance()), "km");
        m_camera->subSpacecraftPoint(ssplat, ssplon);
        gp->findKeyword("SubSpacecraftLatitude").setValue(toString(ssplat), "degrees");
        gp->findKeyword("SubSpacecraftLongitude").setValue(toString(ssplon), "degrees");
        gp->findKeyword("SpacecraftAltitude").setValue(toString(
                        m_camera->SpacecraftAltitude()), "km");
        gp->findKeyword("OffNadirAngle").setValue(toString(
                        m_camera->OffNadirAngle()), "degrees");
        double subspcgrdaz = m_camera->GroundAzimuth(m_camera->UniversalLatitude(), 
                                              m_camera->UniversalLongitude(),
                                              ssplat, ssplon);
        gp->findKeyword("SubSpacecraftGroundAzimuth").setValue(
                                                 toString(subspcgrdaz), "degrees");

        m_camera->sunPosition(sB);
        gp->findKeyword("SunPosition").addValue(toString(sB[0]), "km");
        gp->findKeyword("SunPosition").addValue(toString(sB[1]), "km");
        gp->findKeyword("SunPosition").addValue(toString(sB[2]), "km");
        gp->findKeyword("SunPosition").addComment("Sun Information");
        
        double sunAzi = m_camera->SunAzimuth();
        if (Isis::IsValidPixel(sunAzi)) {
          gp->findKeyword("SubSolarAzimuth").setValue(toString(sunAzi), "degrees");
        }
        else {
          gp->findKeyword("SubSolarAzimuth").setValue("NULL");
        }

        gp->findKeyword("SolarDistance").setValue(toString(
                        m_camera->SolarDistance()), "AU");
        m_camera->subSolarPoint(sslat, sslon);
        gp->findKeyword("SubSolarLatitude").setValue(toString(sslat), "degrees");
        gp->findKeyword("SubSolarLongitude").setValue(toString(sslon), "degrees");
        double subsolgrdaz = m_camera->GroundAzimuth(m_camera->UniversalLatitude(), 
                                              m_camera->UniversalLongitude(),
                                              sslat, sslon);
        gp->findKeyword("SubSolarGroundAzimuth").setValue(
                                                 toString(subsolgrdaz), "degrees");

        gp->findKeyword("Phase").setValue(toString(m_camera->PhaseAngle()), "degrees");
        gp->findKeyword("Phase").addComment("Illumination and Other");
        gp->findKeyword("Incidence").setValue(toString(
                        m_camera->IncidenceAngle()), "degrees");
        gp->findKeyword("Emission").setValue(toString(
                        m_camera->EmissionAngle()), "degrees");
        
        double northAzi = m_camera->NorthAzimuth();
        if (Isis::IsValidPixel(northAzi)) {
          gp->findKeyword("NorthAzimuth").setValue(toString(northAzi), "degrees");
        }
        else {
          gp->findKeyword("NorthAzimuth").setValue("NULL");
        }

        gp->findKeyword("EphemerisTime").setValue(toString(
                        m_camera->time().Et()), "seconds");
        gp->findKeyword("EphemerisTime").addComment("Time");
        utc = m_camera->time().UTC();
        gp->findKeyword("UTC").setValue(utc);
        gp->findKeyword("LocalSolarTime").setValue(toString(
                        m_camera->LocalSolarTime()), "hour");
        gp->findKeyword("SolarLongitude").setValue(toString(
                        m_camera->solarLongitude().degrees()), "degrees");

        std::vector<double>lookB = m_camera->lookDirectionBodyFixed();
        gp->findKeyword("LookDirectionBodyFixed").addValue(toString(lookB[0]), "degrees");
        gp->findKeyword("LookDirectionBodyFixed").addValue(toString(lookB[1]), "degrees");
        gp->findKeyword("LookDirectionBodyFixed").addValue(toString(lookB[2]), "degrees");
        gp->findKeyword("LookDirectionBodyFixed").addComment("Look Direction Unit Vectors in Body Fixed, J2000, and Camera Coordinate Systems.");

        std::vector<double>lookJ = m_camera->lookDirectionJ2000();
        gp->findKeyword("LookDirectionJ2000").addValue(toString(lookJ[0]), "degrees");
        gp->findKeyword("LookDirectionJ2000").addValue(toString(lookJ[1]), "degrees");
        gp->findKeyword("LookDirectionJ2000").addValue(toString(lookJ[2]), "degrees");

        double lookC[3];
        m_camera->LookDirection(lookC);
        gp->findKeyword("LookDirectionCamera").addValue(toString(lookC[0]), "degrees");
        gp->findKeyword("LookDirectionCamera").addValue(toString(lookC[1]), "degrees");
        gp->findKeyword("LookDirectionCamera").addValue(toString(lookC[2]), "degrees");



        if (allowErrors) gp->findKeyword("Error").setValue("NULL");
      }
    }
    return gp;
  }


  /** 
   * Retrieves a pointer to the camera.
   * 
   * @return @b Camera* A pointer to the Camera. 
   */
  Camera *CameraPointInfo::camera() {
    return m_camera;
  }


  /** 
   * Retrieves a pointer to the current cube.
   * 
   * @return @b Cube* A pointer to the current cube. 
   */
  Cube *CameraPointInfo::cube() {
    return m_currentCube;
  }
}
