#ifndef LroWideAngleCamera_h
#define LroWideAngleCamera_h
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

#include "PushFrameCamera.h"

namespace Isis {
  /**
   * @brief LRO Wide Angle Camera Model
   *
   * This is the camera model for the Lunar Reconnaissance Orbiter wide angle 
   * camera. 
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup LunarReconnaissanceOrbiter
   *
   * @author 2009-07-08 Jeff Anderson
   *
   * @internal
   *   @history 2009-07-15 Steven Lambright - Added support for COLOROFFSET
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *            inherit directly from Camera
   *   @history 2009-09-09 Steven Lambright - Updated wavelengths
   *   @history 2009-11-06 Steven Lambright - FilterName keyword is now Center
   *   @history 2010-03-15 Steven Lambright - Tiling hint now set to a safe
   *            value regardless of output projection resolution. Also
   *            incorporated ASU's changes for new modes.
   *   @history 2010-05-12 Kris Becker - Added checks for number of bands to
   *            match number of values in BandBin/Center keyword and insure a
   *            valid band is selected in SetBand() method;  Rewrote the
   *            camera distortion model that also requires negative
   *            coefficients in IK kernel.
   *   @history 2010-08-21 Kris Becker - Reworked the camera model to
   *          utilize the contents of the IK, which is new. The LRO/LROC IK
   *          lro_lroc_v14.ti and higher contain the appropriate parameters to
   *          coincide with the code changes made here. IMPORTANT:  This
   *          results in Version = 2 of the LroWideAngleCamera as depicted in
   *          the Camera.plugin for both WAC-UV and WAC-VIS.
   *   @history 2010-10-04 Kris Becker - Modified the frame kernel code to use
   *            the instrument code instead of the WAC ID.  This change was
   *            brought about with the release of frames kernel
   *            lro_frames_2010214_v01.tf (actually used version 2010277 that
   *            contains updated angles for VIS and UV).
   *   @history 2011-05-03 Jeannie Walldren - Updated unitTest to test for new
   *            methods. Updated documentation. Removed Lro namespace wrap
   *            inside Isis namespace wrap. Added Isis Disclaimer to files.
   *            Added NAIF error check to constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *            coding standards. References #972.
   *  
   */
  class LroWideAngleCamera : public PushFrameCamera {
    public:
      // constructor
      LroWideAngleCamera(Cube &cube);

      //! Destroys the LroWideAngleCamera object
      ~LroWideAngleCamera() {};

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
      virtual int CkFrameId() const { return (-85000); }

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

    private:
      double p_etStart;              //!< Ephemeris Start iTime
      double p_bandTimeOffset;       //!< Offset iTime for Band
      double p_exposureDur;          //!< Exposure Duration value from labels
      double p_interframeDelay;      //!< Interframe Delay value from labels
      int p_nframelets;                 //!< Number of framelets in whole image
      std::vector<int> p_detectorStartLines;
      std::vector<int> p_frameletOffsets;

      int PoolKeySize(const QString &key) const;
      std::vector<int> GetVector(const QString &key);
  };
};
#endif

