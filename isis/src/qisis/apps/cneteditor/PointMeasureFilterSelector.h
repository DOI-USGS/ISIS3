#ifndef PointMeasureFilterSelector_H
#define PointMeasureFilterSelector_H


#include "AbstractFilterSelector.h"


namespace Isis
{
  class AbstractFilter;

  class PointMeasureFilterSelector : public AbstractFilterSelector
  {
      Q_OBJECT

    public:
      PointMeasureFilterSelector();
      PointMeasureFilterSelector(const PointMeasureFilterSelector & other);
      virtual ~PointMeasureFilterSelector();
      PointMeasureFilterSelector & operator=(
        const PointMeasureFilterSelector & other);


    protected:
      void createSelector();


    protected: // slots (already marked as slots inside parent)
      void changeFilter(int);
  };
}

#endif
