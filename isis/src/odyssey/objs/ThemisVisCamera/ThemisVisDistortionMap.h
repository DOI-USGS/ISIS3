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
   *   @history 2014-04-17 Jeannie Backer - Updated documentation for forward/reverse directions
   *                           using ISIS2 lev1u_m01_thm_routines.c.  Added empty destructor.
   *   @history 2014-04-17 Jeannie Backer - Rewrote the reverse direction map (setFocalPlane) to
   *                           solve for the forward direction and iterate until a solution in
   *                           found. Fixes #1659
   */
  class ThemisVisDistortionMap : public CameraDistortionMap {
    public:
      ThemisVisDistortionMap(Camera *parent);
      ~ThemisVisDistortionMap();

      virtual bool SetFocalPlane(const double dx, const double dy);
      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

    private:
      double p_irPixelPitch;    //!< Pixel Pitch for Themis Ir Camera (in meters per pixel).  
      double p_visPixelPitch;   //!< Pixel Pitch for Themis Vis Camera (in meters per pixel).

      double p_ir_b5_effectiveDetectorLine; /**< Effective 1-based detector line number used for 
                                                 observing the Band 5, i.e., average of the 16
                                                 detector lines used for the band. Detector line
                                                 numbers increase upwards in the image. */
      double p_irBoreLine; //!< The bore line for Themis IR instrument.
  };
};
#endif
