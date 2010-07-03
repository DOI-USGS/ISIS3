#if !defined(ID_h)
#define ID_h

/**
 *   @file
 *   $Revision: 1.1.1.1 $                                                             
 *   $Date: 2006/10/31 23:18:07 $      
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

namespace Isis{
  /**                                                   
   * @brief   Creates sequential IDs
   * 
   * This class generates IDs in numerical sequence, from an input
   * string. The input must contain one, and only one, series of
   * question marks, which will be replaced with numbers in the
   * generation of IDs. The default start value is 1, but this can
   * be changed.
   * 
   * @ingroup Utility
   *                                                    
   * @author 2006-07-05 Brendan George
   *                                                    
   * @internal
   */  
  class ID{
  public:
    ID(const std::string &name, int basenum=1);

    ~ID();

    std::string Next();

  private:
    std::string p_namebase;
    int p_current;
    int p_numLength;
    int p_numStart;
  };
}

#endif
