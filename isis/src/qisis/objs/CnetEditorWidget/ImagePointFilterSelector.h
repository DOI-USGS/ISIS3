#ifndef ImagePointFilterSelector_H
#define ImagePointFilterSelector_H


#include "AbstractFilterSelector.h"


namespace Isis {

  /**
   * @brief Allows users to choose filters for filtering images and points
   *
   * This class is responsible for creating a list of filters that can be
   * selected for filtering images and points.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   */
  class ImagePointFilterSelector : public AbstractFilterSelector {
      Q_OBJECT

    public:
      ImagePointFilterSelector();
      ImagePointFilterSelector(const ImagePointFilterSelector &other);
      virtual ~ImagePointFilterSelector();
      ImagePointFilterSelector &operator=(const ImagePointFilterSelector &other);


    protected:
      void createSelector();


    protected: // slots (already marked as slots inside parent)
      void changeFilter(int);
  };
}

#endif
