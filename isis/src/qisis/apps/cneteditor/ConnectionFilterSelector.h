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
      ConnectionFilterSelector(const ConnectionFilterSelector & other);
      virtual ~ConnectionFilterSelector();
      ConnectionFilterSelector & operator=(
        const ConnectionFilterSelector & other);


    protected:
      void createSelector();


    protected: // slots (already marked as slots inside parent)
      void changeFilter(int);
  };
}

#endif
