#ifndef NormModel_h
#define NormModel_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <vector>
#include "Pvl.h"
#include "PhotoModel.h"
#include "AtmosModel.h"

namespace Isis {

  /**
   * @brief
   *
   * @author 1998-12-21 Randy Kirk
   *
   * @internal
   *  @history 2007-08-15 Steven Lambright - Refactored
   *  @history 2008-03-07 Janet Barrett - Added p_normWavelength
   *                      variable and SetNormWavelength method to
   *                      allow an application to set this value to
   *                      the BandBin Center keyword value for use
   *                      with MoonAlbedo normalization. The application
   *                      needs to call the SetPhotomWl method in the
   *                      Photometry class and the SetNormWavelength
   *                      method is called from the Photometry class.
   *  @history 2008-06-18 Steven Koechle - Fixed Documentation Errors
   *  @history 2008-07-09 Steven Lambright - Fixed unit test
   */
  class NormModel {
    public:
      NormModel(Pvl &pvl, PhotoModel &pmodel);
      NormModel(Pvl &pvl, PhotoModel &pmodel, AtmosModel &amodel);
      virtual ~NormModel() {};

      //! Return normalization algorithm name
      std::string AlgorithmName() const {
        return p_normAlgorithmName;
      };

      //! Calculate the albedo normalization
      void CalcNrmAlbedo(double pha, double inc, double ema, double dn,
                         double &albedo, double &mult, double &base);
      void CalcNrmAlbedo(double pha, double inc, double ema, double deminc,
                         double demema, double dn, double &albedo,
                         double &mult, double &base);
      virtual void SetNormWavelength(double wavelength);

    protected:
      virtual void NormModelAlgorithm(double pha, double inc, double ema,
                                      double dn, double &albedo, double &mult, double &base) = 0;

      virtual void NormModelAlgorithm(double pha, double inc, double ema,
                                      double deminc, double demema, double dn, double &albedo,
                                      double &mult, double &base) = 0;

      void SetAlgorithmName(std::string name) {
        p_normAlgorithmName = name;
      }
      PhotoModel *GetPhotoModel() {
        return p_normPM;
      }
      AtmosModel *GetAtmosModel() {
        return p_normAM;
      }

      double p_normWavelength;

    private:
      std::string p_normAlgorithmName;
      PhotoModel *p_normPM;
      AtmosModel *p_normAM;
  };
};

#endif
