#ifndef ThemisIrCamera_h
#define ThemisIrCamera_h
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

#include <QString>

namespace Isis {
  class ThemisIrDistortionMap;

  /**
   * @brief THEMIS IR Camera
   *
   * This is the camera model for the Thermal Emission Imaging System 
   * Infrared (THEMIS IR) camera 
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup MarsOdyssey
   *
   * @author  2005-01-01 Jeff Anderson
   *
   * @internal
   *   @history 2007-07-13 Jeff Anderson Added support for spatial summing
   *   @history 2008-08-08 Steven Lambright Now using the new LoadCache(...)
   *                           method instead of CreateCache(...).
   *   @history 2009-05-12 Jeff Anderson Reworked code for changes from
   *                           Kiefer-Torson model to Duxbury model.  The majors changes
   *                           where the removal of a ~1% error in focal lenght and the
   *                           improving the spectral band registration to better than
   *                           1/20th of a pixel
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                           inherit directly from Camera
   *   @history 2011-05-03 Jeannie Walldren - Updated unitTest to test for new methods.
   *                           Updated documentation. Removed Odyssey namespace
   *                           wrap inside Isis namespace. Added Isis Disclaimer
   *                           to files. Added NAIF error check to constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                           coding standards. References #972.
   *   @history 2015-08-12 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods.
   */
  class ThemisIrCamera : public LineScanCamera {
    public:
      // constructors
      ThemisIrCamera(Cube &cube);

      //! Destroys the ThemisIrCamera object.
      ~ThemisIrCamera() {};

      // Band dependent
      void SetBand(const int band);
      bool IsBandIndependent() {
        return false;
      };

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *  
       * @return @b int The appropriate instrument code for the "Camera-matrix" 
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-53000); }

      /** 
       * CK Reference ID - J2000
       * 
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (16); }

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
      double p_etStart;
      double p_lineRate;
      double p_bandTimeOffset;
      QString p_tdiMode;
      QString m_instrumentNameLong; //!< Full instrument name
      QString m_instrumentNameShort; //!< Shortened instrument name
      QString m_spacecraftNameLong; //!< Full spacecraft name
      QString m_spacecraftNameShort; //!< Shortened spacecraft name
      std::vector<int> p_originalBand;
  };
  
};
#endif
