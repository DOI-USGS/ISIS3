#ifndef ThemisVisCamera_h
#define ThemisVisCamera_h
/**
 * @file
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

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
   *   @history 2014-08-22 Jeannie Backer - Improved test coverage to 80/96/90 %. Plugins are
   *                           not tested by unitTests. The other lines lacking test coverage are
   *                           addressed in the todo below. References #1659
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

