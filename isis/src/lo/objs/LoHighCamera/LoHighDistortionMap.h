#ifndef LoHighDistortionMap_h
#define LoHighDistortionMap_h

/**
 *  @file
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <vector>
#include "CameraDistortionMap.h"

namespace Isis {    
  namespace Lo {
    /** Distort/undistort focal plane coordinates
     * 
     * Creates a map for adding/removing optical distortions 
     * from the focal plane of the Lunar Orbiter high resolution camera.  
     * 
     * @ingroup Camera
     * 
     * @see Camera
     * 
     * @internal
     * 
     * @history 2007-06-29 Debbie A. Cook - Original version
     * @history 2008-02-04 Jeff Anderson - Made change to allow for
     * variable focal length in THEMIS IR 
     * @history 2008-07-25 Steven Lambright - Fixed constructor; CameraDistortionMap 
     *          is responsible both for setting the p_camera protected member and
     *          calling Camera::SetDistortionMap. When the parent called
     *          Camera::SetDistortionMap the Camera took ownership of the instance
     *          of this object. By calling this twice, and with Camera only
     *          supporting having one distortion map, this object was deleted before
     *          the constructor was finished.
     */
    class LoHighDistortionMap : public CameraDistortionMap {
      public:
        LoHighDistortionMap(Camera *parent);
  
        void SetDistortion(const int naifIkCode);
        virtual bool SetFocalPlane(const double dx, const double dy);
  
        virtual bool SetUndistortedFocalPlane(const double ux, const double uy);  

      private:
        double p_xPerspective;                /* Perspective correction factor in x */
        double p_yPerspective;                /* Perspective correction factor in y */
        double p_x0;                          /* Center of distortion on x axis */
        double p_y0;                          /* Center of distortion on y axis */ 
        std::vector<double> p_coefs;
        std::vector<double> p_icoefs;
    };
  };
};
#endif
