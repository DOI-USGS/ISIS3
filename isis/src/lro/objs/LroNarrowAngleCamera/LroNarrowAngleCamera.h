#ifndef LroNarrowAngleCamera_h
#define LroNarrowAngleCamera_h
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

#include <QString>

namespace Isis {
  /**
   * @brief LRO Narrow Angle Camera Model
   *
   * This is the camera model for the Lunar Reconnaissance Orbiter narrow angle 
   * camera. 
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup LunarReconnaissanceOrbiter
   *
   * @author 2009-02-20 Jacob Danton
   *
   * @internal
   *   @history 2009-02-20  Jacob Danton, Original Object
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                           inherit directly from Camera
   *   @history 2011-05-03 Jeannie Walldren - Updated unitTest to test for new
   *                           methods. Updated documentation. Removed Lro namespace wrap
   *                           inside Isis namespace wrap. Added Isis Disclaimer to files.
   *                           Added NAIF error check to constructor.
   *   @history 2015-08-14 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           added methods and added test data for LRO NACL.
   */
  class LroNarrowAngleCamera : public LineScanCamera {
    public:
      LroNarrowAngleCamera(Cube &cube);

      //! Destroys the LroNarrowAngleCamera object
      ~LroNarrowAngleCamera() {};

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
      
      virtual QString instrumentNameLong() const;
      virtual QString instrumentNameShort() const;
      virtual QString spacecraftNameLong() const;
      virtual QString spacecraftNameShort() const;

    private:
      QString m_instrumentNameLong; //!< Full instrument name
      QString m_instrumentNameShort; //!< Shortened instrument name
      QString m_spacecraftNameLong; //!< Full spacecraft name
      QString m_spacecraftNameShort; //!< Shortened spacecraft name
  };
};
#endif
