#ifndef DawnFcDistortionMap_h
#define DawnFcDistortionMap_h

#include "CameraDistortionMap.h"
#include "Camera.h"

namespace Isis {
  class DawnFcDistortionMap : public CameraDistortionMap {
    public:
      DawnFcDistortionMap(Camera *parent, double k1, double zDirection = 1.0);
      ~DawnFcDistortionMap() {};

      bool SetFocalPlane(const double ux, const double uy);
      bool SetUndistortedFocalPlane(const double dx, const double dy);

    private:
      double p_k1;
  };
};
#endif
