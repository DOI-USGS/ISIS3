#ifndef Topo_h
#define Topo_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "NormModel.h"

namespace Isis {
  class Pvl;

  /**
   * @brief Topographic derivative of an arbitrary photometric function
   *
   * @author 1999-01-08 Randy Kirk
   *
   * @internal
   *  @history 2007-08-15 Steven Lambright Refactored and fixed unit test
   *  @history 2008-06-18 Steven Lambright Fixed ifndef, removed endlink doxygen
   *           command
   *  @history 2010-11-10 Janet Barrett - Added reference parameters for
   *           phase and emission so user can specify normalization
   *           conditions in initialization
   *  @history 2010-11-30 Janet Barrett - Added the ability to use the photometric
   *           angles from the ellipsoid or the DEM
   *
   */
  class Topo : public NormModel {
    public:
      Topo(Pvl &pvl, PhotoModel &pmodel);
      virtual ~Topo() {};

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
      void SetNormThresh(const double thresh);
      void SetNormAlbedo(const double albedo);

      double p_normPharef;
      double p_normIncref;
      double p_normEmaref;
      double p_normThresh;
      double p_normAlbedo;

  };
};

#endif
