#ifndef HiEqualization_h
#define HiEqualization_h

/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2009/11/25 22:09:21 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "Equalization.h"

// TODO Qt instead
#include <string>
#include <vector>

// TODO Don't include
#include "FileList.h"


namespace Isis {
  class Buffer;
  class Cube;
  class FileList;
  class OverlapStatistics;
  class PvlGroup;
  class Statistics;


  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class HiEqualization : public Equalization {
    protected:
      /**
       * @author ????-??-?? Unknown
       *
       * @internal
       */
      class HiCalculateFunctor : public CalculateFunctor {
        public:
          HiCalculateFunctor(Statistics *stats, Statistics *statsLeft,
              Statistics *statsRight,
              double percent) : CalculateFunctor(stats, percent) {
            m_statsLeft = statsLeft;
            m_statsRight = statsRight;
          }

          virtual ~HiCalculateFunctor() {}

        protected:
          virtual void addStats(Buffer &in) const;

        private:
          Statistics *m_statsLeft;
          Statistics *m_statsRight;
      };

    public:
      HiEqualization(std::string fromListName);
      virtual ~HiEqualization();

      void calculateStatistics();

    protected:
      virtual void fillOutList(FileList &outList, std::string toListName);
      virtual void errorCheck(std::string fromListName);

    private:
      int getCCDType(int ccd);

      std::vector<int> movedIndices;
  };
};

#endif
