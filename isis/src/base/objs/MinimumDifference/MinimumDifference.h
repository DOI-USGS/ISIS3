#if !defined(MinimumDifference_h)
#define MinimumDifference_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2009/06/16 16:12:40 $                                                                 
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

#include "AutoReg.h"

namespace Isis {
  class Pvl;
  class Chip;

  /**                                                                       
   * @brief Minimum difference pattern matching
   *                                                                        
   * This class is used to construct a minimum difference pattern matching
   * algorith.  That is, given a search chip and a pattern chip, the pattern
   * chip is walked through the search chip.  At each position the a sub-search
   * chip is extracted which is the same size as the pattern chip.  Then the
   * absolute value of the difference is computed at each matching pixel in the
   * pattern and sub-search chip.  These differences are then summed to produce
   * the goodness of fit.  The sub-search chip with the lowest goodness of fit
   * will be identified as the pattern match (if a tolerance is met).  The best
   * fit = 0 which means the pattern chip and sub-search chip are identical
   *                                                                        
   * @ingroup PatternMatching
   * 
   * @see MinimumDifference AutoReg
   *                                                                        
   * @author  2005-05-05 Jeff Anderson      
   * 
   * @internal
   *   @history 2006-01-11 Jacob Danton Added idealFit value, unitTest
   *   @history 2006-03-08 Jacob DAnton Added sampling options
   *   @history 2006-03-20 Jacob Danton Changed to *average* minimum
   *                                     difference algorithm.
   */                                                                       
  class MinimumDifference : public AutoReg {
    public:
      /**
       * @brief Construct a MinimumDifference search algorithm
       * 
       * This will construct a minimum difference search algorith.  It is
       * recommended that you use a AutoRegFactory class as opposed to
       * this constructor
       * 
       * @param pvl  A Pvl object that contains a valid automatic registration
       * definition
       */
      MinimumDifference (Pvl &pvl) : AutoReg(pvl) { };

      //! Destructor
      virtual ~MinimumDifference() {};

    protected:
      virtual double MatchAlgorithm (Chip &pattern, Chip &subsearch);
      virtual bool CompareFits(double fit1, double fit2);
      virtual double IdealFit() const { return 0.0;};
      virtual std::string AlgorithmName() const {return "MinimumDifference";};

  };
};

#endif
