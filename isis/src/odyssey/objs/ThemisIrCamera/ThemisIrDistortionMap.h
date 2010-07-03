#ifndef ThemisIrDistortionMap_h
#define ThemisIrDistortionMap_h

#include "CameraDistortionMap.h"

namespace Isis {
  namespace Odyssey {
    /** Distort/undistort focal plane coordinates
     *
     * Creates a map for adding/removing optical distortions
     * from the focal plane of the Themis IR camera.
     *
     * @ingroup Camera
     *
     * @see Camera
     *
     * @internal
     *
     * @history 2005-02-01 Jeff Anderson
     * Original version
     *
     * @history 2009-03-27 Jeff Anderson
     * Modified to use Duxbury's distortion model from Feb 2009
     * email with attached PDF document
     *
     */
    class ThemisIrDistortionMap : public CameraDistortionMap {
      public:
        ThemisIrDistortionMap(Camera *parent);

        virtual bool SetFocalPlane(const double dx, const double dy);

        virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

        void SetBand (int band);

      private:
        double p_k;
        double p_alpha1;
        double p_alpha2;
    };
  };
};
#endif
