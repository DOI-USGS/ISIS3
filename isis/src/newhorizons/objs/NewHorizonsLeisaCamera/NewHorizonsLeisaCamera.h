#ifndef NewHorizonsLeisaCamera_h
#define NewHorizonsLeisaCamera_h
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
   * This is the camera model for LEISA, New Hoirzon's infrared 
   * Spectrometer.  
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup New Horizons
   *
   * @author 2014-09-02 Kristin Berry
   *
   * @internal 
   *  
   */
  class NewHorizonsLeisaCamera : public LineScanCamera {
    public:
      //! Create a NewHorizonsLeisaCamera object
      NewHorizonsLeisaCamera(Cube &cube);

      //! Destroys the NewHorizonsLeisaCamera object
      ~NewHorizonsLeisaCamera() {};

       /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *  
       * @return @b int The appropriate instrument code for the "Camera-matrix" 
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-98000); }//from NAIF_INSTRUMENT_ID in /usgs/cpkgs/isis3/data/newhorizons/kernels/ck/*.lbl or spacit R or spacit S on ck

      /** 
       *  CK Reference ID - 
       * 
       * @return @b int The appropriate code for the "Camera-matrix"  
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (1); } //can get from spacit S on ck

      /** 
       *  SPK Reference ID - J2000
       *  
       * @return @b int The appropriate instrument code for the Spacecraft 
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (1); } //can get from spacit S on spk
  };
};
#endif
