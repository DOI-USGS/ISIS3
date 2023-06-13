#ifndef MinnaertEmpirical_h
#define MinnaertEmpirical_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "NumericalApproximation.h"
#include "PhotoModel.h"

namespace Isis {
  class Pvl;

  /**
   * @brief Empirical Minnaert photometric model
   *  Derive model albedo using phase dependent Minnaert equation
   *  and calculated empirically.
   *  Limb-darkening k and phase function are arbitrary polynomials
   *  in phase angle.
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
   * @author 1999-01-08 Randy Kirk
   *
   * @internal
   *  @history 2011-08-17 Janet Barrett - Ported from ISIS2 to ISIS3.
   */
  class MinnaertEmpirical : public PhotoModel {
    public:
      MinnaertEmpirical(Pvl &pvl);
      virtual ~MinnaertEmpirical();

      void SetPhotoPhaseList(QString phasestrlist);
      void SetPhotoPhaseList(PvlKeyword phaselist);
      void SetPhotoKList(QString kstrlist);
      void SetPhotoKList(PvlKeyword kstrlist);
      void SetPhotoPhaseCurveList(QString phasecurvestrlist);
      void SetPhotoPhaseCurveList(PvlKeyword phasecurvestrlist);

      //! Return photometric phase angle list
//      inline std::vector<double> PhotoPhaseList() const {
//        return p_photoPhaseList;
//      };
      //! Return photometric k value list
//      inline std::vector<double> PhotoKList() const {
//        return p_photoKList;
//      };
      //! Return photometric phase curve value list
//      inline std::vector<double> PhotoPhaseCurveList() const {
//        return p_photoPhaseCurveList;
//      };

      virtual double PhotoModelAlgorithm(double phase, double incidence,
                                         double emission);

    private:
//      int p_photoPhaseAngleCount;
//      std::vector<double> p_photoPhaseList;
//      std::vector<double> p_photoKList;
//      std::vector<double> p_photoPhaseCurveList;
//      NumericalApproximation p_photoKSpline;
//      NumericalApproximation p_photoBSpline;
  };
};

#endif
