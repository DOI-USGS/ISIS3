#ifndef PointMeasureFilterSelector_H
#define PointMeasureFilterSelector_H


#include "AbstractFilterSelector.h"


namespace Isis {
  class AbstractFilter;

  /**
   * @brief Allows users to choose filters for filtering points and measures
   *
   * This class is responsible for creating a list of filters that can be
   * selected for filtering points and measures.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal 
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054. 
   */
  class PointMeasureFilterSelector : public AbstractFilterSelector {
      Q_OBJECT

    public:
      PointMeasureFilterSelector();
      PointMeasureFilterSelector(const PointMeasureFilterSelector &other);
      virtual ~PointMeasureFilterSelector();
      PointMeasureFilterSelector &operator=(
        const PointMeasureFilterSelector &other);


    protected:
      void createSelector();


    protected: // slots (already marked as slots inside parent)
      void changeFilter(int);
  };
}

#endif
