#ifndef AlbedoAtm_h
#define AlbedoAtm_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.7 $                                                             
 * $Date: 2009/05/11 21:53:32 $                                                                 
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
 *          to NumericalMethods class.
 *  @history 2008-11-07 Jeannie Walldren - Fixed documentation.
 *  @history 2009-05-11 Janet Barrett - Fixed so that the NormModelAlgorithm
 *          supporting DEM input is the empty function. DEM input is not yet
 *          supported.
 *  
 */
  class AlbedoAtm : public NormModel {
    public:
      AlbedoAtm (Pvl &pvl, PhotoModel &pmodel, AtmosModel &amodel);
      //! Empty Destructor
      virtual ~AlbedoAtm() {};

    protected:
      virtual void NormModelAlgorithm (double pha, double inc, double ema,
          double dn, double &albedo, double &mult, double &base);
      virtual void NormModelAlgorithm (double pha, double inc, double ema, 
          double deminc, double demema, double dn, double &albedo,
	        double &mult, double &base) {};

    private:
      //! Set parameters needed for albedo normalization
      void SetNormIncref(const double incref);

      double p_normPsurfref;
      double p_normIncref;
      double p_normPstdref;
      double p_normAhref;
      double p_normMunotref;
      double p_normTransref;
      double p_normTrans0ref;
      double p_normSbar;
  };
};

#endif
