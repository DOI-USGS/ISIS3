#ifndef PhotoModel_h
#define PhotoModel_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.8 $                                                             
 * $Date: 2008/11/05 23:38:02 $                                                                 
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
#include "Pvl.h"

namespace Isis {
/**
 * @brief
 *
 * @ingroup RadiometricAndPhotometricCorrection
 * @author 1998-12-21 Randy Kirk
 *  
 * @internal
 *  @history 1998-12-21 Randy Kirk - USGS, Flagstaff - Original
 *          code
 *  @history 2007-02-20 Janet Barrett - Imported from Isis2.
 *  @history 2007-07-31 Steven Lambright - Moved children methods out of this
 *                      class and into the children classes
 *  @history 2008-03-07 Janet Barrett - Moved variables and related
 *                      methods that pertain to Hapke specific parameters
 *                      to this class from the HapkeHen class. Also added
 *                      the code to set standard conditions.
 *  @history 2008-06-18 Steven Koechle - Fixed Documentation Errors
 *  @history 2008-07-09 Steven Lambright - Fixed unit test 
 *  @history 2008-10-17 Steven Lambright - Moved Hapke-specific methods out of
 *                      this class and into children classes.
 *  @history 2008-11-05 Jeannie Walldren - Moved PhtAcos() from
 *                      NumericalMethods class.
 */
  class PhotoModel {
    public:
      PhotoModel (Pvl &pvl);
      virtual ~PhotoModel() {};

      //! Return algorithm name found in Pvl file from constructor
      inline std::string AlgorithmName () const { return p_photoAlgorithmName; };

      virtual void SetStandardConditions(bool standard);
      //! Returns true if standard conditions are used, i.e., if SetStandardConditions(true) has been called.  This is initialized to false in the constructor.
      bool StandardConditions() const { return p_standardConditions; }

      // Obtain topographic derivative 
      double PhtTopder(double phase, double incidence, double emission);

      // Obtain arccosine 
      static double PhtAcos(double cosang);

      // Calculate the surface brightness
      double CalcSurfAlbedo(double pha, double inc, double ema);

    protected:
      virtual double PhotoModelAlgorithm (double phase, 
            double incidence, double emission) = 0;

    private:
      //! Unique name of the photometric model
      std::string p_photoAlgorithmName; 
      //! Indicates whether standard conditions are used
      bool p_standardConditions;
  };
};

#endif
