#ifndef Photometry_h
#define Photometry_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.5 $                                                             
 * $Date: 2008/07/09 19:40:52 $                                                                 
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
#include <vector>

namespace Isis {
  class Pvl;
  class PhotoModel;
  class AtmosModel;
  class NormModel;
/**
 * @internal
 *  @history 2007-08-02 Steven Lambright - Fixed memory leak
 *  @history 2008-03-07 Janet Barrett - Added SetPhotomWl method to allow
 *                      the application to set the p_normWavelength variable for
 *                      use by MoonAlbedo normalization.
 *  @history 2008-06-18 Steven Koechle - Fixed Documentation Errors
 *  @history 2008-07-09 Steven Lambright - Fixed unit test
 */
  class Photometry {
    public:
      Photometry (Pvl &pvl);
      virtual ~Photometry();

      //! Calculate the surface brightness
      void Compute(double pha, double inc, double ema, double dn,
                   double &albedo, double &mult, double &base);
      void Compute(double pha, double inc, double ema, double deminc,
                     double demema, double dn, double &albedo,
		     double &mult, double &base);

      //! Set the wavelength 
      virtual void SetPhotomWl(double wl);

    protected:
      AtmosModel *p_phtAmodel;
      PhotoModel *p_phtPmodel;
      NormModel *p_phtNmodel;
  };
};

#endif
