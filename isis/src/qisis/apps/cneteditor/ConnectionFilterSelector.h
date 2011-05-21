#ifndef ConnectionFilterSelector_H
#define ConnectionFilterSelector_H


#include "AbstractFilterSelector.h"


namespace Isis
{
  class AbstractFilter;

  class ConnectionFilterSelector : public AbstractFilterSelector
  {
      Q_OBJECT

    public:
      ConnectionFilterSelector();
      virtual ~ConnectionFilterSelector();


    protected:
      void createSelector();


    protected: // slots (already marked as slots inside parent)
      void changeFilter(int);
  };
}

#endif
