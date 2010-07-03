#ifndef MocWideAngleDistortionMap_h
#define MocWideAngleDistortionMap_h

#include <vector>
#include "CameraDistortionMap.h"

namespace Isis {    
  namespace Mgs {
    /** Distort/undistort focal plane coordinates
     * 
     * Creates a map for adding/removing optical distortions 
     * from the focal plane of the Moc wide angle camera.  
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
};
#endif
