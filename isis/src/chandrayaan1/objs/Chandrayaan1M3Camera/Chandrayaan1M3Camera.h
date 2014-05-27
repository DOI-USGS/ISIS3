#ifndef Chandrayaan1M3Camera_h
#define Chandrayaan1M3Camera_h
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

#include "LineScanCamera.h"

namespace Isis {
  /**
   * @brief Chandrayaan1 M3 Camera Model
   *
   * This is the camera model for the Chandrayaan1 M3 (Moon Mineralogy Mapper) Camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Chandrayaan1
   *
   * @author 2013-08-18 Stuart Sides
   *
   * @internal
   *
   */
  class Chandrayaan1M3Camera : public LineScanCamera {
    public:
      Chandrayaan1M3Camera(Cube &cube);

      //! Destroys the Chandrayaan1M3Camera object.
      ~Chandrayaan1M3Camera() {};

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *  
       * @return @b int The appropriate instrument code for the "Camera-matrix" 
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-86000); }

      /** 
       *  CK Reference ID - J2000
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
