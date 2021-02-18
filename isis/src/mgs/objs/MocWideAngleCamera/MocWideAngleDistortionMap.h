#ifndef MocWideAngleDistortionMap_h
#define MocWideAngleDistortionMap_h
/**
 * @file
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

#include <vector>
#include "CameraDistortionMap.h"

namespace Isis {
  /** 
   *  Distort/undistort focal plane coordinates
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of the Moc wide angle camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup MarsGlobalSurveyor
   *
   * @see Camera
   *  
   * @author 2005-02-01 Jeff Anderson 
   * @internal
   *   @history 2005-02-01 Jeff Anderson - Original version
   *   @history 2011-05-03 Jeannie Walldren - Removed Mgs namespace wrap.
   *
   */
  class MocWideAngleDistortionMap : public CameraDistortionMap {
    public:
      MocWideAngleDistortionMap(Camera *parent, bool red);

      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

    private:
      std::vector<double> p_coefs;
      std::vector<double> p_icoefs;
      double p_scale;
      int p_numCoefs;
  };
};
#endif
