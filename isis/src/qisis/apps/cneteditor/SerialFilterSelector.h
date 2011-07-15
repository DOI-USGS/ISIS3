#ifndef SerialFilterSelector_H
#define SerialFilterSelector_H


#include "AbstractFilterSelector.h"


namespace Isis
{
  class AbstractFilter;

  class SerialFilterSelector : public AbstractFilterSelector
  {
      Q_OBJECT

    public:
      SerialFilterSelector();
      SerialFilterSelector(const SerialFilterSelector & other);
      virtual ~SerialFilterSelector();
      SerialFilterSelector & operator=(const SerialFilterSelector & other);


    protected:
      void createSelector();


    protected: // slots (already marked as slots inside parent)
      void changeFilter(int);
  };
}

#endif
