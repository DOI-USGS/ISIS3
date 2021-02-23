#ifndef NoNormalization_h
#define NoNormalization_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "NormModel.h"

namespace Isis {
  class Pvl;

  /**
   * @brief NoNormalization - perform simple correction without normalization (a*dn +b)
   *
   * @author 2008-03-17 Janet Barrett
   *
   * @internal
   *   @history 2008-03-17 Janet Barrett - Original version
   *   @history 2008-06-18 Steven Koechle - Fixed Documentation Errors
   *   @history 2010-05-18 Janet Barrett - Modified class so that it does nothing to
   *                       the incoming dn value. The outgoing albedo value will be
   *                       the same as the incoming dn value.
   *   @history 2010-11-30 Janet Barrett - Added ability to use photometric angles
   *                       from the ellipsoid or the DEM
   *   @history 2011-01-28 Janet Barrett - Fixed NormModelAlgorithm so that it applies 
   *                       the photometric correction to the incoming dn value
   *
   */
  class NoNormalization : public NormModel {
    public:
      NoNormalization(Pvl &pvl, PhotoModel &pmodel);
      virtual ~NoNormalization() {};

    protected:
      virtual void NormModelAlgorithm(double pha, double inc, double ema,
                                      double dn, double &albedo, double &mult, double &base) {};
      virtual void NormModelAlgorithm(double pha, double inc, double ema,
                                      double deminc, double demema, double dn, double &albedo,
                                      double &mult, double &base);
  };
};

#endif
