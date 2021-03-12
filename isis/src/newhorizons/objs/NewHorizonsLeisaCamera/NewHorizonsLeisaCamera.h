#ifndef NewHorizonsLeisaCamera_h
#define NewHorizonsLeisaCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "LineScanCamera.h"

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
   * @history 2015-08-26 Kristin Berry - Updated error condition in SetBand and
   *                          Camera::SetBand call to set the virtual band.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
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

    private:
      QVector<int> m_originalBand; //!< Stores the band bin OriginalBand keyword values

      QVector<double> m_origTransx;  //!< The original transx affine coefficients from the iak
      QVector<double> m_origTransy;  //!< The original transy affine coefficients from the iak
      QVector<double> m_origTranss; //!< The original transs affine coefficients from the iak
      QVector<double> m_origTransl; //!< The original transl affine coefficients from the iak
  };
};
#endif
