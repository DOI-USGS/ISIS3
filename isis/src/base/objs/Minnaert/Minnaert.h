#if !defined(Minnaert_h)
#define Minnaert_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.3 $                                                             
 * $Date: 2008/06/19 15:18:26 $                                                                 
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

#include "PhotoModel.h"

namespace Isis {
  class Pvl;

/**
 * @brief Minnaert photometric model
 *  Derive model albedo using Minnaert equation.
 *  Phase independent and calculated analytically.
 *  Limb-darkening k is a constant.
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
 * @author 1989-08-02 Tammy Becker
 *
 * @internal
 *  @history 2007-07-31 Steven Lambright - Moved PhotoK from base PhotoModel class to this
 *                                           child. 
 */
  class Minnaert : public PhotoModel {
    public:
      Minnaert (Pvl &pvl);
      virtual ~Minnaert() {};
      
      void SetPhotoK(const double k);
      //! Return photometric K value
      inline double PhotoK () const { return p_photoK; };

    private:
      double p_photoK;

    protected:
      virtual double PhotoModelAlgorithm (double phase, double incidence,
            double emission);

  };
};

#endif
