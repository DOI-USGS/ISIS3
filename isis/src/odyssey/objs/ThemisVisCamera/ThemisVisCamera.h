#ifndef ThemisVisCamera_h
#define ThemisVisCamera_h
/**
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

using namespace Isis;
namespace Odyssey {
  /**
   * @brief Themis VIS Camera Model
   *
   * This is the camera model for the Themis Vis Push Frame camera
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
   *   @history 2010-08-04 Jeannie Walldren - Fixed documentation.  Removed Isis
   *                                          namespace wrap around Odyssey
   *                                          namespace and replaced with "using
   *                                          namespace Isis".  Added NAIF error
   *                                          check to constructor.
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

      /** CK Frame ID - Instrument Code from spacit run on CK */
      virtual int CkFrameId() const { return (-53000); }

      /** CK Reference ID - MARSIAU */
      virtual int CkReferenceId() const { return (16); }

      /** SPK Reference ID - J2000 */
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

