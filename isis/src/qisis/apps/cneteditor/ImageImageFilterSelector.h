#ifndef ImageImageFilterSelector_H
#define ImageImageFilterSelector_H


#include "AbstractFilterSelector.h"


namespace Isis
{
  namespace CnetViz
  {
    class AbstractFilter;

    class ImageImageFilterSelector : public AbstractFilterSelector
    {
        Q_OBJECT

      public:
        ImageImageFilterSelector();
        ImageImageFilterSelector(const ImageImageFilterSelector & other);
        virtual ~ImageImageFilterSelector();
        ImageImageFilterSelector & operator=(
          const ImageImageFilterSelector & other);


      protected:
        void createSelector();


      protected: // slots (already marked as slots inside parent)
        void changeFilter(int);
    };
  }
}

#endif
