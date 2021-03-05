#ifndef AlbedoAtm_h
#define AlbedoAtm_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "NormModel.h"

namespace Isis {
  class Pvl;

  /**
   * @brief Albedo normalization with atmosphere
   *
   * @ingroup RadiometricAndPhotometricCorrection
   * @author 1998-12-21 Randy Kirk
   *
   * @internal
   *  @history 2007-08-15 Steven Lambright - Refactored code and fixed unit test
   *  @history 2008-06-18 Christopher Austin - Fixed documentation errors
   *  @history 2008-11-05 Jeannie Walldren - Modified references
   *                         to NumericalMethods class.
   *  @history 2008-11-07 Jeannie Walldren - Fixed documentation.
   *  @history 2009-05-11 Janet Barrett - Fixed so that the NormModelAlgorithm
   *                          supporting DEM input is the empty function. DEM input is not yet
   *                          supported.
   *  @history 2010-11-10 Janet Barrett - Added reference parameters for
   *                          phase and emission so user can specify normalization
   *                          conditions in initialization
   *  @history 2010-11-30 Janet Barrett - Added ability to use photometric angles
   *                          from the ellipsoid and the DEM
   *  @history 2017-07-03 Makayla Shepherd - Updated documentation. References #4807.
   *
   */
  class AlbedoAtm : public NormModel {
    public:
      AlbedoAtm(Pvl &pvl, PhotoModel &pmodel, AtmosModel &amodel);
      //! Empty Destructor
      virtual ~AlbedoAtm() {};

    protected:
      /**
       * Performs the normalization.
       * 
       * @param pha The phase angle.
       * @param inc The incidence angle.
       * @param ema The emission angle.
       * @param dn The DN value
       * @param albedo ???
       * @param mult The multiplier of the image
       * @param base The base of the image
       */
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

      double p_normPsurfref;  //!< ???
      double p_normPharef;    //!< The reference phase angle
      double p_normIncref;    //!< The reference incidence angle
      double p_normEmaref;    //!< The reference emission angle
      double p_normPstdref;   //!< ???
      double p_normAhref;     //!< ???
      double p_normMunotref;  //!< ???
      double p_normTransref;  //!< ???
      double p_normTrans0ref; //!< ???
      double p_normTranss;    //!< ???
      double p_normSbar;      //!< ???
  };
};

#endif
