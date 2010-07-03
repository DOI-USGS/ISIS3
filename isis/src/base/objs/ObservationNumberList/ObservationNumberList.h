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

#include <string>
#include <map>
#include <vector>

#include "SerialNumberList.h"

namespace Isis {

  /**
   * @brief Needs Documentation
   * 
   * Needs Documentation
   * 
   * @author 2007-09-17 Debbie A. Cook
   * 
   * @internal
   *   @history 2007-09-17 Debbie A. Cook - Original version
   *   @history 2008-01-11 Christopher Austin - Made class more
   *            general, inheriting SerialNumberList among others
   *   @history 2008-05-01 Debbie A. Cook - Removed upper bound check
   *            in ObservationNumberMapIndex because when entries are
   *            removed from the observation list, the serialNumberIndex
   *            may exceed the size of the map
   *   @history 2008-06-18 Steven Koechle - Fixed Documentation Errors
   *   @history 2008-10-30 Steven Lambright - Fixed problem with definition
   *            of struct quad, pointed out by "novus0x2a" (Support Board Member)
   */
  class ObservationNumberList : public Isis::SerialNumberList {
    public:
      ObservationNumberList (const std::string &list, bool checkTarget=true);
      ObservationNumberList(Isis::SerialNumberList *snlist);
      ~ObservationNumberList();

      void Add (int isn, const int observationIndex,std::string observationNumber) ;
      int ObservationSize () const;

      int ObservationNumberMapIndex(const int serialNumberIndex );

      void Remove ( Isis::SerialNumberList *snlist );
      void Remove (const std::string &listfile);

      bool HasObservationNumber (const std::string &on);

      std::string ObservationNumber(const std::string &filename);
      std::string ObservationNumber (int index);
      std::vector<std::string> PossibleFilenames (const std::string &on);

    private:
      struct ObservationSet {
        int serialNumberIndex;
        int observationNumberIndex;
        std::string observationNumber;
      };

      void init ( Isis::SerialNumberList *snlist );

      std::vector<ObservationSet> p_sets;
      std::multimap<int,int> p_indexMap;
      int p_numberObservations;
  };
};

#endif

