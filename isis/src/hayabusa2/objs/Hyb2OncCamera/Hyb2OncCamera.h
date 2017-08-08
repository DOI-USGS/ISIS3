#ifndef Hyb2OncCamera_h
#define Hyb2OncCamera_h

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

#include "FramingCamera.h"

namespace Isis {
/**
   * This is the camera model for the Hayabusa2 ONC camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Hayabusa2
   *
   * @author  2017-07-07 Kristin Berry (and ... ? )
   *
   * @internal
   *   @history 2017-07-07 Kristin Berry - Original version
   *   
   */
  class Hyb2OncCamera : public FramingCamera {
    public:
      Hyb2OncCamera(Cube &cube);

      ~Hyb2OncCamera();

      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time,
                                                             double exposureDuration);

      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkReferenceId() const;
  };
};
#endif
