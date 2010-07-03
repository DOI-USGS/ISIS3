/**
 * @file  
 * This class describes a non-radial distortion map. The distortion
 * map is a third-order Taylor series expansion of a generic function.
 *
 * Please direct questions to
 * Lillian Nguyen, JHUAPL, (443)778-5477, Lillian.Nguyen@jhuapl.edu
 */
#ifndef TaylorCameraDistortionMap_h
#define TaylorCameraDistortionMap_h

#ifndef CameraDistortionMap_h
#include "CameraDistortionMap.h"
#endif
#include "Camera.h"

namespace Isis {
  /** Distort/undistort focal plane coordinates
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of a camera.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author
   *
   * @internal
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

      void DistortionFunction(double ux, double uy, double* dx, double* dy);
      void DistortionFunctionJacobian(double x, double y, double* Jxx, double* Jxy, double* Jyx, double* Jyy);
  };
};

#endif /*TaylorCameraDistortionMap_h*/
