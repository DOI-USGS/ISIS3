/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "LineScanCameraGroundMap.h"
#include "EigenUtilities.h"

#include <cmath>
#include <iostream>
#include <iomanip>
#include <limits>

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
  LineScanCameraGroundMap::LineScanCameraGroundMap(Camera *cam) : CameraGroundMap(cam) {
    bool originalIgnoreProj = cam->isProjectionIgnored();
    try {
      // Sample a 5x5 image grid at two ground heights to fit the
      // projective transform robustly: 5x5 covers wide-FOV corner
      // distortion, two layers break the rank-deficiency that single-
      // layer sampling has on narrow-FOV/narrow-swath sensors (HiRISE).
      const int gridDim = 5;
      const int numPts = gridDim * gridDim;
      double u_factors[numPts];
      double v_factors[numPts];
      for (int gi = 0; gi < gridDim; gi++) {
        for (int gj = 0; gj < gridDim; gj++) {
          u_factors[gi * gridDim + gj] = static_cast<double>(gi) / (gridDim - 1);
          v_factors[gi * gridDim + gj] = static_cast<double>(gj) / (gridDim - 1);
        }
      }

      cam->IgnoreProjection(true);
      cam->SetImage(cam->ParentSamples()/2.0, cam->ParentLines()/2.0);
      SurfacePoint refPt = cam->GetSurfacePoint();

      double numImageRows = cam->ParentLines();
      double numImageCols = cam->ParentSamples();

      std::vector<std::vector<double>> ip(2 * numPts, std::vector<double>(2, 0.0));
      std::vector<std::vector<double>> gp(2 * numPts, std::vector<double>(3, 0.0));
      m_useApproxInitTrans = true;

      for (int i = 0; i < numPts; i++) {
        // Store as Line, Sample
        ip[i][0] = u_factors[i] * numImageRows;
        ip[i][1] = v_factors[i] * numImageCols;
        // Run set image as Sample, Line
        if (!cam->SetImage(ip[i][1], ip[i][0])) {
          m_useApproxInitTrans = false;
        }
        SurfacePoint surfacePt = cam->GetSurfacePoint();
        gp[i][0] = surfacePt.GetX().meters();
        gp[i][1] = surfacePt.GetY().meters();
        gp[i][2] = surfacePt.GetZ().meters();

        // Second layer: a second 3D point on the same camera ray.
        // CSM does this by re-intersecting the ray against an
        // ellipsoid 100 m higher; here we just walk 100 m along the
        // ray toward the spacecraft. Same effect: a second point with
        // identical image coords but different (X,Y,Z), giving the
        // projective fit non-degenerate depth coverage. A radial shift
        // would be wrong off-nadir because radial != ray direction.
        ip[i + numPts][0] = ip[i][0];
        ip[i + numPts][1] = ip[i][1];
        double scPos[3];
        cam->instrumentPosition(scPos);  // body-fixed, in km
        double dx = gp[i][0] - scPos[0] * 1000.0;
        double dy = gp[i][1] - scPos[1] * 1000.0;
        double dz = gp[i][2] - scPos[2] * 1000.0;
        double dnorm = std::sqrt(dx*dx + dy*dy + dz*dz);
        const double delta_along_ray = 100.0;  // meters
        if (dnorm > 0.0) {
          double scale = delta_along_ray / dnorm;
          gp[i + numPts][0] = gp[i][0] - scale * dx;
          gp[i + numPts][1] = gp[i][1] - scale * dy;
          gp[i + numPts][2] = gp[i][2] - scale * dz;
        }
        else {
          m_useApproxInitTrans = false;
        }
      }

      if (m_useApproxInitTrans) {
        computeBestFitProjectiveTransform(ip, gp, m_projTransCoeffs);

        // Sanity check the fit: evaluate the line projective at each
        // training ground point and compare to the known training line.
        // For narrow-FOV/narrow-swath sensors (e.g. HiRISE single CCD)
        // the 14-parameter projective fit can be ill-conditioned and
        // produce a transform whose initial guess sends the secant
        // search to a wrong local zero of the line-offset functor.
        // Detect that here: if the residual at any training point
        // exceeds a few percent of the image height, the fit is not
        // useful and we should fall back to the legacy no-init-guess
        // secant path (which works for these sensors).
        std::vector<double> const& u = m_projTransCoeffs;
        double maxLineResid = 0.0;
        for (int i = 0; i < (int)ip.size(); i++) {
          double x = gp[i][0], y = gp[i][1], z = gp[i][2];
          double den = 1.0 + u[4] * x + u[5] * y + u[6] * z;
          if (den == 0.0 || std::isnan(den) || std::isinf(den)) {
            maxLineResid = std::numeric_limits<double>::infinity();
            break;
          }
          double predLine = (u[0] + u[1] * x + u[2] * y + u[3] * z) / den;
          double resid = std::abs(predLine - ip[i][0]);
          if (!std::isfinite(resid)) {
            maxLineResid = std::numeric_limits<double>::infinity();
            break;
          }
          if (resid > maxLineResid) maxLineResid = resid;
        }
        // Tolerance: 5% of the image height. The projective only needs
        // to put the secant in the right basin; tighter than 5% would
        // start tripping for healthy wide-FOV cameras with curvature
        // that the linear-fractional projective can't represent
        // exactly.
        double residTol = std::max(50.0, 0.05 * cam->ParentLines());
        if (!std::isfinite(maxLineResid) || maxLineResid > residTol) {
          m_useApproxInitTrans = false;
        }
      }
    }
    catch (...) {
      m_useApproxInitTrans = false;
    }
    cam->IgnoreProjection(originalIgnoreProj);
  }


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
  bool LineScanCameraGroundMap::SetGround(const Latitude &lat, const Longitude &lon) {
    Distance radius(p_camera->LocalRadius(lat, lon));

    if (radius.isValid()) {
      return SetGround(SurfacePoint(lat, lon, radius));
    }
    else {
      return false;
    }
  }


  /** Compute undistorted focal plane coordinate from ground position.
   *  See FindFocalPlane for more details.
   *
   * @param surfacePoint 3D point on the surface of the planet
   *
   * @return conversion was successful
   */
  bool LineScanCameraGroundMap::SetGround(const SurfacePoint &surfacePoint) {
    FindFocalPlaneStatus status = FindFocalPlaneStatus::Failure;
    if (m_useApproxInitTrans) {
      std::vector<double> const& u = m_projTransCoeffs; // alias, to save on typing

      double x = surfacePoint.GetX().meters();
      double y = surfacePoint.GetY().meters();
      double z = surfacePoint.GetZ().meters();
      double line_den = 1 + u[4]  * x + u[5]  * y + u[6]  * z;
      double approxLine = 0;
      double numRows = p_camera->ParentLines();

      // Sanity checks. Ensure we don't divide by 0 and that the numbers are valid.
      if (line_den == 0.0 || std::isnan(line_den) || std::isinf(line_den)) {
        approxLine = numRows / 2.0;
      }
      else {
        // Apply the formula
        approxLine = (u[0] + u[1] * x + u[2] * y + u[3]  * z) / line_den;        
      }

      // Since this is valid only over the image,
      // don't let the result go beyond the image border.
      if (approxLine < 0.0) approxLine = 0.0;
      if (approxLine > numRows) approxLine = numRows - 1;
      status = FindFocalPlane(surfacePoint, approxLine);
    }
    else {
      // Projective approximation either failed to fit (e.g. corner
      // pixels off the body) or its training-point residuals were too
      // large to trust (e.g. narrow-FOV/narrow-swath sensors where
      // the 14-parameter linear-fractional fit is ill-conditioned).
      // Use the middle line as a uniformly-OK initial guess: the
      // secant generally converges from there for most pixels in a
      // line-scanner image, even off-nadir, much better than the old
      // quadratic-fit fallback in FindFocalPlane(surfacePoint).
      double approxLine = p_camera->ParentLines() / 2.0;
      status = FindFocalPlane(surfacePoint, approxLine);
    }

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

  /** Helper function to compute undistorted focal plane coordinate 
   *  from ground position. This method tries two different methods.
   *  First a quadratic method searching through all times within the 
   *  image. If this fails then it tries to use Brents 
   *  method (Numerical Recipies 454 - 456) to compute the undistorted
   *  focal plane coordinates.
   *
   * @param surfacePoint 3D point on the surface of the planet
   *
   * @return conversion was successful
   */
  FindFocalPlaneStatus LineScanCameraGroundMap::FindFocalPlane(const SurfacePoint &surfacePoint) {
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

    std::sort(pts.begin(), pts.end(), ptXLessThan);

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
      if (!p_camera->Sensor::SetGround(surfacePoint, true)) {
        return Failure;
      }
    }

    p_camera->Sensor::LookDirection(lookC);
    ux = p_camera->FocalLength() * lookC[0] / lookC[2];
    uy = p_camera->FocalLength() * lookC[1] / lookC[2];

    p_focalPlaneX = ux;
    p_focalPlaneY = uy;

    return Success;
  }

  /** Helper function to compute undistorted focal plane coordinate 
   *  from ground position. This method uses an initial guess with
   *  the secent method https://en.wikipedia.org/wiki/Secant_method
   *  to compute the undistorted focal plane coordinates.
   *
   * @param surfacePoint 3D point on the surface of the planet
   * @param approxLine The approximate like computed from another source
   *
   * @return conversion was successful
   */
  FindFocalPlaneStatus LineScanCameraGroundMap::FindFocalPlane(const SurfacePoint &surfacePoint, const double &approxLine) {
    double approxTime = 0;
    double approxOffset = 0;
    double lookC[3] = {0.0, 0.0, 0.0};
    double ux = 0.0;
    double uy = 0.0;

    double lineRate = ((LineScanCameraDetectorMap *)p_camera->DetectorMap())->LineRate(); //line rate

    if (lineRate == 0.0) return Failure;

    LineOffsetFunctor offsetFunc(p_camera,surfacePoint);
    SensorSurfacePointDistanceFunctor distanceFunc(p_camera,surfacePoint);

    // Use the line given as a start point for the secant method root search.
    p_camera->DetectorMap()->SetParent(p_camera->ParentSamples() / 2.0, approxLine);
    approxTime = p_camera->time().Et();
    approxOffset = offsetFunc(approxTime);

    double f0, f1, x0, x1;

    // starting times for the secant method
    x0 = approxTime;
    x1 = x0 + lineRate;

    // starting offsets
    f0 = approxOffset;  //the first is already calculated
    f1 = offsetFunc(x1);

    // Iterate to refine the given approximate time that the instrument imaged the ground point
    for (int j=0; j < 10; j++) {

      if (fabs(f1) < 1e-6 || ((f1 - f0) == 0.0)) {
        p_camera->Sensor::setTime(x1);
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

      double x2 = x1 - f1 * (x1 - x0) / (f1 - f0);

      double f2 = offsetFunc(x2);

      x0 = x1;
      f0 = f1;
      x1 = x2;
      f1 = f2;
    } // End use a guess
    return Failure;
  }
}


bool ptXLessThan(const QList<double> l1, const QList<double> l2) {
  return l1[0] < l2[0];
}
