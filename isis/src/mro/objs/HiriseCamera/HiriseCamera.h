#ifndef HiriseCamera_h
#define HiriseCamera_h
/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2009/08/31 15:12:31 $
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
   * @brief Hirise Camera Model
   *
   * This class is the implementation of the camera model
   * for the MRO HiRISE instrument.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup MarsReconnaissanceOrbiter
   *
   * @author  2005-02-22 Jim Torson
   *
   * @internal
   *  @history 2005-10-03 Elizabeth Miller - Modified file to support Doxygen
   *                           documentation
   *  @history 2008-08-08 Steven Lambright Made the unit test work with a Sensor
   *                           change. Also, now using the new LoadCache(...) method instead of
   *                           createCache(...).
   *  @history 2009-03-08 Debbie A. Cook Removed reference to obsolete LineScanCameraDetectorMap
   *                           method SetXAxisTimeDependent
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                           inherit directly from Camera
   *   @history 2011-05-03 Jeannie Walldren - Updated unitTest to test for new
   *                           methods. Added NAIF error check. Removed Mro namespace.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                           coding standards. References #972.
   *   @history 2015-08-12 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods.
   */
  class HiriseCamera : public LineScanCamera {
    public:
      // Constructs a HiriseCamera object
      HiriseCamera(Cube &cube);

      // Destroys the HiriseCamera object
      ~HiriseCamera();

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *  
       * @return @b int The appropriate instrument code for the "Camera-matrix" 
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-74000); }

      /** 
       *  CK Reference ID - MRO_MME_OF_DATE
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
