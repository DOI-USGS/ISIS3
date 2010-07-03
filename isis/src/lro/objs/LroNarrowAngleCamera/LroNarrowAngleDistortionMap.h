#ifndef LroNarrowAngleDistortionMap_h
#define LroNarrowAngleDistortionMap_h

#include <vector>
#include "CameraDistortionMap.h"

namespace Isis {
  namespace Lro {

    /** Distort/undistort focal plane coordinates
     *
     * Creates a map for adding/removing optical distortions
     * from the focal plane of a camera.
     *
     * @ingroup Camera
     *
     * @see Camera
     *
     * @author 2009-07-03 Jacob Danton
     * @history 2010-05-10 Ken Edmundson - Corrected computation of distorted
     *          and undistorted locations
     *
     * @internal
     */
    class LroNarrowAngleDistortionMap : public CameraDistortionMap {
      public:
        LroNarrowAngleDistortionMap(Camera *parent);

        //! Destructor
        virtual ~LroNarrowAngleDistortionMap() {};

        void SetDistortion(const int naifIkCode);

        virtual bool SetFocalPlane(const double dx, const double dy);

        virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

    };
  };
};
#endif
