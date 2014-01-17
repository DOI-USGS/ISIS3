#ifndef NewHorizonsLorriDistortionMap_h
#define NewHorizonsLorriDistortionMap_h

#include "CameraDistortionMap.h"
#include "Camera.h"

namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class NewHorizonsLorriDistortionMap : public CameraDistortionMap {
    public:
      NewHorizonsLorriDistortionMap(Camera *parent, double k1, double zDirection = 1.0);
      ~NewHorizonsLorriDistortionMap() {};

      bool SetFocalPlane(const double ux, const double uy);
      bool SetUndistortedFocalPlane(const double dx, const double dy);

    private:
      double p_k1;
  };
};
#endif
