#ifndef ThemisVisDistortionMap_h
#define ThemisVisDistortionMap_h
/**
 * @file
 *
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
#include "CameraDistortionMap.h"

namespace Isis {
  /**
   * @brief Distort/undistort focal plane coordinates
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of the Themis VIS camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup MarsOdyssey
   *
   * @see ThemisVisCamera 
   *  
   * @author 2006-01-03 Elizabeth Miller
   *
   * @internal 
   *   @history 2011-05-03 Jeannie Walldren - Fixed documentation.  Removed
   *                          Odyssey namespace wrap inside Isis wrap.
   */
  class ThemisVisDistortionMap : public CameraDistortionMap {
    public:
      ThemisVisDistortionMap(Camera *parent);

      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

    private:
      double p_irPixelPitch;    //!< Pixel Pitch for Themis Ir Camera
      double p_visPixelPitch;   //!< Pixel Pitch for Themis Vis Camera

      double p_ir_b5; //!< Effective Band 5 Detector Value for the Ir Camera
  };
};
#endif
