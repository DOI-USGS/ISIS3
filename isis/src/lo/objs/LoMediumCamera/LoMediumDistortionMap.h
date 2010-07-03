#ifndef LoMediumDistortionMap_h
#define LoMediumDistortionMap_h

#include <vector>
#include "CameraDistortionMap.h"

namespace Isis {    
  namespace Lo {
    /** Distort/undistort focal plane coordinates
     * 
     * Creates a map for adding/removing optical distortions 
     * from the focal plane of the Lunar Orbiter medium resolution camera.  
     * 
     * @ingroup Camera
     * 
     * @see Camera
     * 
     * @internal
     * 
     * @history 2007-07-31 Debbie A. Cook - Original version
     * @history 2008-02-04 Jeff Anderson - Made change to support variable
     * focal length in THEMIS IR camera 
     * @history 2008-07-25 Steven Lambright - Fixed constructor; CameraDistortionMap 
     *          is responsible both for setting the p_camera protected member and
     *          calling Camera::SetDistortionMap. When the parent called
     *          Camera::SetDistortionMap the Camera took ownership of the instance
     *          of this object. By calling this twice, and with Camera only
     *          supporting having one distortion map, this object was deleted before
     *          the constructor was finished.
     * @history 2009-05-22 Debbie A. Cook - Cleaned up code and added iteration loop. Previous
     *          version only iterated twice, but results indicated more iterations were
     *          needed for better accuracy.
     * @history 2009-08-21 Debbie A. Cook - Added test for data outside focal plane limits
     *          plus 10% to avoid getting erroneous data projected on oblique images
     * @history 2010-01-25 Debbie A. Cook - Increased out-of-bounds test to 17.5% of fiducial max
     *          to make sure lat/lons were defined to the image edges
     * 
     */
    class LoMediumDistortionMap : public CameraDistortionMap {
      public:
        LoMediumDistortionMap(Camera *parent);
  
        void SetDistortion(const int naifIkCode);
        virtual bool SetFocalPlane(const double dx, const double dy);
  
        virtual bool SetUndistortedFocalPlane(const double ux, const double uy);  

      private:
        double p_sample0;                  /* Center of distortion on sample axis */
        double p_line0;                       /* Center of distortion on line axis */ 
        std::vector<double> p_coefs;
        std::vector<double> p_icoefs;
    };
  };
};

#endif
