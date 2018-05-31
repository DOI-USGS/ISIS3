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
   * This class models the behavior and attributes of the OSIRIS-REx Cameras:  Mapping Camera,
   * PolyMath Camera, and Sample Camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Osiris Rex
   *
   * @author  2014-04-02 Janet Barrett
   *
   * @internal
   *   @history 2014-04-02 Janet Barrett - Original version.
   *   @history 2015-09-28 Stuart C. Sides - Updated to work with OCams data delivery 2015-07
   *   @history 2016-11-17 Stuart C. Sides - Modified the camera detector origin, in the
   *                           constructor, to reflect the difference between the zero
   *                           based system in the IK and the one based system of
   *                           ISIS. That is, add one to the IK CCD_CENTER values.
   *   @history 2016-12-27 Jeannie Backer - Fixed coding standards. Improved docuementation.
   *                           Fixed CKFrameId() to return the spacecraft ID rather than the MapCam
   *                           ID. Added tests. Moved from branch into trunk. Fixes #4570.
   *   @history 2017-08-25 Jeannie Backer - Updated to handle multiple focus postition IK
   *                           codes for PolyCam frames. We read PolyCamFocusPositionNaifId from
   *                           the Instrument group for focus position specific values (such ase
   *                           focal length) and we read NaifFrameId from the Kernels group the
   *                           instrument frame code. Fixes #5127
   *   @history 2018-03-27 Jesse Mapel - Changed to only replace the IK code with the PolyCam focus
   *                                     setting ID if the image is a PolyCam image. Fixes #5213.
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
