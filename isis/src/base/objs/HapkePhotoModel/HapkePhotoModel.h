#ifndef HapkePhotoModel_h
#define HapkePhotoModel_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.1 $                                                             
 * $Date: 2008/10/17 16:58:50 $                                                                 
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

#include <string>
#include "PhotoModel.h"

namespace Isis {
  class Pvl;

/**
 * @brief Hapke photometric model class. 
 *  
 * This class contains the shared data between all Hapke photometric models. 
 * These photometric models include Hapke Legendre, Hapke Legendre Smooth, 
 * Hapke Henyey Greenstein, and Hapke Henyey Greenstein Smooth. 
 *  
 * This class is not a photometric model in itself, it simply handles these 
 * parameters: 
 *   Wh
 *   Hh
 *   B0
 *   Theta
 *
 * @author 2008-10-17 Steven Lambright
 *
 */
  class HapkePhotoModel : public PhotoModel {
    public:
      HapkePhotoModel (Pvl &pvl);
      virtual ~HapkePhotoModel() {};

      void SetPhotoWh(const double wh);
      //! Return photometric Wh value
      inline double PhotoWh () const { return p_photoWh; };

      void SetPhotoHh(const double hh);
      //! Return photometric Hh value
      inline double PhotoHh () const { return p_photoHh; };

      void SetPhotoB0(const double b0);
      //! Return photometric B0 value
      inline double PhotoB0 () const { return p_photoB0; };

      void SetPhotoTheta(const double theta);
      //! Return photometric Theta value
      inline double PhotoTheta () const { return p_photoTheta; };

      void SetOldTheta(double theta) { p_photoThetaold = theta; }

      //! Hapke's approximation to Chandra's H function
      inline double Hfunc(double u, double gamma) {
        return (1.0 + 2.0 * u)/(1.0 + 2.0 * u * gamma);
      }

      void SetStandardConditions(bool standard);

    protected:
      double p_photoWh;
      double p_photoHh;
      double p_photoB0;
      double p_photoB0save;
      double p_photoTheta;
      double p_photoThetaold;
  };
};

#endif
