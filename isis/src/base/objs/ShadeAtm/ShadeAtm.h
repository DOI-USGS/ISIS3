#ifndef ShadeAtm_h
#define ShadeAtm_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "NormModel.h"

namespace Isis {
  class Pvl;

  /**
   * @brief
   *
   * @ingroup RadiometricAndPhotometricCorrection
   * @author 1998-12-21 Randy Kirk
   *
   * @internal
   *  @history 2007-08-15 Steven Lambright - Refactored code
   *  @history 2008-11-05 Jeannie Walldren - Modified references
   *           to NumericalMethods class.
   *  @history 2009-05-11 Janet Barrett - Fixed so that the NormModelAlgorithm
   *           supporting DEM input is the empty function. DEM input is not yet
   *           supported.
   *  @history 2010-11-10 Janet Barrett - Added reference parameters for
   *           phase and emission so user can specify normalization
   *           conditions in initialization
   *  @history 2010-11-30 Janet Barrett - Added ability to use photometric angles
   *           from the ellisoid and the DEM
   *
   */
  class ShadeAtm : public NormModel {
    public:
      ShadeAtm(Pvl &pvl, PhotoModel &pmodel, AtmosModel &amodel);
      virtual ~ShadeAtm() {};

    protected:
      virtual void NormModelAlgorithm(double pha, double inc, double ema,
                                      double dn, double &albedo, double &mult, double &base) {};
      virtual void NormModelAlgorithm(double pha, double inc, double ema,
                                      double deminc, double demema, double dn, double &albedo,
                                      double &mult, double &base);

    private:
      void SetNormPharef(const double pharef);
      void SetNormIncref(const double incref);
      void SetNormEmaref(const double emaref);
      void SetNormAlbedo(const double albedo);

      double p_normPharef;
      double p_normIncref;
      double p_normEmaref;
      double p_normAlbedo;

  };
};

#endif
