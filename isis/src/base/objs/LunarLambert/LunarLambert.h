#if !defined(LunarLambert_h)
#define LunarLambert_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.3 $                                                             
 * $Date: 2008/06/19 14:34:21 $                                                                 
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
 * @brief Lunar (Lommel-Seeliger)-Lambert law photometric model
 * Derive model albedo for Lunar (Lommel-Seeliger)-Lambert law.
 * Phase independent and calculated analytically.
 * Lommel-Seeliger law:
 *
 * Reflectance=1/(1+cos(E)/cos(I))
 *
 * Where: E=the angle between the observer and the slope normal
 *       I=the angle between the sun and the slope normal
 * 
 * @author 1999-01-08 Randy Kirk
 *
 * @internal
 *  @history 2007-07-31 Steven Lambright - Moved PhotoL from base PhotoModel class
 *                       to this child.
 */
  class LunarLambert : public PhotoModel {
    public:
      LunarLambert (Pvl &pvl);
      virtual ~LunarLambert() {};

      void SetPhotoL (const double l);

      //! Return photometric L value
      inline double PhotoL () const { return p_photoL; };

    private:
      double p_photoL;

    protected:
      virtual double PhotoModelAlgorithm (double phase, double incidence,
            double emission);

  };
};

#endif
