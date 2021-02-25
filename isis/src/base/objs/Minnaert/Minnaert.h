#ifndef Minnaert_h
#define Minnaert_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "PhotoModel.h"

namespace Isis {
  class Pvl;

  /**
   * @brief Minnaert photometric model
   *  Derive model albedo using Minnaert equation.
   *  Phase independent and calculated analytically.
   *  Limb-darkening k is a constant.
   *  \code
   *  albedo = brightness*[mu / (mu*munot)**k)]
   *  assumptions:
   *    1. bidirectional reflectance
   *    2. semi-infinite medium
   *                                               k      k-1
   *  reflectance (inc,ema,phase)=albedo  *  munot   * mu
   *           Minnaert               Minnaert
   *  \endcode
   *
   *  Where k is the Minnaert index, an empirical constant (called nu in Hapke)
   *
   *  If k (nu) = 1, Minnaert's law reduces to Lambert's law.
   *  See Theory of Reflectance and Emittance Spectroscopy, 1993;
   *  Bruce Hapke; pg. 191-192.
   *
   * @author 1989-08-02 Tammy Becker
   *
   * @internal
   *  @history 2007-07-31 Steven Lambright - Moved PhotoK from base PhotoModel class to this
   *                                           child.
   */
  class Minnaert : public PhotoModel {
    public:
      Minnaert(Pvl &pvl);
      virtual ~Minnaert() {};

      void SetPhotoK(const double k);
      //! Return photometric K value
//      inline double PhotoK() const {
//        return p_photoK;
//      };

    protected:
      virtual double PhotoModelAlgorithm(double phase, double incidence,
                                         double emission);

    private:
//      double p_photoK;

  };
};

#endif
