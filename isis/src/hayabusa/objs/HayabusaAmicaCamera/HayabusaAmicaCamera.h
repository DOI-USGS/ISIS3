#ifndef HayabusaAmicaCamera_h
#define HayabusaAmicaCamera_h
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
   * This is the camera model for the Hayabusa AMICA camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Hayabusa
   *
   * @author  2013-11-27 Kris Becker
   *
   * @internal
   *   @history 2013-11-27 Kris Becker - Original version
   *   @history 2015-02-26 Kris Becker - Implement starting detector specs; add
   *                           summing and AlphaCube support.
   *   @history 2015-03-11 Kris Becker - Fixed timing error - was using the
   *                           UTC StartTime rather than the
   *                           SpacecraftClockStartCount. References #2180.
   *   @history 2015-08-11 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these added methods.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   *   @history 2016-09-14 Kelvin Rodriguez - Enforced the order in which BORESIGHT_LINE and
   *                           BORESIGHT_SAMPLE are added to the PVL. Part of porting to
   *                           OSX 10.11 
   *   @history 2017-01-03 Jeannie Backer - Renamed from AmicaCamera to HayabusaAmicaCamera.
   *   @history 2017-01-03 Jeannie Backer - Fixed bug in constructor. When setting the detector
   *                           start line, the camera model was not taking into account the image
   *                           flip on ingestion. Added subframe example to test. Fixes #.
   */
  class HayabusaAmicaCamera : public FramingCamera {
    public:
      HayabusaAmicaCamera(Cube &cube);

      ~HayabusaAmicaCamera();

      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time,
                                                             double exposureDuration);

      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkReferenceId() const;
  };
};
#endif
