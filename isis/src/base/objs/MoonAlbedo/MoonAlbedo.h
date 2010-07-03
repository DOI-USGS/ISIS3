#if !defined(MoonAlbedo_h)
#define MoonAlbedo_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2008/06/19 15:25:40 $                                                                 
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
 * @brief Albedo dependent phase function normalization for the Moon
 * 
 * @author 1998-12-21 Randy Kirk
 *
 * @internal
 */
  class MoonAlbedo : public NormModel {
    public:
      MoonAlbedo (Pvl &pvl, PhotoModel &pmodel);
      virtual ~MoonAlbedo() {};

    protected:
      virtual void NormModelAlgorithm (double pha, double inc, double ema,
          double dn, double &albedo, double &mult, double &base);
      virtual void NormModelAlgorithm (double pha, double inc, double ema, 
          double deminc, double demema, double dn, double &albedo,
	  double &mult, double &base) {};

    private:
      //! Set parameters needed for albedo dependent phase function
      //! normalization for the Moon
      void SetNormD(const double d);
      void SetNormE(const double e);
      void SetNormF(const double f);
      void SetNormG2(const double g2);
      void SetNormXmul(const double xmul);
      void SetNormWl(const double wl);
      void SetNormH(const double h);
      void SetNormBsh1(const double bsh1);
      void SetNormXb1(const double xb1);
      void SetNormXb2(const double xb2);

      double p_normD;
      double p_normE;
      double p_normF;
      double p_normG2;
      double p_normXmul;
      double p_normWl;
      double p_normH;
      double p_normBsh1;
      double p_normXb1;
      double p_normXb2;
      double p_normF1;
      double p_normG2sq;
      double p_normPg30;
      double p_normBc1;
      double p_normFbc3;
      double p_normC3;
      double p_normPg32;
      double p_normBshad3;
  };
};

#endif
