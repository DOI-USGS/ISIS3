#ifndef HiEqualization_h
#define HiEqualization_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Equalization.h"

#include <string>
#include <vector>

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
       *   @history 2012-11-30 Tracie Sucharski - Removed unused include file Projection.h.  This
       *                           change was made in reference to #775
       *   @histroy 2016-07-15 Ian Humphrey - Modified calculateStatisics() to set the solved state
       *                           to true to reflect changes to Equalization. References #2282.
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
      HiEqualization(QString fromListName);
      virtual ~HiEqualization();

      void calculateStatistics();

    protected:
      virtual void fillOutList(FileList &outList, QString toListName);
      virtual void errorCheck(QString fromListName);

    private:
      int getCCDType(int ccd);

      std::vector<int> movedIndices;
  };
};

#endif
