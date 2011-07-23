#ifndef DawnVirCameraDistortionMap_h
#define DawnVirCameraDistortionMap_h
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

#ifndef CameraDistortionMap_h
#include "CameraDistortionMap.h"
#endif
#include "Camera.h"

namespace Isis {
  /** Distort/undistort focal plane coordinates
   *
   *  Creates a one-to-one distortion map in the focal plane.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Dawn
   *
   * @see DawnVirCamera
   * @see CameraDistortionMap
   *
   * @author Kris Becker
   *
   * @internal
   *   @history 2011-07-07
   */
  class DawnVirCameraDistortionMap : public CameraDistortionMap {
    public:
      DawnVirCameraDistortionMap(Camera *parent, double zDirection = 1.0) :
                                 CameraDistortionMap(parent, zDirection) { }

      //! Destructor
      ~DawnVirCameraDistortionMap() {};

      bool SetFocalPlane(const double dx, const double dy) {
        p_focalPlaneX = dx;
        p_focalPlaneY = dy;

        p_undistortedFocalPlaneX = dx;
        p_undistortedFocalPlaneY = dy;
        return true;
      }

      bool SetUndistortedFocalPlane(const double ux, const double uy) {
        p_undistortedFocalPlaneX = ux;
        p_undistortedFocalPlaneY = uy;
    
        p_focalPlaneX = ux;
        p_focalPlaneY = uy;
        return true;
      }

    protected:
  };
};

#endif /*DawnVirCameraDistortionMap_h*/
