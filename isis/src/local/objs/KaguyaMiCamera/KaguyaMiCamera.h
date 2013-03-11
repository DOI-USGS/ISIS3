#ifndef KaguyaMiCamera_h
#define KaguyaMiCamera_h
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

#include "LineScanCamera.h"

namespace Isis {
  /**
   * @brief LRO Narrow Angle Camera Model
   *
   * This is the camera model for the Kaguya Multiband pushbroom imagers (VIR and NIR)
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Kaguya
   *
   * @author 2009-02-20 Jacob Danton
   *
   * @internal
   *   @history 2012-06-14 Orrin Thomas - original version
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                          coding standards. References #972.
   */
  class KaguyaMiCamera : public LineScanCamera {
    public:
      KaguyaMiCamera(Cube &cube);

      //! Destroys the LroNarrowAngleCamera object
      ~KaguyaMiCamera() {};

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *  
       * @return @b int The appropriate instrument code for the "Camera-matrix" 
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-131000); }

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
  };
};
#endif
