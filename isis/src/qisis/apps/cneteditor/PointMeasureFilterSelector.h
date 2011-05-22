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
      virtual ~PointMeasureFilterSelector();


    protected:
      void createSelector();


    protected: // slots (already marked as slots inside parent)
      void changeFilter(int);
  };
}

#endif
