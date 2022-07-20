// $Id: IssNACamera.h,v 1.7 2009/08/31 15:12:29 slambright Exp $
#ifndef IssNACamera_h
#define IssNACamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "FramingCamera.h"

namespace Isis {
  /**
   * @brief Cassini ISS Narrow Angle Camera Model
   *
   *     * This is the camera model for the Cassini Imaging Science Subsystem Narrow
   * Angle Camera
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Cassini-Huygens
   *
   * @see
   *      http://saturn.jpl.nasa.gov/spacecraft/cassiniorbiterinstruments/instrumentscassiniiss
   * @see http://pds-imaging.jpl.nasa.gov/portal/cassini_mission.html
   * @see http://astrogeology.usgs.gov/Missions/Cassini
   *
   * @author  2007-07-10 Steven Koechle
   *
   * @internal
   *   @history 2007-07-10 Steven Koechle - Original Version
   *   @history 2007-07-10 Steven Koechle - Removed hardcoding of NAIF Instrument number
   *   @history 2007-07-11 Steven Koechle - casted NaifIkCode to int before iString to fix
   *                           problem on Linux 32bit
   *   @history 2008-08-08 Steven Lambright Now using the new LoadCache(...) method instead of
   *                           CreateCache(...).
   *   @history 2009-01-22 Kris Becker Added new frame rotation to the CK frame hierarchy to
   *                          convert to detector coordinates. This is essentially a 180 degree
   *                          rotation.  The frame definition is actually contained in the IAK.
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer inherit directly
   *                           from Camera
   *   @history 2011-01-14 Travis Addair - Added new CK/SPK accessor methods, pure virtual in
   *                           Camera, implemented in mission specific cameras
   *   @history 2011-02-09 Steven Lambright - Major changes to camera classes documentation.
   *   @history 2011-05-03 Jeannie Walldren - Added ShutterOpenCloseTimes() method. Updated
   *                           unitTest to test for new methods. Updated documentation. Replaced
   *                           Cassini namespace wrap with ISISnamespace. Added ISISDisclaimer
   *                           to files. Added NAIF error check to constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                           coding standards. References #972.
   *   @history 2012-09-12 Stuart C. Sides -  Added ability for the camera to use the default
   *                           focal length if the observation was taken using a filter
   *                           combination not supported in the calibration report. Fixes #837.
   *   @history 2015-08-11 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   *   @history 2022-07-14 Amy Stamile - Removed SpkCenterId function due to spkwriter writing
   *                           positions of Cassini relative to Titan (NAIF ID 606) but labeling
   *                           it in the kernel as the position relative to the Saturn Barycenter
   *                           (NAIF ID 6) Reference #4942.
   */
  class IssNACamera : public FramingCamera {
    public:
      IssNACamera(Cube &cube);
      //! Destroys the IssNACamera object.
      ~IssNACamera() {};
      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time,
                                                             double exposureDuration);

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-82000); }

      /**
       * CK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (1); }

      /**
       *  SPK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (1); }
  };
};
#endif
