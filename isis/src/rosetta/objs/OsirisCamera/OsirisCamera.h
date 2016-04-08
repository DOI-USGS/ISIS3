#ifndef OsirisCamera_h
#define OsirisCamera_h
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

#include "FramingCamera.h"

namespace Isis {
  /**
   * This is the camera model for the Osiris NAC Framing Camera 
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Rosetta
   *
   * @author 2015-05-21 Sasha Brownsberger
   *
   * @internal 
   *  
   */
  class OsirisCamera : public FramingCamera {
    public:
      //! Create a OsirisNacCamera object
      OsirisCamera(Cube &cube);

      //! Destroys the NewHorizonsLorriCamera object
      ~OsirisCamera() {};

    /** 
     * Reimplemented from FrameCamera 
     *  
     * @original author Stuart Sides
     *
     * @modified Sasha Brownsberger (2015/05/21)
     * 
     * @param time Start time of the observation
     * @param exposureDuration The exposure duration of the observation
     * 
     * @return std::pair<iTime,iTime> The start and end times of the observation
     */
      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time,
                                                             double exposureDuration);

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *  
       * @return @b int The appropriate instrument code for the "Camera-matrix" 
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-226000); } //Code for Rosetta orbitter; no specific code for Osiris in ck files.  

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
