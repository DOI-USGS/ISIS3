#ifndef MarciCamera_h
#define MarciCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "PushFrameCamera.h"

namespace Isis {
  /**
   * @brief Marci Camera Model
   *
   * This is the camera model for the MARCI Instrument
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup MarsReconnaissanceOrbiter
   *
   * @author 2008-10-23 Steven Lambright
   *
   * @internal
   *   @history 2008-11-19 Steven Lambright - Added distortion model, made VIS bands other than
   *                           orange work
   *   @history 2008-11-25 Steven Lambright - The coloroffset now works properly; if an offset is
   *                           supplied in marci2isis the cube will still project properly.
   *   @history 2009-03-17 Steven Lambright - Fixed UV to work with the distortion
   *                           model correctly
   *   @history 2009-05-21 Steven Lambright - Fixed GeometricTilingHint for summed images
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer inherit directly from
   *                           Camera
   *   @history 2011-05-03 Jeannie Walldren - Updated unitTest to test for new methods. Updated
   *                           documentation. Removed Mro namespace wrap inside Isis namespace.
   *                           Added Isis Disclaimer to files. Added NAIF error
   *                           check to constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                          coding standards. References #972.
   *   @history 2014-04-17 Jeannie Backer - Updated due to method name change in
   *                           PushFrameCameraDetectorMap. Moved method implementations to cpp file.
   *                           References #1659
   *   @history 2015-08-12 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   */
  class MarciCamera : public PushFrameCamera {
    public:
      // constructor
      MarciCamera(Cube &cube);
      ~MarciCamera();

      // Sets the band to the band number given
      void SetBand(const int band);
      bool IsBandIndependent();

      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkReferenceId() const;

    private:
      void StoreCoefficients(int naifIkCode);
      void RestoreCoefficients(int vband);

      double p_etStart;              //!< Ephemeris Start iTime
      double p_bandTimeOffset;       //!< Offset iTime for Band
      double p_exposureDur;          //!< Exposure Duration value from labels
      double p_interframeDelay;      //!< Interframe Delay value from labels
      int p_nframelets;                 //!< Number of framelets in whole image
      std::vector<int> p_detectorStartLines;
      std::vector<int> p_filterNumbers;
      std::vector<int> p_frameletOffsets;
  };
};
#endif
