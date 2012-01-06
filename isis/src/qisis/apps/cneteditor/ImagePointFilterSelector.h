#ifndef ImagePointFilterSelector_H
#define ImagePointFilterSelector_H


#include "AbstractFilterSelector.h"


namespace Isis
{
  namespace CnetViz
  {
    class AbstractFilter;

    /**
     * @brief Allows users to choose filters for filtering images and points
     *
     * This class is responsible for creating a list of filters that can be
     * selected for filtering images and points.
     *
     * @author ????-??-?? Eric Hyer
     *
     * @internal
     */
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
