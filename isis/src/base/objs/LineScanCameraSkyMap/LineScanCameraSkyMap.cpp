/**                                                                       
 * @file                                                                  
 * $Revision: 1.1.1.1 $                                                             
 * $Date: 2006/10/31 23:18:08 $                                                                 
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

#include "LineScanCameraSkyMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraDistortionMap.h"
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
    p_camera->Sensor::SetEphemerisTime(p_camera->Spice::CacheStartTime());
    p_camera->Sensor::SetRightAscensionDeclination(ra,dec);

    double lookC[3];
    p_camera->Sensor::LookDirection(lookC);
    double ux = p_camera->FocalLength() * lookC[0] / lookC[2];
    double uy = p_camera->FocalLength() * lookC[1] / lookC[2];

    CameraDistortionMap *distortionMap = p_camera->DistortionMap();
    if (!distortionMap->SetUndistortedFocalPlane(ux,uy)) return false;
    double dx = distortionMap->FocalPlaneX();
    double dy = distortionMap->FocalPlaneY();

    CameraFocalPlaneMap *focalMap = p_camera->FocalPlaneMap();
    if (!focalMap->SetFocalPlane(dx,dy)) return false;
    double startOffset = focalMap->DetectorLineOffset() - 
                         focalMap->DetectorLine();

    // Get ending bounding time and offset for iterative loop
    p_camera->Sensor::SetEphemerisTime(p_camera->Spice::CacheEndTime());
    p_camera->Sensor::SetRightAscensionDeclination(ra,dec);

    p_camera->Sensor::LookDirection(lookC);
    ux = p_camera->FocalLength() * lookC[0] / lookC[2];
    uy = p_camera->FocalLength() * lookC[1] / lookC[2];

    if (!distortionMap->SetUndistortedFocalPlane(ux,uy)) return false;
    dx = distortionMap->FocalPlaneX();
    dy = distortionMap->FocalPlaneY();

    if (!focalMap->SetFocalPlane(dx,dy)) return false;
    double endOffset = focalMap->DetectorLineOffset() - 
                       focalMap->DetectorLine();

    // Make sure we are in the image
    if ((startOffset < 0.0) && (endOffset < 0.0)) return false;
    if ((startOffset > 0.0) && (endOffset > 0.0)) return false;

    // Get everything ordered for iteration
    double fl,fh,xl,xh;
    if (startOffset < endOffset) {
      fl = startOffset;
      fh = endOffset;
      xl = p_camera->Spice::CacheStartTime();
      xh = p_camera->Spice::CacheEndTime();
    }
    else {
      fl = endOffset;
      fh = startOffset;
      xl = p_camera->Spice::CacheEndTime();
      xh = p_camera->Spice::CacheStartTime();
    }

    // Iterate to find the time at which the instrument imaged the ground point
    LineScanCameraDetectorMap *detectorMap = 
      (LineScanCameraDetectorMap *) p_camera->DetectorMap();
    double timeTol = detectorMap->LineRate() / 10.0;
    for (int j=0; j<30; j++) {
      double etGuess = xl + (xh - xl) * fl / (fl - fh);
      p_camera->Sensor::SetEphemerisTime(etGuess);
      p_camera->Sensor::SetRightAscensionDeclination(ra,dec);
      p_camera->Sensor::LookDirection(lookC);
      ux = p_camera->FocalLength() * lookC[0] / lookC[2];
      uy = p_camera->FocalLength() * lookC[1] / lookC[2];

      if (!distortionMap->SetUndistortedFocalPlane(ux,uy)) return false;
      dx = distortionMap->FocalPlaneX();
      dy = distortionMap->FocalPlaneY();

      if (!focalMap->SetFocalPlane(dx,dy)) return false;
      double f = focalMap->DetectorLineOffset() - 
                 focalMap->DetectorLine();

      double delTime;
      if (f < 0.0) {
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
      if (fabs(delTime) < timeTol || f == 0.0) {
        p_focalPlaneX = ux;
        p_focalPlaneY = uy;
        return true;
      }
    }
    return false;
  }
}
