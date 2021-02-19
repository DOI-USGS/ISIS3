#ifndef ApolloMetricCamera_h
#define ApolloMetricCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "FramingCamera.h"

namespace Isis {
  /**
   * @brief Apollo Metric Camera Model
   *
   * This is the camera model for the Apollo metric camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Apollo
   *
   * @author 2006-11-14 Jacob Danton
   *
   * @internal
   *   @history 2006-11-14 Jacob Danton - Original Version
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                           inherit directly from Camera
   *   @history 2010-07-20 Sharmila Prasad - Modified documentation to remove
   *                           Doxygen Warning
   *   @history 2011-01-14 Travis Addair - Added new CK/SPK accessor methods,
   *                           pure virtual in Camera, implemented in mission
   *                           specific cameras.
   *   @history 2011-02-09 Steven Lambright - Major changes to camera classes.
   *   @history 2011-05-03 Jeannie Walldren - Added ShutterOpenCloseTimes()
   *                           method. Updated unitTest to test for new methods.
   *                           Updated documentation. Removed Apollo namespace
   *                           wrap inside Isis namespace. Added Isis Disclaimer
   *                           to files.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                           coding standards.   References #972.
   *   @history 2014-01-17 Kris Becker - Set CkReferenceID to J2000 to resolve
   *                           problem with ckwriter. Also, set the SpkReferenceId
   *                           to J2000. References #1737 and #1739.
   *   @history 2015-08-24 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods and added test data for Apollo 16 and 17.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   *   @history 2017-06-28 Makayla Shepherd - Updated documentation. References #4807.
   */
  class ApolloMetricCamera : public FramingCamera {
    public:
      ApolloMetricCamera(Cube &cube);
      //! Destroys the ApolloMetricCamera Object
      ~ApolloMetricCamera() {};
      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time,
                                                             double exposureDuration);

      /**
       * CK frame ID -
       * Apollo 15 instrument code (A15_METRIC) = -915240
       * Apollo 16 instrument code (A16_METRIC) = -916240
       * Apollo 17 instrument code (A17_METRIC) = -917240
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return p_ckFrameId; }

      /**
       * CK Reference ID -
       * APOLLO_15_NADIR = 1
       * APOLLO_16_NADIR = 1
       * APOLLO_17_NADIR = 1
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix" Kernel
       *         Reference ID
       */
      virtual int CkReferenceId() const { return p_ckReferenceId; }

      /**
       * SPK Target Body ID -
       * Apollo 15 = -915
       * Apollo 16 = -916
       * Apollo 17 = -917
       *
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Target ID
       */
      virtual int SpkTargetId() const { return p_spkTargetId; }

      /**
       *  SPK Reference ID - J2000
       *
       *  Even thought the ephemeris is relative to B1950, this specification
       *  is for writing SPK kernels.  We should stay with the J2000 epoch in
       *  these cases.
       *
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Reference ID,
       */
      virtual int SpkReferenceId() const { return (1); }

    private:
      int p_ckFrameId;       //!< "Camera-matrix" Kernel Frame ID
      int p_ckReferenceId;   //!< "Camera-matrix" Kernel Reference ID
      int p_spkTargetId;     //!< Spacecraft Kernel Target ID
  };
};
#endif
