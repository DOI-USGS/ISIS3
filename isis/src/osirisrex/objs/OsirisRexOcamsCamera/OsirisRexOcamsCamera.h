#ifndef OsirisRexOcamsCamera_h
#define OsirisRexOcamsCamera_h
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
   * This is the camera model for the Osiris Rex MapCam Camera 
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Osiris Rex
   *  
   * @author  2014-04-02 Janet Barrett
   *
   * @internal
   *   @history 2014-04-02 Janet Barrett - Original version.
   *   @history 2015-09-28 Stuart C. Sides - Updated to work with data delivered 2015-07
   *   @history 2016-04-20 Jeannie Backer - Brought closer to ISIS coding
   *            standards.
   *  
   *  
   *   @todo fix method documentation
   *
   */
  class OsirisRexOcamsCamera : public FramingCamera {

    public:

      OsirisRexOcamsCamera(Cube &cube);
      ~OsirisRexOcamsCamera();

      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time, 
                                                             double exposureDuration);

      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkReferenceId() const;

  };
};
#endif
