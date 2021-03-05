/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "LineScanCameraSkyMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraDistortionMap.h"
#include "iTime.h"
#include "LineScanCameraDetectorMap.h"

namespace Isis {
  /** Compute undistorted focal plane coordinate from ra/dec
   *
   * @param ra    right ascension in degrees
   * @param dec   declination in degrees
   *
   * @return conversion was successful
   * @todo what happens if we are looking behind the focal plane?????
   * @todo what happens if we are looking parallel to the focal plane??
   * @todo can lookC[2] == zero imply parallel
   * @todo can this all be solved by restricting the physical size of
   * the focal plane?
   */
  bool LineScanCameraSkyMap::SetSky(const double ra, const double dec) {
    // Get beginning bounding time and offset for iterative loop
    p_camera->Sensor::setTime(p_camera->Spice::cacheStartTime());
    p_camera->Sensor::SetRightAscensionDeclination(ra, dec);

    double lookC[3];
    p_camera->Sensor::LookDirection(lookC);
    double ux = p_camera->FocalLength() * lookC[0] / lookC[2];
    double uy = p_camera->FocalLength() * lookC[1] / lookC[2];

    CameraDistortionMap *distortionMap = p_camera->DistortionMap();
    if(!distortionMap->SetUndistortedFocalPlane(ux, uy)) return false;
    double dx = distortionMap->FocalPlaneX();
    double dy = distortionMap->FocalPlaneY();

    CameraFocalPlaneMap *focalMap = p_camera->FocalPlaneMap();
    if(!focalMap->SetFocalPlane(dx, dy)) return false;
    double startOffset = focalMap->DetectorLineOffset() -
                         focalMap->DetectorLine();

    // Get ending bounding time and offset for iterative loop
    p_camera->Sensor::setTime(p_camera->Spice::cacheEndTime());
    p_camera->Sensor::SetRightAscensionDeclination(ra, dec);

    p_camera->Sensor::LookDirection(lookC);
    ux = p_camera->FocalLength() * lookC[0] / lookC[2];
    uy = p_camera->FocalLength() * lookC[1] / lookC[2];

    if(!distortionMap->SetUndistortedFocalPlane(ux, uy)) return false;
    dx = distortionMap->FocalPlaneX();
    dy = distortionMap->FocalPlaneY();

    if(!focalMap->SetFocalPlane(dx, dy)) return false;
    double endOffset = focalMap->DetectorLineOffset() -
                       focalMap->DetectorLine();

    // Make sure we are in the image
    if((startOffset < 0.0) && (endOffset < 0.0)) return false;
    if((startOffset > 0.0) && (endOffset > 0.0)) return false;

    // Get everything ordered for iteration
    double fl, fh, xl, xh;
    if(startOffset < endOffset) {
      fl = startOffset;
      fh = endOffset;
      xl = p_camera->Spice::cacheStartTime().Et();
      xh = p_camera->Spice::cacheEndTime().Et();
    }
    else {
      fl = endOffset;
      fh = startOffset;
      xl = p_camera->Spice::cacheEndTime().Et();
      xh = p_camera->Spice::cacheStartTime().Et();
    }

    // Iterate to find the time at which the instrument imaged the ground point
    LineScanCameraDetectorMap *detectorMap =
      (LineScanCameraDetectorMap *) p_camera->DetectorMap();
    double timeTol = detectorMap->LineRate() / 10.0;
    for(int j = 0; j < 30; j++) {
      double etGuess = xl + (xh - xl) * fl / (fl - fh);
      p_camera->Sensor::setTime(etGuess);
      p_camera->Sensor::SetRightAscensionDeclination(ra, dec);
      p_camera->Sensor::LookDirection(lookC);
      ux = p_camera->FocalLength() * lookC[0] / lookC[2];
      uy = p_camera->FocalLength() * lookC[1] / lookC[2];

      if(!distortionMap->SetUndistortedFocalPlane(ux, uy)) return false;
      dx = distortionMap->FocalPlaneX();
      dy = distortionMap->FocalPlaneY();

      if(!focalMap->SetFocalPlane(dx, dy)) return false;
      double f = focalMap->DetectorLineOffset() -
                 focalMap->DetectorLine();

      double delTime;
      if(f < 0.0) {
        delTime = xl - etGuess;
        xl = etGuess;
        fl = f;
      }
      else {
        delTime = xh - etGuess;
        xh = etGuess;
        fh = f;
      }

      // See if we converged on the point so set up the undistorted
      // focal plane values and return
      if(fabs(delTime) < timeTol || f == 0.0) {
        p_focalPlaneX = ux;
        p_focalPlaneY = uy;
        return true;
      }
    }
    return false;
  }
}
