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
   *   @history 2006-05-17 Elizabeth Miller - Depricated CameraManager to
   *                                          CameraFactory in unitTest
   *   @history 2006-09-12 Elizabeth Miller - Fixed bug in detector map caused
   *                                         by the deletion of the alphacube
   *                                         group in the labels
   *   @history 2008-06-16 Steven Lambright - Made camera work with new push
   *                                          frame classes
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                                          inherit directly from Camera
   *   @history 2010-07-27 Jeannie Walldren - Fixed documentation.
   *   @history 2010-08-04 Jeannie Walldren - Removed Isis namespace wrap around
   *                                          Odyssey namespace and replaced
   *                                          with "using namespace Isis".
   *                                          Added NAIF error check to
   *                                          constructor.
   *   @history 2010-09-14 Steven Lambright - Updated unitTest to not use a DEM.
   *   @history 2011-01-14 Travis Addair - Added new CK/SPK accessor methods,
   *                                          pure virtual in Camera,
   *                                          implemented in mission specific
   *                                          cameras.
   *   @history 2011-02-09 Steven Lambright - Major changes to camera classes.
   *   @history 2011-02-23 Mackenzie Boyd -   Modified pixel pitch from 203.9
   *                                          to 202.059 per request from
   *                                          Christopher Edwards at ASU,
   *                                          (Christopher.Edwards@asu.edu).
   *                                          Updated unitTest.
   *   @history 2011-05-03 Jeannie Walldren - Fixed documentation.  Replaced
   *                                          Odyssey namespace wrap with Isis
   *                                          namespace wrap. Added Isis
   *                                          Disclaimer to files. Updated
   *                                          unitTest to test for new methods.
   */
  class ThemisVisCamera : public PushFrameCamera {
    public:
      // constructor
      ThemisVisCamera(Pvl &lab);

      //! Destroys the Themis Vis Camera object
      ~ThemisVisCamera() {};

      // Sets the band to the band number given
      void SetBand(const int band);

      double BandEphemerisTimeOffset(int vband);

      /**
       * The camera model is band dependent, so this method returns false
       *
       * @return @b bool This will always return False.
       */
      bool IsBandIndependent() {
        return false;
      };

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *  
       * @return @b int The appropriate instrument code for the "Camera-matrix" 
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-53000); }

      /** 
       * CK Reference ID - MARSIAU
       * 
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (16); }

      /** 
       * SPK Reference ID - J2000
       * 
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (1); }

    private:
      double p_etStart;                 //!< Ephemeris Start iTime
      double p_bandTimeOffset;          //!< Offset iTime for Band
      double p_exposureDur;             //!< Exposure Duration value from labels
      double p_interframeDelay;         //!< Interframe Delay value from labels
      int p_nframes;                    //!< Number of frames in whole image
      std::vector<int> p_originalBand;  //!< Vector of order for original band numbers
  };
};

#endif

