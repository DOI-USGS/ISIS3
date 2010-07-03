#if !defined(Anisotropic1_h)
#define Anisotropic1_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.5 $                                                             
 * $Date: 2008/11/05 23:36:30 $                                                                 
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
 * 
 *
 * @ingroup RadiometricAndPhotometricCorrection
 * @author 1998-12-21 Randy Kirk
 *  
 * @internal
 *  @history 1998-12-21 Randy Kirk - USGS, Flagstaff - Original
 *          code.
 *  @history 2007-02-20 Janet Barrett - Imported from Isis2.
 *  @history 2007-08-15 Steven Lambright - Refactored code
 *  @history 2008-03-07 Janet Barrett - Moved code to set standard
 *                      conditions to the AtmosModel class
 *  @history 2008-06-18 Christopher Austin - Fixed documentation error
 *  @history 2008-11-05 Jeannie Walldren - Replaced reference to
 *           NumericalMethods::r8expint() with AtmosModel::En().
 *           Added documentation from Isis2.
 */
  class Anisotropic1 : public AtmosModel {
    public:
      Anisotropic1 (Pvl &pvl, PhotoModel &pmodel);
      virtual ~Anisotropic1() {};

    protected:
      virtual void AtmosModelAlgorithm (double phase, double incidence, 
            double emission);

    private:
      void SetAtmosHnorm(const double hnorm);

      double p_atmosE2;
      double p_atmosE3;
      double p_atmosE4;
      double p_atmosE5;
      double p_atmosDelta_0;
      double p_atmosDelta_1;
      double p_atmosAlpha0_0;
      double p_atmosAlpha1_0;
      double p_atmosBeta0_0;
      double p_atmosBeta1_0;
      double p_atmosWha2;
      double p_atmosWham;
      double p_atmosX0_0;
      double p_atmosY0_0;
      double p_atmosX0_1;
      double p_atmosY0_1;
      double p_atmosFac;
      double p_atmosDen;
      double p_atmosQ0;
      double p_atmosQ1;
      double p_atmosP0;
      double p_atmosP1;
      double p_atmosQ02p02;
      double p_atmosQ12p12;
      double p_atmosHnorm;
  };
};

#endif
