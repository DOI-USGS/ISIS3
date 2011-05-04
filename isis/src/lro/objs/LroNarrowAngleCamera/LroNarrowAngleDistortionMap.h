#ifndef LroNarrowAngleDistortionMap_h
#define LroNarrowAngleDistortionMap_h
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

#include <vector>
#include "CameraDistortionMap.h"

namespace Isis {

  /** 
   *  Distort/undistort focal plane coordinates
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of a camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup LunarReconnaissanceOrbiter
   *
   * @see LroNarrowAngleCamera
   *
   * @author 2009-07-03 Jacob Danton
   * @internal
   *   @history 2010-05-10 Ken Edmundson - Corrected computation of distorted
   *            and undistorted locations
   *   @history 2010-08-21 Kris Becker - Changed the sign of the distortion 
   *            parameter to match the calibration report.  The LRO/LROC IK
   *            lro_lroc_v14.ti and above contain the appropriate parameters
   *            to coincide with the code change made here.  IMPORTANT:  This
   *            results in Version = 2 of the LroNarrowAngleCamera as depicted
   *            in the Camera.plugin for both NAC-L and NAC-R.
   *   @history 2011-05-03 Jeannie Walldren - Removed Lro namespace wrap.
   */
  class LroNarrowAngleDistortionMap : public CameraDistortionMap {
    public:
      LroNarrowAngleDistortionMap(Camera *parent);

      //! Destroys the LroNarrowAngleDistortionMap object.
      virtual ~LroNarrowAngleDistortionMap() {};

      void SetDistortion(const int naifIkCode);

      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

  };
};
#endif
