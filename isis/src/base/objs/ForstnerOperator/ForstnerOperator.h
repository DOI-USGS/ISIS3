#if !defined(ForstnerOperator_h)
#define ForstnerOperator_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2007/08/02 23:21:03 $                                                                 
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
   * @brief Forstner interest operator
   * 
   * This class is used to construct a forstner interest operator.
   * For this class, the interest is always positive with the worst
   * interest amount being 0. The higher the interest, the better.
   * 
   * @see InterestOperator
   * 
   * @see "A Fast Operator for Detection and Precise Location of
   *      Distinct Points, Corners and Centres of Circular
   *      Features" by W. Forstner and E. Gulch    (Forstner.pdf)
   *
   * @author 2006-05-01 Jacob Danton
   */
  class ForstnerOperator : public InterestOperator {
    public:
      ForstnerOperator (Pvl &pvl) : InterestOperator(pvl) {};
      virtual ~ForstnerOperator() {};

    protected:
      virtual double Interest (Chip &chip);
  };
};

#endif
