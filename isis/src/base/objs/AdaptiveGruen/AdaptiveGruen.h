#if !defined(AdaptiveGruen_h)
#define AdaptiveGruen_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.1 $                                                             
 * $Date: 2009/09/09 23:42:41 $                                                                 
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

#include "Gruen.h"

namespace Isis {
  class Pvl;

  /**                                                                       
   * @brief Gruen (adaptive) pattern matching 
   *  
   * The AdaptiveGruen pattern/search chip registration algorithm is derived 
   * from the Gruen class.  It is adaptive in that it uses an Affine transform 
   * to load the subsearch chip from the search chip.  The Affine transform is 
   * iteratively minimized to converge on an cummulative affine solution that 
   * best matches the pattern chip.
   *                                                                        
   * @ingroup PatternMatching
   * 
   * @see Gruen AutoReg MinimumGruen
   *                                                                        
   * @author  2009-09-09 Kris Becker
   * 
   * @internal
   */                                                                       
  class AdaptiveGruen : public Gruen {
    public:
      /**
       * @brief Construct a AdaptiveGruen search algorithm
       * 
       * This will construct an AdaptiveGruen search algorithm.  It is 
       * recommended that you use a AutoRegFactory class as opposed to this 
       * constructor 
       * 
       * @param pvl  A Pvl object that contains a valid automatic registration
       * definition
       */
      AdaptiveGruen (Pvl &pvl) : Gruen(pvl) { }

      /** Destructor for AdaptiveGruen */
      virtual ~AdaptiveGruen() {}

      /**
       * AdaptiveGruen is adaptive
       */ 
      virtual bool IsAdaptive() { return (true); }

    protected:
      /** Return name of Algorithm */
      virtual std::string AlgorithmName() const {return ("AdaptiveGruen");}


  };
};

#endif
