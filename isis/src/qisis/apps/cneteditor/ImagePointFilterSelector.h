#ifndef ImagePointFilterSelector_H
#define ImagePointFilterSelector_H


#include "AbstractFilterSelector.h"


namespace Isis
{
  namespace CnetViz
  {
    class AbstractFilter;

    class ImagePointFilterSelector : public AbstractFilterSelector
    {
        Q_OBJECT

      public:
        ImagePointFilterSelector();
        ImagePointFilterSelector(const ImagePointFilterSelector & other);
        virtual ~ImagePointFilterSelector();
        ImagePointFilterSelector & operator=(
            const ImagePointFilterSelector & other);


      protected:
        void createSelector();


      protected: // slots (already marked as slots inside parent)
        void changeFilter(int);
    };
  }
}

#endif
