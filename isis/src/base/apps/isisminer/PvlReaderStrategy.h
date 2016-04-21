#ifndef PvlReaderStrategy_h
#define PvlReaderStrategy_h
/**
 * @file                                                                  
 * $Revision: 6187 $ 
 * $Date: 2015-05-11 17:31:51 -0700 (Mon, 11 May 2015) $ 
 * $Id: PvlReaderStrategy.h 6187 2015-05-12 00:31:51Z kbecker@GS.DOI.NET $
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

// PvlConstraints, SharedResource and ResourceList typedefs
#include "PvlFlatMap.h"
#include "Resource.h"

namespace Isis {

  class PvlObject;

  /**
   * @brief PvlReaderStrategy - provides inline calculations
   *  
   * Object = Strategy 
   *   Type = PvlReader
   *   Name = PvlReader
   *   FromList = mybiglist
   *  
   *   PvlFileRef = Keyword
   *   PvlFile = %1_%2.%3
   *   PvlFileArgs = (key1, key2, key3)
   * EndObject 
   *  
   * @author 2012-07-15 Kris Becker 
   * @internal 
   *   @history 2012-07-15 Kris Becker - Original version.
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-03-23 Ian Humphrey - Added documentation. Made Identity keyword optional.
   *                           Modified to allow use of KeyListFileArgs.
   *   @history 2015-05-08 Kris Becker - Modify constructor to take a global
   *                          resources list; modified apply() method to accept
   *                          a global resource list.
   */
  class PvlReaderStrategy : public Strategy {
  
    public:
      PvlReaderStrategy();
      PvlReaderStrategy(const PvlObject &definition, const ResourceList &globals);
      virtual ~PvlReaderStrategy();
  
      virtual int apply(ResourceList &resources, const ResourceList &globals);
  
    private:
      SharedResource pvlResource(const QString &pvlfile, 
                                 const ResourceList &globals, int nth) const;
  
      QString        m_pvlfile;  //!< The name of the provided FromList Pvl file
      QString        m_identity; //!< The value of the Identity keyword
      PvlConstraints m_pvlparms; //!< Constraints indicated by Includes, Excludes, or KeyListFile
  };

} // Namespace Isis

#endif
