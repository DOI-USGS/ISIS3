#ifndef MexHrscSrcCamera_h
#define MexHrscSrcCamera_h
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

#include <QString>

namespace Isis {
  /**
   * This is the camera model for the Mex HRSC SRC Framing Camera 
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Mex
   *
   * @author 2020-04-02 Stuart Sides
   *
   * @internal 
   *   @history 2020-04-01 Stuart Sides - Initial version

   */
  class MexHrscSrcCamera : public FramingCamera {
    public:
      //! Create a MexHrscSrcCamera object
      MexHrscSrcCamera(Cube &cube);

      //! Destroys the MexHrscSrcCamera object
      ~MexHrscSrcCamera() {};

     /** 
      * Reimplemented from FrameCamera 
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
      virtual int CkFrameId() const { return (-41001); }


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
