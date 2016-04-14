#ifndef PdsColumn_h
#define PdsColumn_h
/**
 * @file                                                                  
 * $Revision: 6172 $ 
 * $Date: 2015-05-06 15:52:18 -0700 (Wed, 06 May 2015) $ 
 * $Id: PdsColumn.h 6172 2015-05-06 22:52:18Z kbecker@GS.DOI.NET $
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

// parent class - includes ResourceList, SharedResource typedefs
#include "Resource.h"

class QString;

namespace Isis {

  class PvlContainer; 
  class PvlObject; 

  /**
   * @brief This class defines the format of a PDS column. 
   *  
   * It inherits from the Resource class and is defined by the following 
   * required and optional keywords: 
   *  
   * Required: 
   * @code 
   * <ul> 
   *   <li>COLUMN_NUMBER</li>
   *   <li>NAME</li>
   *   <li>DATA_TYPE</li>
   *   <li>START_BYTE</li>
   *   <li>BYTES</li>
   *   <li>DESCRIPTION</li>
   * </ul> 
   * @endcode 
   *  
   * Optional: 
   * @code 
   * <ul> 
   *   <li>UNIT</li>
   *   <li>FORMAT</li>
   * </ul>
   * @endcode 
   *   
   * Each entry of data for a PDS column is stored as keyword values in a 
   * Resource object. The name of the keyword that contains the PDS column data 
   * will match the name of the PdsColumn resource. 
   *  
   *  
   * @author 2014-11-28 Kris Becker 
   * @internal 
   *   @history 2014-11-28 Kris Becker - Original version.
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-03-31 Jeannie Backer - Updated documentation.
   */
  class PdsColumn : public Resource {
    public:
      PdsColumn();
      PdsColumn(const QString &name);
      PdsColumn(const PvlContainer &column);
      PdsColumn(const Resource &resource);
      virtual ~PdsColumn();
  
      bool isValid() const;
      int   size() const;
  
      void setBytes(const int &bytes);
      int bytes() const;
      int bytes(ResourceList &resources) const;
  
      QString dataType() const;
  
      int isQuoted() const;
      void setStartByte(const int &sbyte);
      int startByte() const;
      int endByte() const;
  
      void setFormat(const QString &format);
      QString formattedValue(SharedResource &resource,
                               const QString &defstring = "NULL") const;
  
      static PdsColumn *promote(SharedResource &resource);
//      virtual Resource *clone(const QString &name, 
//                              const bool &withAssets=false) const;
  
      virtual PvlObject toPvl(const QString &object = "COLUMN") const;
  
    private:
      QString format() const;
      int formatSize(const QString &fmt) const;
  
  };

} // Namespace Isis

#endif
