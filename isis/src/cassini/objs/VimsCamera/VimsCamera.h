#ifndef VimsCamera_h
#define VimsCamera_h
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

#include "Camera.h"

namespace Isis {
  /** 
   *  @brief Cassini Vims camera model
   *
   *   This is the camera model for the Cassini Vims instrument
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Cassini-Huygens
   *
   * @see Camera
   *
   * @internal
   *
   *   @history 2006-03-16 Tracie Sucharski Original version
   *   @history 2009-04-06 Steven Lambright Fixed problem that caused double
   *                          deletion of sky map / ground map.
   *   @history 2009-08-03 Debbie A. Cook - Added new tolerance argument to
   *                          CreateCache call to be compatible with update to
   *                          Spice class
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                          inherit directly from Camera
   *   @history 2011-01-14 Travis Addair - Added new CK/SPK accessor methods,
   *                          pure virtual in Camera, implemented in mission
   *                          specific cameras.
   *   @history 2011-02-09 Steven Lambright - Major changes to camera classes.
   *   @history 2010-08-05 Jeannie Walldren - Updated documentation. Removed
   *                          Cassini namespace wrap inside Isis namespace wrap.
   *                          Added NAIF error check to constructor.
   *   @history 2011-05-03 Jeannie Walldren - Updated unitTest to test for new
   *                          methods. Updated documentation. Removed Cassini
   *                          namespace wrap inside Isis namespace wrap. Added
   *                          Isis Disclaimer to files. Added NAIF error check
   *                          to constructor.
   *
   */
  class VimsCamera : public Camera {
    public:
      // constructors
      VimsCamera(Pvl &lab);

      //! Destroys the VimsCamera object.
      ~VimsCamera() {};

      /**
       * The Vims camera is the only point camera we have.
       *  
       * @return CameraType Camera::Point
       */
      virtual CameraType GetCameraType() const {
        return Point;
      }

//       void SetBand (const int physicalBand);
//       bool IsBandIndependent () { return false; };

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *  
       * @return @b int The appropriate instrument code for the "Camera-matrix" 
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-82000); }

      /** 
       * CK Reference ID - J2000
       * 
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (1); }

      /** 
       *  SPK Center ID - 6 (Saturn)
       *  
       * @return @b int The appropriate instrument code for the Spacecraft 
       *         Kernel Center ID
       */
      virtual int SpkCenterId() const { return 6; }

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
