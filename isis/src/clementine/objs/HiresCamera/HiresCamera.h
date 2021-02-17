#ifndef HiresCamera_h
#define HiresCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "FramingCamera.h"

namespace Isis {
  /**
   * This is the camera model for the Clementine High-Resolution Camera
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Clementine
   *
   * @see
   *      http://astrogeology.usgs.gov/Projects/Clementine/nasaclem/sensors/hires/hires.html
   * @see
   *      http://astrogeology.usgs.gov/Projects/Clementine/nasaclem/clemhome.html
   * @see http://pds-imaging.jpl.nasa.gov/portal/clementine_mission.html
   * @see http://astrogeology.usgs.gov/Missions/Clementine
   *
   * @author  2009-01-16 Tracie Sucharski
   *
   * @internal
   *   @history 2009-01-21 Tracie Sucharski - Original version
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                           inherit directly from Camera.  Camera is now pure
   *                           virtual, parent class is FramingCamera.
   *   @history 2010-09-16 Steven Lambright - Updated unitTest to not use a DEM.
   *   @history 2011-01-14 Travis Addair - Added new CK/SPK accessor methods,
   *                           pure virtual in Camera, implemented in mission
   *                           specific cameras.
   *   @history 2011-02-09 Steven Lambright - Major changes to camera classes.
   *   @history 2011-05-03 Jeannie Walldren - Added ShutterOpenCloseTimes()
   *                           method. Updated unitTest to test for new methods.
   *                           Updated documentation. Replaced Clementine
   *                           namespace wrap with Isis namespace. Added Isis
   *                           Disclaimer to files. Added NAIF error check to
   *                           constructor. Changed centertime in constructor to
   *                           add half exposure duration to start time to
   *                           maintain consistency with other Clementine models.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                           coding standards. References #972.
   *   @history 2015-08-12 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   */
  class HiresCamera : public FramingCamera {
    public:
      HiresCamera(Cube &cube);
      //! Destroys the HiresCamera object.
      ~HiresCamera() {};
      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time,
                                                             double exposureDuration);

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-40000); }

      /**
       * CK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (1); }

      /**
       * SPK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (1); }
  };
};
#endif
