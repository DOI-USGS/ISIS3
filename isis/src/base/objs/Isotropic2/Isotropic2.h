#ifndef Isotropic2_h
#define Isotropic2_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.5 $                                                             
 * $Date: 2008/11/05 23:37:17 $                                                                 
 *                                                                        
 *   Unless noted otherwise, the portions of Isis written by the USGS are 
 *   public domain. See individual third-party library and package descriptions 
 *   for intellectual property information, user agreements, and related  
 *   information.                                                         
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or   
 *   implied, is made by the USGS as to the accuracy and functioning of such 
 *   software and related material nor shall the fact of distribution     
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.                                        
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html                
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */                                                                       

#include "AtmosModel.h"

namespace Isis {
  class Pvl;

/**
 * @brief
 * @ingroup RadiometricAndPhotometricCorrection
 * @author 1998-12-21 Randy Kirk
 *  
 * @internal
 *  @history 1998-12-21 Randy Kirk - USGS, Flagstaff - Original
 *          code
 *  @history 2007-02-20 Janet Barrett - Imported from Isis2.
 *  @history 2007-08-15 Steven Lambright - Refactored code
 *  @history 2008-03-07 Janet Barrett - Moved code to set standard
 *                      conditions to the AtmosModel class
 *  @history 2008-06-18 Stuart Sides - Fixed doc error
 *  @history 2008-11-05 Jeannie Walldren - Replaced reference to
 *          NumericalMethods::r8expint() with AtmosModel::En(),
 *          NumericalMethods::G11Prime() with
 *          AtmosModel::G11Prime(), and NumericalMethods::r8ei()
 *            with AtmosModel::Ei().  Added documentation from
 *            Isis2.
 */
  class Isotropic2 : public AtmosModel {
    public:
      Isotropic2 (Pvl &pvl, PhotoModel &pmodel);
      virtual ~Isotropic2() {};

      //! Return atmospheric Hnorm value
      double AtmosHnorm () const { return p_atmosHnorm; };

    protected:
      virtual void AtmosModelAlgorithm (double phase, double incidence, double emission);

    private:
      void SetAtmosHnorm(const double hnorm);

      double p_atmosHnorm;
      double p_delta;
      double p_fixcon;
      double p_gammax,p_gammay;
      double p_wha2;
      double p_e1,p_e1_2,p_e2,p_e3,p_e4,p_e5;
      double p_em;
      double p_f1m,p_f2m,p_f3m,p_f4m;
      double p_g12,p_g13,p_g14;
      double p_e;
      double p_f1,p_f2,p_f3,p_f4;
      double p_g11p,p_g12p,p_g13p,p_g14p;
      double p_x0,p_y0;
      double p_alpha0,p_alpha1,p_alpha2;
      double p_beta0,p_beta1,p_beta2;
  };
};

#endif
