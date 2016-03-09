#ifndef NumericalSortStrategy_h
#define NumericalSortStrategy_h
/**
 * @file                                                                  
 * $Revision: 6187 $ 
 * $Date: 2015-05-11 17:31:51 -0700 (Mon, 11 May 2015) $ 
 * $Id: NumericalSortStrategy.h 6187 2015-05-12 00:31:51Z kbecker@GS.DOI.NET $
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

// SharedResource and ResourceList typedefs
#include "Resource.h"

namespace Isis {

  class PvlObject;

  /**
   * @brief NumericalSortStrategy - Sorts Resources using a numerically-valued keyword
   *  
   * This strategy provides users with the ability to sort a list of Resources according to
   * a numerically valued-keyword. The user can sort the Resources in either ascending or
   * descending order.
   * 
   * Here is an example of a NumericalSortStrategy object definition:
   * @code
   * Object = Strategy 
   *   Name = RankSort
   *   Type = NumericalSort
   *   SortKey = Rank
   *   Order = Ascending
   * EndObject
   * @endcode
   * 
   * @author 2012-07-25 Kris Becker
   * @internal
   *   @history 2012-07-25 Kris Becker - Original version.
   *   @history 2015-03-03 Ian Humphrey - Updated documentation.
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-03-19 Ian Humphrey - New test to test thrown exception.
   *   @history 2015-05-08 Kris Becker - Modify constructor to take a global
   *                          resources list; modified apply() method to accept
   *                          a global resource list.
   */
  class NumericalSortStrategy : public Strategy {
  
    public:
      NumericalSortStrategy();
      NumericalSortStrategy(const PvlObject &definition, const ResourceList &globals);
      virtual ~NumericalSortStrategy();
  
      virtual int apply(ResourceList &resources, const ResourceList &globals);
  
    private:
      QString m_sortKey; //!< 
      QString m_order;   //!< 
  
  };


  /**
   * @brief Ascending order sort functor
   * 
   * Determines the ascending order of two Resources by comparing the numerical values of the
   * SortKey keyword and using the less than operator.
   * 
   * @author 2012-07-25 Kris Becker
   * @internal
   *   @history 2012-07-25 Kris Becker - Original version.
   */
  class SortAscending {
    public:
      SortAscending(const QString &sortKey);
      ~SortAscending();
      bool operator()(const SharedResource &a, const SharedResource &b) const;
      
    private:
      QString m_sortKey; //!< 
  };
  
  
  /**
   * @brief Descending order sort functor
   * 
   * Determines the descending order of two Resources by comparing the numerical values of the
   * SortKey keyword and using the greater than operator.
   * 
   * @author 2012-07-25 Kris Becker
   * @internal
   *   @history 2012-07-25 Kris Becker - Original version.
   */
  class SortDescending {
    public:
      SortDescending(const QString &sortKey);
      ~SortDescending();
      bool operator()(const SharedResource &a, const SharedResource &b) const;
      
    private:
      QString m_sortKey; //!< 
  };

} // Namespace Isis

#endif
