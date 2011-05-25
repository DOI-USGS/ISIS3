#ifndef TaylorCameraDistortionMap_h
#define TaylorCameraDistortionMap_h
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
   * Creates a map for adding/removing optical distortions
   * from the focal plane of a camera.  This class describes a non-radial 
   * distortion map. The distortion map is a third-order Taylor series expansion 
   * of a generic function. 
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Messenger 
   *
   * @see MdisCamera
   * @see CameraDistortionMap
   *
   * @author Kris Becker
   *
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Added Isis disclaimer.
   *   @history 2011-05-23 Janet Barrett and Steven Lambright - Spice::GetDouble
   *                         is no longer a static call.
   */
  class TaylorCameraDistortionMap : public CameraDistortionMap {
    public:
      TaylorCameraDistortionMap(Camera *parent, double zDirection = 1.0);

      void SetDistortion(const int naifIkCode);

      //! Destructor
      ~TaylorCameraDistortionMap() {};

      bool SetFocalPlane(const double dx, const double dy);

      bool SetUndistortedFocalPlane(const double ux, const double uy);

    protected:
      std::vector<double> p_odtx; //!< distortion x coefficients
      std::vector<double> p_odty; //!< distortion y coefficients

      void DistortionFunction(double ux, double uy, double *dx, double *dy);
      void DistortionFunctionJacobian(double x, double y, double *Jxx, double *Jxy, double *Jyx, double *Jyy);
  };
};
/**
 * Please direct questions to
 * Lillian Nguyen, JHUAPL, (443)778-5477, Lillian.Nguyen@jhuapl.edu
 */
#endif /*TaylorCameraDistortionMap_h*/
