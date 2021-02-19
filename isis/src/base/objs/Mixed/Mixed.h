#ifndef Mixed_h
#define Mixed_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "NormModel.h"

namespace Isis {
  class Pvl;

  /**
   * @brief Mixed albedo/topo normalization without atmosphere
   *
   * This mode will do albedo normalization over most of the planet
   * but near the terminator it will normalize topographic contrast to
   * avoid the "seams" we are currently getting with the plain albedo
   * normalization.  The two effects will be joined seamlessly.
   * In addition to the parameters for no-atmosphere albedo normaliza-
   * tion (i.e., the photometric parameters and the choice of angles
   * for normal albedo calculation) this mode needs two more parameters.
   * INCMAT is the incidence angle at which the RMS contrast from al-
   * bedo matches the RMS contrast from topography.  (Could input a
   * full 3-angle geometry at which the contrasts are equal but since
   * the user is probably going to find this parameter by trial and er-
   * ror it's easier to specify only incidence and use emission=0,
   * phase=incidence for this second reference state.) ALBEDO, the av-
   * erage normal albedo, is also needed.
   *
   * @author 1998-12-21 Randy Kirk
   *
   * @internal
   *  @history 2007-07-31 Steven Lambright - Refactored code
   *  @history 2008-03-07 Janet Barrett - Changed name of Incmatch variable
   *                      to Incmat
   *  @history 2010-11-10 Janet Barrett - Added reference parameters for
   *                      phase and emission so user can specify normalization
   *                      conditions in initialization - also added match 
   *                      parameters for phase and emission for the same reason
   *  @history 2010-11-30 Janet Barrett - Added ability to use photometric angles
   *                      from the ellipsoid or the DEM
   *
   */
  class Mixed : public NormModel {
    public:
      Mixed(Pvl &pvl, PhotoModel &pmodel);
      virtual ~Mixed() {};

    protected:
      virtual void NormModelAlgorithm(double pha, double inc, double ema,
                                      double dn, double &albedo, double &mult, double &base) {};
      virtual void NormModelAlgorithm(double pha, double inc, double ema,
                                      double deminc, double demema, double dn, double &albedo,
                                      double &mult, double &base);

    private:
      //! Set parameters needed for albedo normalization
      void SetNormPharef(const double pharef);
      void SetNormIncref(const double incref);
      void SetNormEmaref(const double emaref);
      void SetNormPhamat(const double phamat);
      void SetNormIncmat(const double incmat);
      void SetNormEmamat(const double emamat);
      void SetNormThresh(const double thresh);
      void SetNormAlbedo(const double albedo);

      double p_psurfmatch;
      double p_pprimematch;
      double p_anum;
      double p_rhobar;
      double p_psurfref;
      double p_normPharef;
      double p_normIncref;
      double p_normEmaref;
      double p_normThresh;
      double p_normPhamat;
      double p_normIncmat;
      double p_normEmamat;
      double p_normAlbedo;
  };
};

#endif
