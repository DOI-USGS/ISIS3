#if !defined(MaximumCorrelation_h)
#define MaximumCorrelation_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2009/06/16 16:12:10 $                                                                 
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
   * @brief Maximum correlation pattern matching
   *                                                                        
   * This class is used to construct a maximum correlation pattern matching
   * algorith.  That is, given a search chip and a pattern chip, the pattern
   * chip is walked through the search chip.  At each position the a sub-search
   * chip is extracted which is the same size as the pattern chip.  Then the
   * correlation between the two is computed.  The best fit = 1.0 which means
   * the pattern chip and sub-search chip are identical
   * 
   * @ingroup PatternMatching
   *
   * @see MinimumDifference AutoReg
   *
   * @internal
   *   @history 2006-01-11 Jacob Danton Added idealFit value, unitTest
   *   @history 2006-03-08 Jacob Danton Added sampling options
   */
  class MaximumCorrelation : public AutoReg {
    public:
      MaximumCorrelation (Pvl &pvl) : AutoReg(pvl) { };
      virtual ~MaximumCorrelation() {};

    protected:
      virtual double MatchAlgorithm (Chip &pattern, Chip &subsearch);
      virtual bool CompareFits(double fit1, double fit2);
      virtual double IdealFit() const { return 1.0;};
      virtual std::string AlgorithmName() const {return "MaximumCorrelation";};

  };
};

#endif
