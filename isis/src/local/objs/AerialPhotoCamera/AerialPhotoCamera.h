#ifndef AerialPhotoCamera_h
#define AerialPhotoCamera_h
/**
 * @file
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

#include "FramingCamera.h"

namespace Isis {
  /**
   * @brief Aerial Camera Model
   *
   * This is the camera model for generic aerial photos.
   *
   * @ingroup AerialPhoto
   *
   * @author 2014-03-25 Jeff Anderson
   *
   * @internal
   *   @history 2014-03-25 Jeff Anderson - Original Version
   */
  class AerialPhotoCamera : public FramingCamera {
    public:
      AerialPhotoCamera(Cube &cube);

      ~AerialPhotoCamera() {};

      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time, 
                                                             double exposureDuration);

      /**
       * CK frame ID -
       * Naif assigns frame ids.  Since aerial photos aren't tied to NASA 
       * missions we don't really have a frame id.   We will us numbers that the 
       * NAIF team allows as documented in their NAIF ID required reading 
       * manual. This means that we can really can't create NAIF kernels using 
       * spkwriter or ckwriter.  
       *  
       * @return @b int Return generic frame id 
       */
      virtual int CkFrameId() const { return -2000001; }

      /**
       * CK Reference ID -
       *  
       * @return @b int The appropriate instrument code for the "Camera-matrix" Kernel 
       *         Reference ID
       */
      virtual int CkReferenceId() const { return -2000000; }

      /**
       * SPK Target Body ID -
       *  
       * @return @b int The appropriate instrument code for the Spacecraft 
       *         Kernel Target ID
       */
      virtual int SpkTargetId() const { return -2000; }

      /** 
       *  SPK Reference ID - J2000
       *  
       * @return @b int The appropriate instrument code for the Spacecraft 
       *         Kernel Reference ID,
       */
      virtual int SpkReferenceId() const { return (1); }

  };
};
#endif
