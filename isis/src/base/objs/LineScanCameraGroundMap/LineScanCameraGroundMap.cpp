/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "LineScanCameraGroundMap.h"

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
#include "LineScanCameraDetectorMap.h"
#include "iTime.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Statistics.h"
#include "SurfacePoint.h"
#include "FunctionTools.h"


using namespace std;
using namespace Isis;

bool ptXLessThan(const QList<double> l1, const QList<double> l2);

/**
 * @author 2012-05-09 Orrin Thomas
 *
 * @internal
 */
class LineOffsetFunctor :
  public std::function<double(double)> {
  public:

    LineOffsetFunctor(Isis::Camera *camera, const Isis::SurfacePoint &surPt) {
      m_camera = camera;
      m_surfacePoint = surPt;
    }


    ~LineOffsetFunctor() {}


    /** Compute the number of lines between the current line (i.e., the line imaged at the et as set
     *  in the camera model) and the line number where the argument et would hit the focal
     *  plane.
     *
     * @param et The et at the new postion
     *
     * @return Line off (see description)
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
        IString msg = "Ephemeris time passed to LineOffsetFunctor is not within the image "
                      "cache bounds";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }

      m_camera->Sensor::setTime(et);

      // Set ground
      if (!m_camera->Sensor::SetGround(m_surfacePoint, false)) {
        IString msg = "Sensor::SetGround failed for surface point in LineScanCameraGroundMap.cpp"
                      " LineOffsetFunctor";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }

      // Calculate the undistorted focal plane coordinates
      m_camera->Sensor::LookDirection(lookC);
      ux = m_camera->FocalLength() * lookC[0] / lookC[2];
      uy = m_camera->FocalLength() * lookC[1] / lookC[2];


      // This was replaced with the code below to get Chandrayaan M3 to work.
      // SetUndistortedFocalPlane was failing a majority of the time, causing most SetGround calls
      // to fail. Even when it did succeed, it was producing non-continous return values.
        // Set the undistorted focal plane coordinates
      //  if (!m_camera->DistortionMap()->SetUndistortedFocalPlane(ux, uy)) {
      //    IString msg = "DistortionMap::SetUndistoredFocalPlane failed for surface point in "
      //                  "LineScanCameraGroundMap.cpp LineOffsetFunctor";
      //    throw IException(IException::Programmer, msg, _FILEINFO_);
      //  }

        // Get the natural (distorted focal plane coordinates)
      //  dx = m_camera->DistortionMap()->FocalPlaneX();
      //  dy = m_camera->DistortionMap()->FocalPlaneY();
      //  std::cout << "use dist" << std::endl;
      //}


      // Try to use SetUndistortedFocalPlane, if that does not work use the distorted x,y
      // under the assumption (bad|good) that extrapolating the distortion
      // is causing the distorted x,y to be way off the sensor, and thus not very good anyway.
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
                      "LineScanCameraGroundMap.cpp LineOffsetFunctor";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }

      // Return the offset
      return (m_camera->FocalPlaneMap()->DetectorLineOffset() -
              m_camera->FocalPlaneMap()->DetectorLine());
    }


  private:
    SurfacePoint m_surfacePoint;
    Camera* m_camera;
};


/**
 * @author 2012-05-09 Orrin Thomas
 *
 * @internal
 */
class SensorSurfacePointDistanceFunctor :
  public std::function<double(double)> {

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
      if (!m_camera->Sensor::SetGround(surfacePoint, false)) {
         IString msg = "Sensor::SetGround failed for surface point in LineScanCameraGroundMap.cpp"
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
  LineScanCameraGroundMap::LineScanCameraGroundMap(Camera *cam) : CameraGroundMap(cam) {}


  /** Destructor
   *
   */
  LineScanCameraGroundMap::~LineScanCameraGroundMap() {}

  /** Compute undistorted focal plane coordinate from ground position
   *
   * @param lat planetocentric latitude in degrees
   * @param lon planetocentric longitude in degrees
   *
   * @return conversion was successful
   */
  bool LineScanCameraGroundMap::SetGround(const Latitude &lat,
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
  bool LineScanCameraGroundMap::SetGround(const SurfacePoint &surfacePoint, const int &approxLine) {
    FindFocalPlaneStatus status = FindFocalPlane(approxLine, surfacePoint);
    if (status == Success) return true;
    //if(status == Failure) return false;
    return false;
  }


  /** Compute undistorted focal plane coordinate from ground position
   *
   * @param surfacePoint 3D point on the surface of the planet
   *
   * @return conversion was successful
   */
  bool LineScanCameraGroundMap::SetGround(const SurfacePoint &surfacePoint) {
    FindFocalPlaneStatus status = FindFocalPlane(-1, surfacePoint);

    if (status == Success) return true;

    return false;
  }


  double LineScanCameraGroundMap::FindSpacecraftDistance(int line,
      const SurfacePoint &surfacePoint) {

    CameraDetectorMap *detectorMap = p_camera->DetectorMap();
    detectorMap->SetParent(p_camera->ParentSamples() / 2, line);
    if (!p_camera->Sensor::SetGround(surfacePoint, false)) {
      return DBL_MAX;
    }

    return p_camera->SlantDistance();
  }


  LineScanCameraGroundMap::FindFocalPlaneStatus
      LineScanCameraGroundMap::FindFocalPlane(const int &approxLine,
                                              const SurfacePoint &surfacePoint) {

    //CameraDistortionMap *distortionMap = p_camera->DistortionMap();
    //CameraFocalPlaneMap *focalMap = p_camera->FocalPlaneMap();

    double approxTime = 0;
    double approxOffset = 0;
    double lookC[3] = {0.0, 0.0, 0.0};
    double ux = 0.0;
    double uy = 0.0;
    //double dx = 0.0, dy = 0.0;
    //double s[3], p[3];
    const double cacheStart = p_camera->Spice::cacheStartTime().Et();
    const double cacheEnd = p_camera->Spice::cacheEndTime().Et();

    double lineRate = ((LineScanCameraDetectorMap *)p_camera->DetectorMap())->LineRate(); //line rate

    if (lineRate == 0.0) return Failure;

    LineOffsetFunctor offsetFunc(p_camera,surfacePoint);
    SensorSurfacePointDistanceFunctor distanceFunc(p_camera,surfacePoint);

    // METHOD #1
    // Use the line given as a start point for the secant method root search.
    if (approxLine >= 0.5) {

      // convert the approxLine to an approximate time and offset
      p_camera->DetectorMap()->SetParent(p_camera->ParentSamples() / 2.0, approxLine);
      approxTime = p_camera->time().Et();

      approxOffset = offsetFunc(approxTime);

      // Check to see if there is no need to improve this root, it's good enough
      if (fabs(approxOffset) < 1e-2) {
        p_camera->Sensor::setTime(approxTime);
        // check to make sure the point isn't behind the planet
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

      // starting times for the secant method, kept within the domain of the cache
      xh = approxTime;
      if (xh + lineRate < cacheEnd) {
        xl = xh + lineRate;
      }
      else {
        xl = xh - lineRate;
      }

      // starting offsets
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


        // elliminate the node farthest away from the current best guess
        if (fabs( xl- etGuess) > fabs( xh - etGuess)) {
          xl = etGuess;
          fl = f;
        }
        else {
          xh = etGuess;
          fh = f;
        }

        // See if we converged on the point so set up the undistorted focal plane values and return
        if (fabs(f) < 1e-2) {
          p_camera->Sensor::setTime(approxTime);
          // check to make sure the point isn't behind the planet
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
      } // End itteration using a guess
      // return Failure; // Removed to let the lagrange method try to find the line if secant fails
    } // End use a guess


    // METHOD #2
    // The guess or middle line did not work so try estimating with a quadratic
    // The offsets are typically quadratic, so three points will be used to approximate a quadratic
    // as a first order attempt to find the root location(s)

    // The three nodes to be used to approximate the quadratic
    double offsetNodes[3];
    double timeNodes[3];
    double timeAverage;
    double scale;
    QList<double> root;
    QList<double> offset;
    QList<double> dist;

    timeNodes[0] = cacheStart;
    timeNodes[2] = cacheEnd;
    timeNodes[1] = (cacheStart+cacheEnd) / 2.0; // middle time

    double quadPoly[3];
    double temp;

    for (int i=0; i<3; i++) {
      offsetNodes[i] = offsetFunc(timeNodes[i]);
    }

    // centralize and normalize the data for stability in root finding
    timeAverage = (timeNodes[0] + timeNodes[1] + timeNodes[2]) / 3.0;
    timeNodes[0] -= timeAverage;
    timeNodes[1] -= timeAverage;
    timeNodes[2] -= timeAverage;

    scale = 1.0 / sqrt((timeNodes[0] - timeNodes[2]) *
                       (timeNodes[0] - timeNodes[2]) +
                       (offsetNodes[0] - offsetNodes[2]) *
                       (offsetNodes[0] - offsetNodes[2]));

    timeNodes[0] *= scale;
    timeNodes[1] *= scale;
    timeNodes[2] *= scale;

    offsetNodes[0] *= scale;
    offsetNodes[1] *= scale;
    offsetNodes[2] *= scale;

    // Use lagrange interpolating polynomials to find the coefficients of the quadratic,
    // there are many ways to do this; I chose to do it this way because it is pretty straight
    // forward and cheap
    quadPoly[0] = quadPoly[1] = quadPoly[2] = 0.0;

    temp = offsetNodes[0] / ((timeNodes[0] - timeNodes[1]) * (timeNodes[0] - timeNodes[2]));
    quadPoly[0] += temp;
    quadPoly[1] += temp * (-timeNodes[1] - timeNodes[2]);
    quadPoly[2] += temp * timeNodes[1] * timeNodes[2];

    temp = offsetNodes[1] / ((timeNodes[1] - timeNodes[0]) * (timeNodes[1] - timeNodes[2]));
    quadPoly[0] += temp;
    quadPoly[1] += temp * (-timeNodes[0] - timeNodes[2]);
    quadPoly[2] += temp * timeNodes[0] * timeNodes[2];

    temp = offsetNodes[2] / ((timeNodes[2] - timeNodes[0]) * (timeNodes[2] - timeNodes[1]));
    quadPoly[0] += temp;
    quadPoly[1] += temp * (-timeNodes[0] - timeNodes[1]);
    quadPoly[2] += temp * timeNodes[0] * timeNodes[1];

    // Now that we have the coefficients of the quadratic look for roots
    // (see Numerical Recipes Third Edition page 227)
    temp = quadPoly[1] * quadPoly[1] - 4.0 * quadPoly[0] * quadPoly[2];  //discriminant

    // THIS IS A PREMATURE FAILURE RETURN. IT SHOULD TRY THE NEXT METHON BEFORE FAILING
    if (temp < 0.0) {
      return Failure;  // there are apparently not any real roots on this image
    }

    if (quadPoly[1] >= 0.0) {
      temp = -0.5 * (quadPoly[1] + sqrt(temp));
    }
    else {
      temp = -0.5 * (quadPoly[1] - sqrt(temp));
    }

    if (quadPoly[0] != 0.0) {
      root.push_back(temp/quadPoly[0]);
    }

    if (quadPoly[2] != 0.0) {
      root.push_back(quadPoly[2]/temp);
    }

    // check to see if the roots are in the time interval of the cache
    for (int i=root.size()-1; i>=0; i--) {
      if ( root[i] < timeNodes[0] || root[i] > timeNodes[2] ) {
        root.removeAt(i);
      }
    }

    // return the calculated roots to the original system
    for (int i=0; i<root.size(); i++) {
      root[i] = root[i]/scale + timeAverage;
    }

    // THIS IS A PREMATURE FAILURE RETURN. IT SHOULD TRY THE NEXT METHON BEFORE FAILING
    if (root.size() == 0) {
      return Failure;  // there are apparently not any roots on this image
    }

    // At the time of this writing ISIS made no attempt to support any sensors that were not "1 to 1".
    // Meaning they imaged the same point on the ground in multiple lines of the image
    // therefore we must somehow reduce multiple possible roots to a single one,  the legacy
    // code (replaced with this code) did this based on distance from the sensor to the target
    // the shortest distance being the winner.  For legacy consistency I have used the same logic below.

    for (int i=0; i<root.size(); i++) {  // Offset/dist calculation loop
      dist << distanceFunc(root[i]);
      offset << offsetFunc(root[i]);
    }

    // Save the root with the smallest dist
    {
      int j=0;
      for (int i=1; i<root.size(); i++) {
        if (dist[i] < dist[j]) j=i;
      }

      approxTime = root[j];  // Now we have our time
      approxOffset = offset[j];  // The offsets are saved to avoid recalculating it later
    }

    if (fabs(approxOffset) < 1.0e-2) { // No need to iteratively improve this root, it's good enough
      p_camera->Sensor::setTime(approxTime);

      // Check to make sure the point isn't behind the planet
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


    // METHOD #3
    // Estimated line and quadratic approximation insufficient, try Brent's method
    // The offsets are typically quadratic, so three points will be used to approximate a quadratic
    // as a first order attempt to find the root location(s)

    // The above sections are sufficient for finding the correct times for the vast majority of
    // back projection solutions.  The following section exists for those few particularly
    // stuborn problems.  They are typically characterised by being significantly non-quadratic.
    // Examples mostly include images with very long exposure times.
    //
    // Further, while the preceeding sections is intended to be fast, this section is intended to be
    // thurough.  Brents method (Numerical Recipies 454 - 456) will be used to find all the roots,
    // that are bracketed by the five points defined in the quadratic solution method above.
    // The root with the shortest distance to the camera will be returned

    // Get everything ordered for iteration combine and sort the five already defined points
    QList <QList <double> > pts;

    for (int i=0; i<3; i++) {
      QList <double> pt;
      pt << timeNodes[i] / scale + timeAverage;
      pt << offsetNodes[i] / scale;
      pts << pt;
    }

    for (int i=0; i<root.size(); i++) {
      QList <double> pt;
      pt << root[i];
      pt << offset[i];
      pts << pt;
    }

    sort(pts.begin(), pts.end(), ptXLessThan);

    root.clear();
    for (int i=1; i<pts.size(); i++) {
      // If the signs of the two offsets are not the same they bracket at least one root
      if ( (pts[i-1][1] > 0) - (pts[i-1][1] < 0) != (pts[i][1] > 0) - (pts[i][1] < 0) ) {
        double temp;
        if (FunctionTools::brentsRootFinder <LineOffsetFunctor> (offsetFunc, pts[i-1], pts[i],
                                                                 1.0e-3, 200, temp)) {
          root << temp;
        }
      }
    }

    // Discard any roots that are looking through the planet
    for (int i = root.size()-1; i>=0; i--) {
      p_camera->Sensor::setTime(root[i]);
      //check to make sure the point isn't behind the planet
      if (!p_camera->Sensor::SetGround(surfacePoint, true)) {
        root.removeAt(i);
      }
    }

    // If none of the roots remain...
    if (root.size() == 0) {
      return Failure;
    }

    // Choose from the remaining roots, the solution with the smallest distance to target
    dist.clear();
    offset.clear();
    for (int i=0; i<root.size(); i++) {  // Offset/dist calculation loop
      dist << distanceFunc(root[i]);
      offset << offsetFunc(root[i]);
    }

    // Save the root with the smallest dist
    {
      int j=0;
      for (int i=1; i<root.size(); i++) {
        if (dist[i] < dist[j]) j=i;
      }

      p_camera->Sensor::setTime(root[j]);
    }

    // No need to make sure the point isn't behind the planet, it was done above
    p_camera->Sensor::LookDirection(lookC);
    ux = p_camera->FocalLength() * lookC[0] / lookC[2];
    uy = p_camera->FocalLength() * lookC[1] / lookC[2];

    p_focalPlaneX = ux;
    p_focalPlaneY = uy;

    return Success;
  }
}


bool ptXLessThan(const QList<double> l1, const QList<double> l2) {
  return l1[0] < l2[0];
}
