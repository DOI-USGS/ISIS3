#ifndef MarciCamera_h
#define MarciCamera_h
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
   *   @history 2008-11-19 Steven Lambright - Added distortion model, made VIS
   *                      bands other than orange work
   *   @history 2008-11-25 Steven Lambright - The coloroffset now works properly;
   *            if an offset is supplied in marci2isis the cube will still project
   *            properly.
   *   @history 2009-03-17 Steven Lambright - Fixed UV to work with the distortion
   *            model correctly
   *   @history 2009-05-21 Steven Lambright - Fixed GeometricTilingHint for summed
   *            images
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *            inherit directly from Camera
   *   @history 2011-05-03 Jeannie Walldren - Updated unitTest to test for new
   *            methods. Updated documentation. Removed Mro namespace wrap
   *            inside Isis namespace. Added Isis Disclaimer to files. Added
   *            NAIF error check to constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                          coding standards. References #972.
   */
  class MarciCamera : public PushFrameCamera {
    public:
      // constructor
      MarciCamera(Pvl &lab);

      //! Destroys the Themis Vis Camera object
      ~MarciCamera() {};

      // Sets the band to the band number given
      void SetBand(const int band);

      /**
       * The camera model is band dependent, so this method returns false
       *
       * @return bool False
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
      virtual int CkFrameId() const { return (-74000); }

      /** 
       * CK Reference ID - MRO_MME_OF_DATE
       * 
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (-74900); }

      /** 
       *  SPK Reference ID - J2000
       *  
       * @return @b int The appropriate instrument code for the Spacecraft 
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (1); }

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

