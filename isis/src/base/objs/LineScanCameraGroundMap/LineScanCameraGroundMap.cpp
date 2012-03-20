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

#include "LineScanCameraGroundMap.h"

#include <iostream>
#include <iomanip>

#include <QTime>
#include <QList>

#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "Distance.h"
#include "LineScanCameraDetectorMap.h"
#include "iTime.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Statistics.h"
#include "SurfacePoint.h"

using namespace std;

namespace Isis {
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
    } else {
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
    if(status == Success) return true;
    //if(status == Failure) return false;
    return false;
  }
  

  /** Compute undistorted focal plane coordinate from ground position
   *
   * @param lat planetocentric latitude in degrees
   * @param lon planetocentric longitude in degrees
   * @param radius local radius in meters
   *
   * @return conversion was successful
   */
  bool LineScanCameraGroundMap::SetGround(const SurfacePoint &surfacePoint) {
    FindFocalPlaneStatus status = FindFocalPlane(-1, surfacePoint);
    if(status == Success) return true;
    //if(status == Failure) return false;
    return false;

    /*if(status == BoundingProblem) {
      // Get ending bounding framelets and distances for iterative loop to minimize the spacecraft distance
      int startLine = 1;
      double startDist = FindSpacecraftDistance(startLine, surfacePoint);

      int endLine = p_camera->Lines();
      double endDist = FindSpacecraftDistance(endLine, surfacePoint);
      int deltaX = abs(startLine - endLine) / 2;

      // start + deltaX = middle framelet.
      //  We're able to optimize this modified binary search
      //  because the 'V' shape -- it's mostly parallel. Meaning,
      //  if the left side is higher than the right, then the
      //  solution is closer to the right. The bias factor will
      //  determine how much closer.
      double biasFactor = startDist / endDist;

      if(biasFactor < 1.0) {
        biasFactor = -1.0 / biasFactor;
        biasFactor = -(biasFactor + 1) / biasFactor;
      }
      else {
        biasFactor = (biasFactor - 1) / biasFactor;
      }

      int middleLine = startLine + (int)(deltaX + biasFactor * deltaX);
      if(FindFocalPlane(middleLine, surfacePoint) == Success) {
        return true;
      }
      else {
        return false;
      }
    }

    return false;*/
  }

  double LineScanCameraGroundMap::FindSpacecraftDistance(int line,
      const SurfacePoint &surfacePoint) {
    CameraDetectorMap *detectorMap = p_camera->DetectorMap();
    detectorMap->SetParent(p_camera->ParentSamples() / 2, line);
    if(!p_camera->Sensor::SetGround(surfacePoint, false)) return DBL_MAX;

    return p_camera->SlantDistance();
  }



  LineScanCameraGroundMap::FindFocalPlaneStatus
  LineScanCameraGroundMap::FindFocalPlane(const int &approxLine,
                                          const SurfacePoint &surfacePoint) {

    CameraDistortionMap *distortionMap = p_camera->DistortionMap();
    CameraFocalPlaneMap *focalMap = p_camera->FocalPlaneMap();

    double approxTime=0,approxOffset=0;
    double lookC[3] = {0.0, 0.0, 0.0};
    double ux = 0.0, uy = 0.0;
    double dx = 0.0, dy = 0.0;
    double s[3], p[3];
    const double cacheStart = p_camera->Spice::CacheStartTime().Et();
    const double cacheEnd = p_camera->Spice::CacheEndTime().Et();

    double lineRate = ((LineScanCameraDetectorMap *)p_camera->DetectorMap())->LineRate(); //line rate

    if (lineRate == 0.0) return Failure;

    if (approxLine > 0) {  //if an approximate line is given use that as a start point for the Newton root search
      //convert the approxLine to an approximate time and offset
      p_camera->DetectorMap()->SetParent(p_camera->ParentSamples() / 2, approxLine);
      approxTime = p_camera->Time().Et();
      p_camera->Sensor::SetTime(approxTime);
      if(!p_camera->Sensor::SetGround(surfacePoint, false)) {
        return Failure;
      }
      p_camera->Sensor::LookDirection(lookC);
      ux = p_camera->FocalLength() * lookC[0] / lookC[2];
      uy = p_camera->FocalLength() * lookC[1] / lookC[2];

      if(!distortionMap->SetUndistortedFocalPlane(ux, uy)) {
        return Failure;
      }
      dx = distortionMap->FocalPlaneX();
      dy = distortionMap->FocalPlaneY();

      if(!focalMap->SetFocalPlane(dx, dy)) {
        return Failure;
      }
      approxOffset = focalMap->DetectorLineOffset() - focalMap->DetectorLine();
    }
    else {  //no estimate given for the approximate line
      //the offsets are thought to be at most quadratic (at the time of this writing), so let's fit a quadratic to guess the root location

      //the three nodes to be used to approximate the quadratic
      double offsetNodes[3];
      double timeNodes[3],timeAverage,scale;
      QList<double> root;

      timeNodes[0] = cacheStart;
      timeNodes[2] = cacheEnd;
      timeNodes[1] = (cacheStart+cacheEnd)/2.0; //middle time

      double quadPoly[3],temp;

      for (int i=0;i<3;i++) {
        p_camera->Sensor::SetTime(timeNodes[i]);
        if(!p_camera->Sensor::SetGround(surfacePoint, false)) {
          return Failure;
        }
        p_camera->Sensor::LookDirection(lookC);
        ux = p_camera->FocalLength() * lookC[0] / lookC[2];
        uy = p_camera->FocalLength() * lookC[1] / lookC[2];

        if(!distortionMap->SetUndistortedFocalPlane(ux, uy)) {
          return Failure;
        }
        dx = distortionMap->FocalPlaneX();
        dy = distortionMap->FocalPlaneY();

        if(!focalMap->SetFocalPlane(dx, dy)) {
          return Failure;
        }
        offsetNodes[i] = focalMap->DetectorLineOffset() - focalMap->DetectorLine();
      }
   
      //centralize and normalize the data for stability in root finding
      timeAverage = (timeNodes[0] + timeNodes[1] + timeNodes[2])/3.0;
      timeNodes[0] -= timeAverage;
      timeNodes[1] -= timeAverage;
      timeNodes[2] -= timeAverage;

      scale = 1.0/sqrt( (timeNodes[0] - timeNodes[2])*(timeNodes[0] - timeNodes[2]) + (offsetNodes[0] - offsetNodes[2])*(offsetNodes[0] - offsetNodes[2]) );

      timeNodes[0] *= scale;
      timeNodes[1] *= scale;
      timeNodes[2] *= scale;

      offsetNodes[0] *= scale;
      offsetNodes[1] *= scale;
      offsetNodes[2] *= scale;

      //use lagrange interpolating polynomials to find the coefficients of the quadratic, there are many ways to do this; I chose to do it this way because it is pretty straight forward and cheap
      quadPoly[0] = quadPoly[1] = quadPoly[2] = 0.0;

      temp = offsetNodes[0]/((timeNodes[0]-timeNodes[1])*(timeNodes[0]-timeNodes[2]));
      quadPoly[0] += temp;
      quadPoly[1] += temp*(-timeNodes[1]-timeNodes[2]);
      quadPoly[2] += temp*timeNodes[1]*timeNodes[2];

      temp = offsetNodes[1]/((timeNodes[1]-timeNodes[0])*(timeNodes[1]-timeNodes[2]));
      quadPoly[0] += temp;
      quadPoly[1] += temp*(-timeNodes[0]-timeNodes[2]);
      quadPoly[2] += temp*timeNodes[0]*timeNodes[2];
      
      temp = offsetNodes[2]/((timeNodes[2]-timeNodes[0])*(timeNodes[2]-timeNodes[1]));
      quadPoly[0] += temp;
      quadPoly[1] += temp*(-timeNodes[0]-timeNodes[1]);
      quadPoly[2] += temp*timeNodes[0]*timeNodes[1];

      //now that we have the coefficients of the quadratic look for roots (see Numerical Recipes Third Edition page 227)
      temp = quadPoly[1]*quadPoly[1] - 4.0*quadPoly[0]*quadPoly[2];  //discriminant
      if (temp < 0.0) return Failure;  //there are apparently not any real roots on this image

      if(quadPoly[1] >= 0.0) temp = -0.5*(quadPoly[1] + sqrt(temp));
      else                   temp = -0.5*(quadPoly[1] - sqrt(temp));
      if (quadPoly[0] != 0.0) root.push_back(temp/quadPoly[0]);
      if (quadPoly[2] != 0.0) root.push_back(quadPoly[2]/temp);

      //check to see if the roots are in the time interval of the cache
      for (int i=root.size()-1;i>=0;i--) {
        if ( root[i] < timeNodes[0] || root[i] > timeNodes[2] ) 
          root.removeAt(i);
      }
     
      //return the calculated roots to the original system
      for (int i=0;i<root.size();i++) root[i] = root[i]/scale + timeAverage;

      if (root.size()==0) return Failure;  //there are apparently not any roots on this image

      //at the time of this writing ISIS made no attempt to support any sensors that were not "1 to 1"  meaning that imaged the same point on the ground in multiple lines of the image
      //  therefore we must somehow reduce multiple possible roots to a single one,  the legacy code (replaced with this code) did this based on distance from the sensor to the target
      //  the shortest distance being the winner.  For legacy consistency I have used the same logic below.
      int j=0;     
      QList<double> dist;
      QList<double> offset;

      for (int i=0;i<root.size();i++) {  //Offset/dist calculation loop
        p_camera->Sensor::SetTime(root[i]);
        if(!p_camera->Sensor::SetGround(surfacePoint, false)) {
          return Failure;
        }
        p_camera->InstrumentPosition(s);
        p_camera->Coordinate(p);
        dist.push_back( sqrt((s[0] - p[0]) * (s[0] - p[0]) +
                             (s[1] - p[1]) * (s[1] - p[1]) +
                             (s[2] - p[2]) * (s[2] - p[2]) ) );  //distance
        p_camera->Sensor::LookDirection(lookC);
        ux = p_camera->FocalLength() * lookC[0] / lookC[2];
        uy = p_camera->FocalLength() * lookC[1] / lookC[2];

        if(!distortionMap->SetUndistortedFocalPlane(ux, uy)) {
          return Failure;
        }
        dx = distortionMap->FocalPlaneX();
        dy = distortionMap->FocalPlaneY();

        if(!focalMap->SetFocalPlane(dx, dy)) {
          return Failure;
        }
        offset.push_back( focalMap->DetectorLineOffset() - focalMap->DetectorLine() );
      }  //end Offset/dist calculation loop
      //save the root with the smallest dist as the starting point for the iterations
      for (int i=1;i<root.size();i++) {
        if (dist[i] < dist[j]) j=i;
      }      
      
      approxTime = root[j];  //now we have our start time
      approxOffset = offset[j];  //the offsets are saved to avoid recalculating it later
      //printf("DEBUG: approxOffset: %lf  approxTime: %lf\n",approxOffset,approxTime);
    } //end of start time selection

    if (fabs(approxOffset) < 1e-2) { //no need to iteratively improve this root, it's good enough
      p_camera->Sensor::SetTime(approxTime);
      if(!p_camera->Sensor::SetGround(surfacePoint, false)) {
        return Failure;
      }
      p_camera->Sensor::LookDirection(lookC);
      ux = p_camera->FocalLength() * lookC[0] / lookC[2];
      uy = p_camera->FocalLength() * lookC[1] / lookC[2];
     
      p_focalPlaneX = ux;
      p_focalPlaneY = uy;
      return Success;
    }

    //Now that an appoximate time has been identified the secant root fining method will be used to refine it
    //  NOTE: derivatives change so slowing there is nothing to be gained from using other multipoint methods of derivate approximation

    // Get everything ordered for iteration
    double fl, fh, xl, xh;

    //starting times
    xh = approxTime;
    if (xh + lineRate < cacheEnd) xl = xh + lineRate;
    else                          xl = xh - lineRate;

    //starting offsets
    fh = approxOffset;  //the first is already calculated
    p_camera->Sensor::SetTime(xl);
    if(!p_camera->Sensor::SetGround(surfacePoint, false)) {
      return Failure;
    }

    p_camera->Sensor::LookDirection(lookC);
    ux = p_camera->FocalLength() * lookC[0] / lookC[2];
    uy = p_camera->FocalLength() * lookC[1] / lookC[2];

    if(!distortionMap->SetUndistortedFocalPlane(ux, uy)) {
      return Failure;
    }

    dx = distortionMap->FocalPlaneX();
    dy = distortionMap->FocalPlaneY();

    if(!focalMap->SetFocalPlane(dx, dy)) {
      return Failure;
    }

    fl = focalMap->DetectorLineOffset() -
         focalMap->DetectorLine() ;

    // Iterate to refine the time at which the instrument imaged the ground point
    for(int j = 0; j < 10; j++) {
      //printf("DEBUG: %25.20lf %25.20lf %25.20lf %25.20lf\n",xh,fh,xl,fl);
      if ( fl-fh == 0.0) return Failure;
      double etGuess = xl + (xh - xl) * fl / (fl - fh);

      if (etGuess < cacheStart) etGuess = cacheStart;
      if (etGuess > cacheEnd) etGuess = cacheEnd;

      p_camera->Sensor::SetTime(etGuess);
      if(!p_camera->Sensor::SetGround(surfacePoint, false)) {
        return Failure;
      }
      p_camera->Sensor::LookDirection(lookC);
      ux = p_camera->FocalLength() * lookC[0] / lookC[2];
      uy = p_camera->FocalLength() * lookC[1] / lookC[2];

      if(!distortionMap->SetUndistortedFocalPlane(ux, uy)) {
        return Failure;
      }
      dx = distortionMap->FocalPlaneX();
      dy = distortionMap->FocalPlaneY();

      if(!focalMap->SetFocalPlane(dx, dy)) {
        return Failure;
      }
      double f = focalMap->DetectorLineOffset() -
                 focalMap->DetectorLine();

      if( fabs( xl- etGuess) > fabs( xh - etGuess) ) {  //elliminate the node farthest away from the current best guess
        xl = etGuess;
        fl = f;
      }
      else {
        xh = etGuess;
        fh = f;
      }

      // See if we converged on the point so set up the undistorted
      // focal plane values and return
      if(fabs(f) < 1e-2) {
        p_focalPlaneX = ux;
        p_focalPlaneY = uy;
        //printf("iterations: %d  Lines Corrected: %lf\n",j,fabs(etGuess-approxTime)/lineRate);
        sleep(0.2);
        return Success;        
      }
    }
   
    return Failure;
  }
}
