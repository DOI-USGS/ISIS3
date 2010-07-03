#ifndef NirDistortionMap_h
#define NirDistortionMap_h

#include "CameraDistortionMap.h"
#include "Camera.h"

namespace Isis { 
  class RadialDistortionMap : public CameraDistortionMap {
    public:
      RadialDistortionMap (Camera *parent, double k1, double zDirection = 1.0);
      ~RadialDistortionMap () {};     

      bool SetFocalPlane (const double dx, const double dy);
      bool SetUndistortedFocalPlane (const double ux, const double uy);

    private:
      double p_k1;
      //double p_cameraSpec;
  };
};
#endif
