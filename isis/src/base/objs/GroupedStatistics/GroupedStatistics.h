#ifndef GroupedStatistics_h
#define GroupedStatistics_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// Global forward declarations
template< class A, class B > class QMap;
template< class A > class QVector;
class QString;

namespace Isis {

  // Isis forward declarations
  class Statistics;

  /**
   * @brief Grouped Statistics
   *
   * This class is used to store statistics on a group of related items.
   *
   * This class is include safe meaning that includers of this class will only
   * get this class.
   *
   * @ingroup Statistics
   *
   * @author 2009-09-14 Eric Hyer
   *
   * @see Statistics
   *
   * @internal
   *  @history 2009-09-14 Eric Hyer - Original Version
   *  @history 2009-09-18 Eric Hyer - Fixed some comments / documentation
   *  @history 2009-10-15 Eric Hyer - Added GetStatisticTypes method
   *
   */
  class GroupedStatistics {
    public:
      GroupedStatistics();
      GroupedStatistics(const GroupedStatistics &other);
      ~GroupedStatistics();

      void AddStatistic(const QString &statType, const double &newStat);
      const Statistics &GetStatistics(const QString &statType) const;
      const QVector< QString > GetStatisticTypes() const;

      GroupedStatistics &operator=(const GroupedStatistics &other);

    private:
      //! Map from statistic type to Statistics object
      QMap< QString, Statistics > * groupedStats;
  };
};

#endif

