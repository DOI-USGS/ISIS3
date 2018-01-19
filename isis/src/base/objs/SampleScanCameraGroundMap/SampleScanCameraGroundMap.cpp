/**
 * @file
 * $Revision: 1.7 $
 * $Date: 2010/06/17 18:59:11 $
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

#include "SampleScanCameraGroundMap.h"

#include <iostream>
#include <iomanip>

#include <QTime>
#include <QList>
#include <QFile>
#include <QTextStream>

#include "IException.h"
#include "IString.h"
#include "Camera.h"
#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "Distance.h"
#include "FunctionTools.h"
#include "iTime.h"
#include "Latitude.h"
#include "Longitude.h"
#include "SampleScanCameraDetectorMap.h"
#include "Statistics.h"
#include "SurfacePoint.h"


using namespace std;
using namespace Isis;

//bool ptXLessThan(const QList<double> l1, const QList<double> l2);

/**
 * @author 2012-05-09 Orrin Thomas
 *
 * @internal
 */
class SampleOffsetFunctor :
  public std::unary_function<double, double > {
  public:

    SampleOffsetFunctor(Isis::Camera *camera, const Isis::SurfacePoint &surPt) {
      m_camera = camera;
      surfacePoint = surPt;
    }


    ~SampleOffsetFunctor() {}


    /** Compute the number of samples between the current sample (i.e., the sample imaged at the et
     *  as set in the camera model) and the sample number where the argument et would hit the focal
     *  plane.
     *  
     * @param et The et at the new postion
     *
     * @return Sample off (see description)
     */
    double operator()(double et) {
      double lookC[3] = {0.0, 0.0, 0.0};
      double ux = 0.0;
      double uy = 0.0;
      double dx = 0.0;
      double dy = 0.0;

      // Verify the time is with the cache bounds
      double startTime = m_camera->cacheStartTime().Et();
      double endTime = m_camera->cacheEndTime().Et();
      if (et < startTime || et > endTime) {
        IString msg = "Ephemeris time passed to SampleOffsetFunctor is not within the image "
                      "cache bounds";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
     
      m_camera->Sensor::setTime(et);
 
      // Set ground
      if (!m_camera->Sensor::SetGround(surfacePoint, false)) {
        IString msg = "Sensor::SetGround failed for surface point in SampleScanCameraGroundMap.cpp"
                      " SampleOffsetFunctor";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
   
      // Calculate the undistorted focal plane coordinates

      m_camera->Sensor::LookDirection(lookC);
      ux = m_camera->FocalLength() * lookC[0] / lookC[2];
      uy = m_camera->FocalLength() * lookC[1] / lookC[2];

      // Try to use SetUndistortedFocalPlane, if that does not work use the distorted x,y
      // under the assumption (bad|good) that extrapolating the distortion
      // is causing the distorted x to be way off the sensor, and thus not very good anyway.
      if (m_camera->DistortionMap()->SetUndistortedFocalPlane(ux, uy)) {
        // Get the natural (distorted focal plane coordinates)
        dx = m_camera->DistortionMap()->FocalPlaneX();
        dy = m_camera->DistortionMap()->FocalPlaneY();
      }
      else {
        dx = ux;
        dy = uy;
      }

      if (!m_camera->FocalPlaneMap()->SetFocalPlane(dx, dy)) {
        IString msg = "FocalPlaneMap::SetFocalPlane failed for surface point in "
                      "SampleScanCameraGroundMap.cpp SampleOffsetFunctor";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }

      // return sample offset in pixels
      return m_camera->FocalPlaneMap()->CenteredDetectorSample();
    }

/*
    double operator()(double et) {
      double lookC[3] = {0.0, 0.0, 0.0};
      double ux = 0.0;
      double uy = 0.0;
      double dx = 0.0;
      double dy = 0.0;

      // Verify the time is with the cache bounds
      double startTime = m_camera->cacheStartTime().Et();
      double endTime = m_camera->cacheEndTime().Et();
      if (et < startTime || et > endTime) {
        IString msg = "Ephemeris time passed to SampleOffsetFunctor is not within the image "
                      "cache bounds";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }

      m_camera->Sensor::setTime(et);

      // Set ground
      if (!m_camera->Sensor::SetGround(surfacePoint, false)) {
        IString msg = "Sensor::SetGround failed for surface point in SampleScanCameraGroundMap.cpp"
                      " SampleOffsetFunctor";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }

      // Calculate the undistorted focal plane coordinates

      m_camera->Sensor::LookDirection(lookC);
      ux = m_camera->FocalLength() * lookC[0] / lookC[2];
      uy = m_camera->FocalLength() * lookC[1] / lookC[2];

      // Try to use SetUndistortedFocalPlane, if that does not work use the distorted x,y
      // under the assumption (bad|good) that extrapolating the distortion
      // is causing the distorted x to be way off the sensor, and thus not very good anyway.
      if (m_camera->DistortionMap()->SetUndistortedFocalPlane(ux, uy)) {
        // Get the natural (distorted focal plane coordinates)
        dx = m_camera->DistortionMap()->FocalPlaneX();
        dy = m_camera->DistortionMap()->FocalPlaneY();
      }
      else {
        dx = ux;
        dy = uy;
      }

      if (!m_camera->FocalPlaneMap()->SetFocalPlane(dx, dy)) {
        IString msg = "FocalPlaneMap::SetFocalPlane failed for surface point in "
                      "SampleScanCameraGroundMap.cpp SampleOffsetFunctor";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }

      // Return the offset
//      return (m_camera->FocalPlaneMap()->DetectorSampleOffset() -
//              m_camera->FocalPlaneMap()->DetectorSample());
      return (m_camera->FocalPlaneMap()->DetectorSampleOffset() -
              m_camera->FocalPlaneMap()->DetectorSample());
    }
*/
  private:
    SurfacePoint surfacePoint;
    Camera* m_camera;
};


/**
 * @author 2012-05-09 Orrin Thomas
 *
 * @internal
 */
class SensorSurfacePointDistanceFunctor : 
  public std::unary_function<double, double > {

  public:
    SensorSurfacePointDistanceFunctor(Isis::Camera *camera, const Isis::SurfacePoint &surPt) {
      m_camera = camera;
      surfacePoint = surPt;
    }


    ~SensorSurfacePointDistanceFunctor() {}


    double operator()(double et) {
      double s[3], p[3];

      //verify the time is with the cache bounds
      double startTime = m_camera->cacheStartTime().Et();
      double endTime = m_camera->cacheEndTime().Et();

      if (et < startTime || et > endTime) {
        IString msg = "Ephemeris time passed to SensorSurfacePointDistanceFunctor is not within the image "
                      "cache bounds";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
      m_camera->Sensor::setTime(et);
      if(!m_camera->Sensor::SetGround(surfacePoint, false)) {
         IString msg = "Sensor::SetGround failed for surface point in SampleScanCameraGroundMap.cpp"
                       "SensorSurfacePointDistanceFunctor";
      }
      m_camera->instrumentPosition(s);
      m_camera->Coordinate(p);
      return sqrt((s[0] - p[0]) * (s[0] - p[0]) +
                  (s[1] - p[1]) * (s[1] - p[1]) +
                  (s[2] - p[2]) * (s[2] - p[2]) );  //distance
    }

  private:
    SurfacePoint surfacePoint;
    Camera* m_camera;
};


namespace Isis {

  /** Constructor
   *
   * @param cam pointer to camera model
   */
  SampleScanCameraGroundMap::SampleScanCameraGroundMap(Camera *cam) : CameraGroundMap(cam) {}


  /** Destructor
   *
   */
  SampleScanCameraGroundMap::~SampleScanCameraGroundMap() {}

  /** Compute undistorted focal plane coordinate from ground position
   *
   * @param lat planetocentric latitude in degrees
   * @param lon planetocentric longitude in degrees
   *
   * @return conversion was successful
   */
  bool SampleScanCameraGroundMap::SetGround(const Latitude &lat,
      const Longitude &lon) {
    Distance radius(p_camera->LocalRadius(lat, lon));

    if (radius.isValid()) {
      return SetGround(SurfacePoint(lat, lon, radius));
    }
    else {
      return false;
    }
  }


  /** Compute undistorted focal plane coordinate from ground position
   *
   * @param lat planetocentric latitude in degrees
   * @param lon planetocentric longitude in degrees
   * @param radius local radius in meters
   *
   * @return conversion was successful
   */
  bool SampleScanCameraGroundMap::SetGround(const SurfacePoint &surfacePoint,
                                            const int &approxSample) {
    FindFocalPlaneStatus status = FindFocalPlane(approxSample, surfacePoint);
    if (status == Success)
      return true;

    return false;
  }
  

  /** Compute undistorted focal plane coordinate from ground position
   *
   * @param surfacePoint 3D point on the surface of the planet
   *
   * @return conversion was successful
   */
  bool SampleScanCameraGroundMap::SetGround(const SurfacePoint &surfacePoint) {
    FindFocalPlaneStatus status = FindFocalPlane(-1, surfacePoint);

    if (status == Success)
      return true;

    return false;
  }


  double SampleScanCameraGroundMap::FindSpacecraftDistance(int sample,
      const SurfacePoint &surfacePoint) {

    CameraDetectorMap *detectorMap = p_camera->DetectorMap();
    detectorMap->SetParent(sample, p_camera->ParentLines() / 2);
    if (!p_camera->Sensor::SetGround(surfacePoint, false)) {
      return DBL_MAX;
    }

    return p_camera->SlantDistance();
  }


  SampleScanCameraGroundMap::FindFocalPlaneStatus
      SampleScanCameraGroundMap::FindFocalPlane(const int &approxLine,
                                              const SurfacePoint &surfacePoint) {
    double approxTime=0;
    double approxOffset=0;
    double lookC[3] = {0.0, 0.0, 0.0};
    double ux = 0.0;
    double uy = 0.0;
    const double cacheStart = p_camera->Spice::cacheStartTime().Et();
    const double cacheEnd = p_camera->Spice::cacheEndTime().Et();

    double sampleRate
        = ((SampleScanCameraDetectorMap *)p_camera->DetectorMap())->SampleRate();

    if (sampleRate == 0.0)
      return Failure;

    SampleOffsetFunctor offsetFunc(p_camera,surfacePoint);

    // parent center sample, line used as first approximation
    p_camera->DetectorMap()->SetParent(p_camera->ParentSamples()/2, p_camera->ParentLines()/2);
    approxTime = p_camera->time().Et();

    approxOffset = offsetFunc(approxTime);

    if (fabs(approxOffset) < 1e-2) { //no need to iteratively improve this root, it's good enough
      p_camera->Sensor::setTime(approxTime);

      // ensure point isn't behind the planet
      if (!p_camera->Sensor::SetGround(surfacePoint, true)) {
        return Failure;
      }
      p_camera->Sensor::LookDirection(lookC);
      ux = p_camera->FocalLength() * lookC[0] / lookC[2];
      uy = p_camera->FocalLength() * lookC[1] / lookC[2];

      p_focalPlaneX = ux;
      p_focalPlaneY = uy;

      return Success;
    }

    double fl, fh, xl, xh;

    //starting times for the secant method, kept within the domain of the cache
    xh = approxTime;
    if (xh + sampleRate < cacheEnd) {
      xl = xh + sampleRate;
    }
    else {
      xl = xh - sampleRate;
    }

    //starting offsets
    fh = approxOffset;  //the first is already calculated
    fl = offsetFunc(xl);

    // Iterate to refine the given approximate time that the instrument imaged the ground point
    for (int j=0; j < 10; j++) {
      if (fl-fh == 0.0) {
        return Failure;
      }
      double etGuess = xl + (xh - xl) * fl / (fl - fh);

      if (etGuess < cacheStart) etGuess = cacheStart;
      if (etGuess > cacheEnd) etGuess = cacheEnd;

      double f = offsetFunc(etGuess);

      // eliminate the node farthest away from the current best guess
      if (fabs( xl- etGuess) > fabs( xh - etGuess)) {
        xl = etGuess;
        fl = f;
      }
      else {
        xh = etGuess;
        fh = f;
      }

      // if converged on point set focal plane values and return
      if (fabs(f) < 1e-2) {
        p_camera->Sensor::setTime(etGuess);

        // ensure point isn't behind the planet
        if (!p_camera->Sensor::SetGround(surfacePoint, true)) {
          return Failure;
        }
        p_camera->Sensor::LookDirection(lookC);
        ux = p_camera->FocalLength() * lookC[0] / lookC[2];
        uy = p_camera->FocalLength() * lookC[1] / lookC[2];

        if (!p_camera->FocalPlaneMap()->SetFocalPlane(ux, uy)) {
          IString msg = "FocalPlaneMap::SetFocalPlane failed for surface point in "
                        "SampleScanCameraGroundMap.cpp SampleOffsetFunctor";
          throw IException(IException::Programmer, msg, _FILEINFO_);
        }

        p_focalPlaneX = ux;
        p_focalPlaneY = uy;

        return Success;
      }
    }
    return Failure;
  }


/*
  SampleScanCameraGroundMap::FindFocalPlaneStatus
      SampleScanCameraGroundMap::FindFocalPlane(const int &approxSample,
                                                const SurfacePoint &surfacePoint) {

    // check for invalid sample rate (not sure if necessary, Orrin had done this)
    if (((SampleScanCameraDetectorMap *)p_camera->DetectorMap())->SampleRate() == 0.0) {
      return Failure;
    }

    SampleOffsetFunctor offsetFunc(p_camera,surfacePoint);

    double approxTime=0;
    double lookC[3] = {0.0, 0.0, 0.0};
    double ux = 0.0;
    double uy = 0.0;

    // mid-sample and mid-line are first approximation
    double midSample = p_camera->ParentSamples()/2.0;
    double midLine = p_camera->ParentLines()/2.0;
    p_camera->DetectorMap()->SetParent(midSample, midLine);
    approxTime = p_camera->time().Et();

    double approxOffset = offsetFunc(approxTime);

    bool converged = false;
    do {
      p_camera->Sensor::setTime(approxTime);

      // ensure surfacePoint isn't behind the planet
      if (!p_camera->Sensor::SetGround(surfacePoint, false)) {
        return Failure;
      }

      p_camera->Sensor::LookDirection(lookC);
      ux = p_camera->FocalLength() * lookC[0] / lookC[2];
      uy = p_camera->FocalLength() * lookC[1] / lookC[2];

      if (fabs(uy) < 1.0e-4) {
        // we have converged
        converged = true;
        break;
      }

      // if not converged, add uy/pixelpitch to starting sample and try again
      midSample += uy/0.005;
      double parentSample = p_camera->DetectorMap()->ParentSample();
      double parentLine = p_camera->DetectorMap()->ParentLine();
      p_camera->DetectorMap()->SetParent(midSample, midLine);
      approxTime = p_camera->time().Et();
    }
    while (converged == false);

    if (converged) {
      p_camera->Sensor::setTime(approxTime);

      //check to make sure the point isn't behind the planet
      if (!p_camera->Sensor::SetGround(surfacePoint, true)) {
        return Failure;
      }
      p_camera->Sensor::LookDirection(lookC);
      ux = p_camera->FocalLength() * lookC[0] / lookC[2];
      uy = p_camera->FocalLength() * lookC[1] / lookC[2];

      if (!p_camera->FocalPlaneMap()->SetFocalPlane(ux, uy)) {
        IString msg = "FocalPlaneMap::SetFocalPlane failed for surface point in "
                      "SampleScanCameraGroundMap.cpp SampleOffsetFunctor";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }

      p_focalPlaneX = ux;
      p_focalPlaneY = uy;

      return Success;
    }

    return Failure;
  }
*/

  /**
   * Compute ground position from focal plane coordinate
   *
   * This method will compute the ground position given an
   * undistorted focal plane coordinate.  Note that the latitude/longitude
   * value can be obtained from the camera pointer passed into the constructor.
   *
   * @param ux undistorted focal plane x in millimeters
   * @param uy undistorted focal plane y in millimeters
   * @param uz undistorted focal plane z in millimeters
   *
   * @return @b bool If conversion was successful
   *
   * TODO: what is implication of this????????
   */
  bool SampleScanCameraGroundMap::SetFocalPlane(const double ux, const double uy, const double uz) {
    return CameraGroundMap::SetFocalPlane(ux, uy, uz);
  }

}


//bool ptXLessThan(const QList<double> l1, const QList<double> l2) {
//  return l1[0] < l2[0];
//}

