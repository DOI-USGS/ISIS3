#ifndef ObservationNumberList_h
#define ObservationNumberList_h
/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2008/10/30 16:19:46 $
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
 *
 */

#include <map>
#include <vector>

#include <QString>

#include "SerialNumberList.h"

namespace Isis {

  /**
   * @brief Create a list of observation numbers from a file or serial number list
   *
   * @description This class allows for creating observation numbers from a provided file or
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

