#ifndef NoOperator_h
#define NoOperator_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.1 $                                                             
 * $Date: 2008/12/12 21:55:56 $                                                                 
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

#include "InterestOperator.h"

namespace Isis {
  class Pvl;
  class Chip;

  /**
   * @brief no interest operator
   * 
   * This class is used to construct a no interest operator.
   * For this class, the interest returned is the constant pi * e
   * 
   * @see InterestOperator
   *
   * @author 2008-12-12 Christopher Austin
   * 
   * @internal
   *   @history 2008-12-12 Jacob Danton - Original Version
   */
  class NoOperator : public InterestOperator {
    public:
      NoOperator (Pvl &pvl) : InterestOperator(pvl) {p_worstInterest = 0.0;};
      virtual ~NoOperator() {};

    protected:
      virtual double Interest (Chip &chip);
  };
};

#endif
