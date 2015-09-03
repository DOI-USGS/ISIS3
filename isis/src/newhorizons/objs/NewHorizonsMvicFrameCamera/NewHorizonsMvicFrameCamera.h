#ifndef NewHorizonsMvicFrameCamera_h
#define NewHorizonsMvicFrameCamera_h
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
   * This is the camera model for the New Horizons MVIC Frame mode Camera 
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup New Horizons
   *
   * @author 2014-03-31 Tracie Sucharski
   *
   * @internal 
   *   @history 2015-08-11 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           added methods.
   */
  class NewHorizonsMvicFrameCamera : public FramingCamera {
    public:
      //! Create a NewHorizonsMvicFrameCamera object
      NewHorizonsMvicFrameCamera(Cube &cube);


      //! Destroys the NewHorizonsMvicFrameCamera object
      ~NewHorizonsMvicFrameCamera() {};


      // Sets the band to the band number given
      void SetBand(const int vband);


//      /**
//       * The camera model is band dependent, so this method returns false
//       *
//       * @return bool False
//       */
////    bool IsBandIndependent() {
////      return false;
////    };


      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time, 
                                                             double exposureDuration);


      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *  
       * @return @b int The appropriate instrument code for the "Camera-matrix" 
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-98000); }


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

      virtual QString instrumentNameLong() const;
      virtual QString instrumentNameShort() const;
      virtual QString spacecraftNameLong() const;
      virtual QString spacecraftNameShort() const;

    private:
      QList<int> m_originalBand;
      QList<QString> m_utcTime;
      double m_etStart;
      double m_exposure;
      
      QString m_instrumentNameLong; //!< Full instrument name
      QString m_instrumentNameShort; //!< Shortened instrument name
      QString m_spacecraftNameLong; //!< Full spacecraft name
      QString m_spacecraftNameShort; //!< Shortened spacecraft name
  };
};
#endif
