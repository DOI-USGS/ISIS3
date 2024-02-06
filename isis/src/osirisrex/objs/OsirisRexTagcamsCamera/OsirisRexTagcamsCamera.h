#ifndef OsirisRexTagcamsCamera_h
#define OsirisRexTagcamsCamera_h
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

#include <QString>

namespace Isis {
  /**
   * This class models the behavior and attributes of the 
   * OSIRIS-REx Navigation cameras: NavCam, NFTCam, and StowCam 
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Osiris Rex
   *  
   * @author  2018-03-09 Kris Becker
   *
   * @internal
   *   @history 2018-03-09 Kris Becker UA Original Version
   *   @history 2019-01-24 Kris Becker - Removed unused variables to suppress
   *                         warnings
   *
   */
  class OsirisRexTagcamsCamera : public FramingCamera {
    public:
      OsirisRexTagcamsCamera(Cube &cube);
      ~OsirisRexTagcamsCamera();

      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time, 
                                                             double exposureDuration);

      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkReferenceId() const;

  };
};
#endif
