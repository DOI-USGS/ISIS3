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

#include <QString>
#include <QVector>

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
   *   @history 2015-08-11 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           name methods.
   *  
   */
  class NewHorizonsLeisaCamera : public LineScanCamera {
    public:
      //! Create a NewHorizonsLeisaCamera object
      NewHorizonsLeisaCamera(Cube &cube);

      //! Destroys the NewHorizonsLeisaCamera object
      ~NewHorizonsLeisaCamera() {};

      //! Flag that NewHorizonsLeisaCamera is band-dependent. 
      bool IsBandIndependent() {
        return false;
      };

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


      void SetBand(const int vband);

      virtual QString instrumentNameLong() const;
      virtual QString instrumentNameShort() const;
      virtual QString spacecraftNameLong() const;
      virtual QString spacecraftNameShort() const;
   
    private:
      QVector<int> m_originalBand; //!< Stores the band bin OriginalBand keyword values

      QVector<double> m_origTransx;  //!< The original transx affine coefficients from the iak
      QVector<double> m_origTransy;  //!< The original transy affine coefficients from the iak
      QVector<double> m_origTranss; //!< The original transs affine coefficients from the iak
      QVector<double> m_origTransl; //!< The original transl affine coefficients from the iak
      
      QString m_instrumentNameLong; //!< Full instrument name
      QString m_instrumentNameShort; //!< Shortened instrument name
      QString m_spacecraftNameLong; //!< Full spacecraft name
      QString m_spacecraftNameShort; //!< Shortened spacecraft name
  };
};
#endif
