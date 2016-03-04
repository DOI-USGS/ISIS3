#ifndef LimitStrategy_h
#define LimitStrategy_h
/**
 * @file                                                                  
 * $Revision: 6187 $ 
 * $Date: 2015-05-11 17:31:51 -0700 (Mon, 11 May 2015) $ 
 * $Id: LimitStrategy.h 6187 2015-05-12 00:31:51Z kbecker@GS.DOI.NET $
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

// parent class
#include "CalculatorStrategy.h"

// SharedResource typedef
#include "Resource.h"

namespace Isis {

  class PvlObject;

  /**
   * @brief Check if a single Resource keyword value is within 
   *        limits, if not, the keyword is deactivated
   *  
   * This Strategy provides users with the ability to check if 
   * Resource keyword values are within numeric limits. If the 
   * keyword is within the limits, the keyword is kept. If a 
   * keyword is not within the limits set, the keyword is 
   * deactivated. 
   * 
   * @code
   * Object = Strategy 
   *   Type = Limit
   *   Name = EmissionAngle
   *   Equation = "((emissionangle > 30) && (emissionangle < 80))
   * EndObject 
   * @endcode 
   *  
   * @author 2012-07-15 Kris Becker 
   * @internal 
   *   @history 2012-07-15 Kris Becker - Original version.
   *   @history 2015-02-27 Makayla Shepherd - Updated documentation.
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-05-01 Kris Becker - Use default CalculatorStrategy
   *                          constructor rather than special constructor
   *   @history 2015-05-08 Kris Becker - Modify constructor to take a global
   *                          resources list; modified apply() method to accept
   *                          a global resource list.
   */
  class LimitStrategy : public CalculatorStrategy {
  
    public:
      LimitStrategy();
      LimitStrategy(const PvlObject &definition, const ResourceList &globals);
      virtual ~LimitStrategy();
  
      virtual int apply(SharedResource &resource, const ResourceList &globals);
  
  };

} // Namespace Isis

#endif
