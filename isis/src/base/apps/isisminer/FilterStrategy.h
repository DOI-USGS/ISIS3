#ifndef FilterStrategy_h
#define FilterStrategy_h
/**
 * @file                                                                  
 * $Revision: 6513 $ 
 * $Date: 2016-01-14 16:04:44 -0700 (Thu, 14 Jan 2016) $ 
 * $Id: FilterStrategy.h 6513 2016-01-14 23:04:44Z kbecker@GS.DOI.NET $
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
#include <QRegExp>
#include <QString>
#include <QStringList>

// ResourceList and SharedResource typedefs
#include "Resource.h"

namespace Isis {

  class PvlObject;

  /**
   * @brief FilterStrategy activates/deactivates Resources with/wothout text 
   *        values
   *  
   * This strategy provides users with the ability to activate (Include) or 
   * deactivate (Exclude) Resources that have or don't have keywords with certain 
   * values. 
   *  
   * The Filter Strategy definition will be read for Include and Exclude values 
   * for the named Resource Keyword. 
   *  
   * Here is an example of a Filter Strategy object definition: 
   *  
   * @code 
   * Object = Strategy 
   *   Name = Filter
   *   Keyword = ObservationType
   *   Include = ("Monochrome", "Color")
   * EndObject
   * @endcode
   *  
   * @author 2013-02-19 Kris Becker 
   * @internal 
   *   @history 2013-02-19 Kris Becker - Original version.
   *   @history 2015-02-23 Kris Becker - Updated documentation
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-05-08 Kris Becker - Modify constructor to take a global
   *                          resources list; modified apply() method to accept
   *                          a global resource list.
   *   @history 2015-09-28 Kris Becker - Added test using a regular expression
   */
  class FilterStrategy : public Strategy {
  
    public:
      FilterStrategy();
      FilterStrategy(const PvlObject &definition, const ResourceList &globals);
      virtual ~FilterStrategy();
  
      virtual int apply(SharedResource &resource, const ResourceList &globals);
  
    private:
      QString     m_key;      //!< 
      bool        m_checkAll; //!< 
      QStringList m_includes; //!< 
      QStringList m_excludes; //!< 
      QRegExp     m_regexp;   //!< Regular expression
  
  };

} // Namespace Isis

#endif
