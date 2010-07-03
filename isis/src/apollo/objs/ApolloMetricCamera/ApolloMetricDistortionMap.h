#ifndef ApolloMetricDistortionMap_h
#define ApolloMetricDistortionMap_h

#include "CameraDistortionMap.h"
#include "Camera.h"

namespace Isis {
  class ApolloMetricDistortionMap : public CameraDistortionMap {
    public:
      ApolloMetricDistortionMap (Camera *parent, double xp, double yp, double k1, double k2, double k3, double j1, double j2, double t0);
      ~ApolloMetricDistortionMap () {};

      bool SetFocalPlane (const double dx, const double dy);
      bool SetUndistortedFocalPlane (const double ux, const double uy);

    private:						// parameters below are from camera calibration report
      double p_xp, p_yp;				//!< principal point coordinates
      double p_k1, p_k2, p_k3;			//!< coefficients of radial distortion
      double p_j1, p_j2;				//!< coefficients of decentering distortion
      double p_t0;		   				//!< angle between positive x-axis of image and vector to imaged point
      									//!< used in computation of decentering distortion
  };
};
#endif
