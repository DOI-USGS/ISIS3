#ifndef CnetReaderStrategy_h
#define CnetReaderStrategy_h
/**
 * @file                                                                  
 * $Revision: 6187 $ 
 * $Date: 2015-05-11 17:31:51 -0700 (Mon, 11 May 2015) $ 
 * $Id: CnetReaderStrategy.h 6187 2015-05-12 00:31:51Z kbecker@GS.DOI.NET $
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
#include "Strategy.h"

// Qt library
#include <QString>

// ResourceList, SharedResource typedefs
#include "Resource.h"

namespace Isis {

  class PvlFlatMap;
  class PvlContainer;
  class PvlObject;

  /**
   * @brief CnetReaderStrategy - Creates Resources from an ISIS control network each ControlMeasure
   *
   * This strategy provides users with the ability to read in an ISIS control network. A Resource is
   * created from each ControlMeasure in the control network. 
   * 
   * Here is an example of a CnetReaderStrategy object defnition:
   * @code
   * Object = Strategy 
   *   Type = CnetReader
   *   Name = CnetReader
   *   CnetFile = mycnetfile
   *  
   *   Identity = %1_%2
   *   IdentityArgs = (PointId, SerialNumber)
   * EndObject 
   * @endcode
   *  
   * @author 2014-12-25 Kris Becker
   * @internal 
   *   @history 2014-12-25 Kris Becker - Original version.
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-03-10 Ian Humphrey - Updated documentation. Identity key is optional.
   *   @history 2015-05-08 Kris Becker - Modify constructor to take a global
   *                          resources list; modified apply() method to accept
   *                          a global resource list.
   */
  class CnetReaderStrategy : public Strategy {
  
    public:
      CnetReaderStrategy();
      CnetReaderStrategy(const PvlObject &definition, const ResourceList &globals);
      virtual ~CnetReaderStrategy(); 
  
      virtual int apply(ResourceList &resources, const ResourceList &globals);
  
    private:
      ResourceList cnetResource(const ResourceList &globals,
                                const int &pointNum = 0) const;
      PvlFlatMap loadkeys(const PvlContainer &keys) const;
  };

} // Namespace Isis

#endif
