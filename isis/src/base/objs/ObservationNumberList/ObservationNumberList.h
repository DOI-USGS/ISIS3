#ifndef ObservationNumberList_h
#define ObservationNumberList_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <map>
#include <vector>

#include <QString>

#include "SerialNumberList.h"

namespace Isis {

  /**
   * @brief Create a list of observation numbers from a file or serial number list
   *
   * This class allows for creating observation numbers from a provided file or
   * and existing non-empty SerialNumberList. Internally, it will map the observation numbers
   * that are created to the corresponding serial number for a given observation. 
   *
   * @author 2007-09-17 Debbie A. Cook
   *
   * @internal
   *   @history 2007-09-17 Debbie A. Cook - Original version
   *   @history 2008-01-11 Christopher Austin - Made class more general, inheriting 
   *                           SerialNumberList among others
   *   @history 2008-05-01 Debbie A. Cook - Removed upper bound check in ObservationNumberMapIndex
   *                           because when entries are removed from the observation list, the 
   *                           serialNumberIndex may exceed the size of the map
   *   @history 2008-06-18 Steven Koechle - Fixed Documentation Errors
   *   @history 2008-10-30 Steven Lambright - Fixed problem with definition of struct quad, pointed
   *                           out by "novus0x2a" (Support Board Member)
   *   @history 2016-06-02 Ian Humphrey - Changed inherited protected members from p_ to m_.
   *                           References #3967.
   *   @history 2016-06-03 Ian Humphrey - Updated documentation and coding standards. Updated
   *                           unit test. Fixes #3990.
   */
  class ObservationNumberList : public SerialNumberList {
    public:
      ObservationNumberList(const QString &list, bool checkTarget = true);
      ObservationNumberList(SerialNumberList *snlist);
      ~ObservationNumberList();

      void add(int isn, const int observationIndex, QString observationNumber) ;
      int observationSize() const;

      int observationNumberMapIndex(const int serialNumberIndex);

      void remove(SerialNumberList *snlist);
      void remove(const QString &listfile);

      bool hasObservationNumber(const QString &on);

      QString observationNumber(const QString &filename);
      QString observationNumber(int index);
      std::vector<QString> possibleFileNames(const QString &on);

    private:
      /**
       * An observation consiting of a serial number index to the ObservationNumberList,
       * an observation number index to the ObservationNumberList, and the observation number.
       */  
      struct ObservationSet {
        int serialNumberIndex;
        int observationNumberIndex;
        QString observationNumber;
      };

      void init(SerialNumberList *snlist);

      std::multimap<int, int> m_indexMap;  //!< Maps serial number index to observation number index
      int m_numberObservations;       //!< Count of observations in the observation number list
      std::vector<ObservationSet> m_sets;  //!< List of observation sets
  };
};

#endif

