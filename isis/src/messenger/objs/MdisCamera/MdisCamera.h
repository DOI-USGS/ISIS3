#ifndef MdisCamera_h
#define MdisCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "FramingCamera.h"

#include <QString>

namespace Isis {
  /**
   * @brief MESSENGER MDIS NAC and WAC Camera Model
   *
   * This is the camera model for both MESSENGER MDIS Wide Angle (WAC) and
   * Narrow Angle (NAC) cameras.
   *
   * This camera model is desinfed to be externally managed as much as
   * possible through the Messenger MDIS instrument kernel (IAK).  See the
   * file $ISISDATA/messenger/kernels/iak/mdisAddendum???.ti for details.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Messenger
   * @see http://pds-imaging.jpl.nasa.gov/documentation/MDISEDRSIS.PDF
   * @see http://pds-imaging.jpl.nasa.gov/documentation/MDIS_CDR_RDRSIS.PDF
   *
   * @author 2005-07-29 Kris Becker
   *
   * @internal
   *   @history 2006-10-31 Jeff Anderson - Updated to accomodate Spice class refactory.
   *   @history 2007-04-24 Kris Becker - Corrected problems with setting Ephemeris
   *                           time after the cache is created;  fixed problem with FPU binning
   *                           geometry is mapped to detector map as described in the SIS.  This
   *                           problem is due to an FPGA coding bug on the camera.
   *   @history 2007-09-05 Kris Becker - Removed test for jailbar imaging mode
   *                           as the MDIS team reports it should be treated as a normal
   *                           image.
   *   @history 2007-09-06 Kris Becker - Removed test for subframe imaging mode
   *                           as the team provided calification on its implications.
   *   @history 2007-12-06 Kris Becker - Added camera distortion model provided
   *                           by Scott Turner, JHU/APL.
   *   @history 2008-08-08 Steven Lambright Now using the new LoadCache(...)
   *                           method instead of CreateCache(...).
   *   @history 2009-01-21 Kris Becker Added a new implementation of the MDIS
   *                           NAC distortion model contributed by Scott Turner and Lillian
   *                           Nguyen at JHU/APL.
   *   @history 2009-06-11 Steven Lambright - Documentation fixes
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                           inherit directly from Camera
   *   @history 2010-10-06 Kris Becker - Updated WAC distortion model to the
   *                           same as the NAC - A Taylor Series implementation.  Update to
   *                           the MDIS IK (msgr_mdis_v120.ti) udpates these parameters as
   *                           well as the WAC focal lengths.  The NAC is unaffected by these
   *                           changes. Updated unitTest.
   *   @history 2010-12-20 Kris Becker - Added implementation of CK and SPK
   *                           NAIF codes for the Camera class.
   *   @history 2011-01-14 Travis Addair - Added new CK/SPK accessor methods,
   *                           pure virtual in Camera, implemented in mission specific cameras.
   *   @history 2011-02-09 Steven Lambright - Major changes to camera classes.
   *   @history 2011-03-31 Kris Becker - Fixed bug in handling of pixel
   *                           binning.  Valid values for PixelBinningMode are 0, 2 or 4.
   *                           This was not handled properly until it was used in conjuction
   *                           with FPU binning in orbit.
   *   @history 2011-05-03 Jeannie Walldren - Added ShutterOpenCloseTimes()
   *                           method. Updated unitTest to test for new methods. Updated
   *                           documentation. Removed Messenger namespace wrap inside Isis
   *                           namespace wrap. Added Isis Disclaimer to files. Added NAIF error
   *                           check to constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                           coding standards. References #972.
   *   @history 2012-07-25 Kris Becker - Added temperature dependant focal
   *                           length computation from new IK content.  Fixes #922.
   *   @history 2012-10-25 Kris Becker - Create instrument neutral temperature
   *                           dependent focal length keyword (TempDependentFocalLength) in the
   *                           NaifKeywords group to store this value for easy access.
   *                           Incremented camera version number to 2. Removed backword
   *                           compatibility code supporting older IKs.  The first valid one
   *                           with this version is msgr_mdis_v131.ti. Updated FK (msgr_v220.tf)
   *                           and PCK (pck00010_MSGR_v10.tpc). Reran spiceinit on all MESSENGER
   *                           data in the test suite and updated all appTests (18 total were
   *                           affected by this change).  Fixes #1214.
   *   @history 2015-08-13 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods.
   *   @history 2015-10-01 Ian Humphrey and Makayla Shepherd - Updated check for Wac instrument to
   *                           include instrument codes that correspond to its filters. References
   *                           #2335.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   */
  class MdisCamera : public FramingCamera {
    public:
      MdisCamera(Cube &cube);
      //! Destroys the MdisCamera Object
      ~MdisCamera() {};
      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time,
                                                             double exposureDuration);

      /**
       * CK frame ID -  - MESSENGER instrument code (MSGR_SPACECRAFT)
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-236000); }
      /**
       * CK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (1); }
      /**
       * SPK Target Body ID - MESSENGER spacecraft -236
       *
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Target ID
       */
      virtual int SpkTargetId() const { return (-236); }
      /**
       *  SPK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (1); }

    private:
      double computeFocalLength(const QString &filterCode, Pvl &label);
  };
};
#endif
