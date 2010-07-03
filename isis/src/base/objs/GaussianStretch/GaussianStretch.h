/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2008/09/09 17:07:44 $                                                                 
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
#ifndef GaussianStretch_h
#define GaussianStretch_h

#include "Statistics.h"
#include "Histogram.h"
#include "Stretch.h"
#include "iException.h"
#include "Constants.h"

namespace Isis {
/**                                                                       
 * @brief Gaussian stretch class
 *                                                                        
 * This class is used to stretch the input histogram to a
 * gaussian distribution with the specified mean and standard
 * deviation.
 *                                                                        
 * @ingroup Utility
 *                                                                        
 * @author 2006-05-25 Jacob Danton
 *                                                                        
 * @internal                                                              
 *   @history 2006-05-25 Jacob Danton Original Version
 *   @history 2006-10-28 Stuart Sides Fixed stretch pair ordering
 *   @history 2008-09-09 Steven Lambright Fixed stretch pair ordering again;
 *            this fix does not solve our problem but makes our tests work and
 *            isn't wrong.
 */                                                                       
  class GaussianStretch : public Isis::Statistics {
  public:
    GaussianStretch (Histogram &histogram, const double mean = 0.0, const double standardDeviation = 1.0) ;
    ~GaussianStretch () {};

    double Map (const double value) const;
  private:
    //! Value of the mean
    Stretch p_stretch;
  }; 
};

#endif
