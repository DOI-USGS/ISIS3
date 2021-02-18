#ifndef ThemisVisCamera_h
#define ThemisVisCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "PushFrameCamera.h"

namespace Isis {
  /**
   * @brief THEMIS VIS Camera Model
   *
   * This is the camera model for the Thermal Emission Imaging System
   * Visible-Imaging Subsystem (THEMIS VIS) Push Frame camera
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup MarsOdyssey
   *
   * @author 2006-01-06 Elizabeth Ribelin
   *
   * @internal
   *   @history 2006-05-17 Elizabeth Miller - Depricated CameraManager to CameraFactory in unitTest
   *   @history 2006-09-12 Elizabeth Miller - Fixed bug in detector map caused by the deletion of
   *                           the alphacube group in the labels
   *   @history 2008-06-16 Steven Lambright - Made camera work with new push frame classes
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to inherit directly from
   *                           PushFrameCamera instead of Camera
   *   @history 2010-07-27 Jeannie Backer - Fixed documentation.
   *   @history 2010-08-04 Jeannie Backer - Removed Isis namespace wrap around Odyssey namespace
   *                           and replaced with "using namespace Isis". Added NAIF error check to
   *                           constructor.
   *   @history 2010-09-14 Steven Lambright - Updated unitTest to not use a DEM.
   *   @history 2011-01-14 Travis Addair - Added new CK/SPK accessor methods, pure virtual in
   *                           Camera, implemented in mission specific cameras.
   *   @history 2011-02-09 Steven Lambright - Major changes to camera classes.
   *   @history 2011-02-23 Mackenzie Boyd -   Modified focal length from 203.9 to 202.059 per
   *                           request from Christopher Edwards at ASU,
   *                           (Christopher.Edwards@asu.edu). Updated unitTest.
   *   @history 2011-05-03 Jeannie Backer - Fixed documentation.  Replaced Odyssey namespace wrap
   *                           with Isis namespace wrap. Added Isis Disclaimer to files. Updated
   *                           unitTest to test for new methods.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                           coding standards. References #972.
   *   @history 2014-08-22 Jeannie Backer - Fixed bug in camera model to used the FilterNumber
   *                           rather than the OriginalBand to determine the appropriate filter
   *                           for this band.  Moved source code from header to cpp file. Improved
   *                           documentation and ISIS 3 Standards. Improved test coverage to
   *                           80/96/90 %. Plugins are not tested by unitTests. The other lines
   *                           lacking test coverage are addressed in the todo below. References
   *                           #1659
   *   @history 2015-08-12 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to
   *                           test these methods.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   *
   *
   *
   *
   *   @todo 2014-08-22 Jeannie Backer - THEMIS cameras do not appear to import a ReferenceBand
   *                        keyword neither in the thm2isis source code nor in its translation
   *                        tables. In this camera model, BandEphemerisTimeOffset() does a check
   *                        for HasReferenceBand(), which will always be false since ReferenceBand
   *                        doesn't exist. We should consider removing this unnecessary check from
   *                        the camera model or import a ReferenceBand upon ingestion.
   */
  class ThemisVisCamera : public PushFrameCamera {
    public:
      // constructor
      ThemisVisCamera(Cube &cube);

      //! Destroys the Themis Vis Camera object
      ~ThemisVisCamera();

      // Sets the band to the band number given
      void SetBand(const int band);

      double BandEphemerisTimeOffset(int vband);

      bool IsBandIndependent();
      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkReferenceId() const;

    private:
      double p_etStart;           //!< Ephemeris Start iTime
      double p_bandTimeOffset;    //!< Offset iTime for Band
      double p_exposureDur;       //!< Exposure Duration value from labels
      double p_interframeDelay;   //!< Interframe Delay value from labels
      int p_nframes;              //!< Number of frames in whole image
      QList<int> p_filterNumber;  /**< List of filter number values from the Instrument BandBin
                                       group that correspond to each band in the cube. Filter
                                       numbers indicate the physical location of the band in the
                                       detector array.  They are numbered by ascending times.*/
  };
};

#endif
