#ifndef ThemisVisDistortionMap_h
#define ThemisVisDistortionMap_h

#include "CameraDistortionMap.h"

namespace Isis {    
  namespace Odyssey {
    /** 
     * @brief Distort/undistort focal plane coordinates
     *   
     * Creates a map for adding/removing optical distortions 
     * from the focal plane of the Themis VIS camera.  
     * 
     * @ingroup Camera
     * 
     * @author 2006-01-03 Elizabeth Miller
     * 
     * @see Camera
     */
    class ThemisVisDistortionMap : public CameraDistortionMap {
      public:
        ThemisVisDistortionMap(Camera *parent);

        virtual bool SetFocalPlane(const double dx, const double dy);
  
        virtual bool SetUndistortedFocalPlane(const double ux, const double uy);  

      private:
        double p_irPixelPitch;    //!<Pixel Pitch for Themis Ir Camera
        double p_visPixelPitch;   //!<Pixel Pitch for Themis Vis Camera
                                   
        //! Effective Band 5 Detector Value for the Ir Camera                           
        double p_ir_b5;           
    };
  };
};
#endif
