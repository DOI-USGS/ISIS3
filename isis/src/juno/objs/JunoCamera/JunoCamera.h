#ifndef JunoCamera_h
#define JunoCamera_h
/**
 * @file
 * $Revision: $
 * $Date: $
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

#include <QString>

namespace Isis {
  /**
   * @brief Juno's JNC (JunoCam) camera model
   *
   * This is the camera model for the JunoCam instrument. This
   * instrument is technically a pushframe instrument, but it is treated as a
   * framing instrument. This is
   * also a more flexible camera model since it will make controlling the
   * individual framelets alot easier.
   * 
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Juno
   * @author 2017-07-22 Jeannie Backer
   *
   * @internal
   *   @history 2017-07-22 Jeannie Backer - Original version. 
   */
  class JunoCamera : public FramingCamera {
    public:
      JunoCamera(Cube &cube);
      ~JunoCamera();

      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time, 
                                                             double exposureDuration);
      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkTargetId() const;
      virtual int SpkReferenceId() const;

  };
};
#endif
