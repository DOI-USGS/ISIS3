#ifndef MeasureLeafItem_H
#define MeasureLeafItem_H


#include "AbstractMeasureItem.h"
#include "AbstractLeafItem.h"


namespace Isis
{
  class ControlMeasure;

  namespace CnetViz
  {

    /**
     * A leaf node in the tree structure that represents a control measure.
     *
     * This class represents a leaf node in the tree model and has a control
     * measure as data. When created, it was used in the point model to show
     * which control measures are in a control point or on an image.
     *
     * @author ????-??-?? Eric Hyer
     *
     * @internal
     */
    class MeasureLeafItem : public AbstractMeasureItem, public AbstractLeafItem
    {
      public:
        MeasureLeafItem(ControlMeasure * cm, int avgCharWidth,
            AbstractTreeItem * parent = 0);
        virtual ~MeasureLeafItem();


      private: // Disallow copying of this class
        MeasureLeafItem(const MeasureLeafItem & other);
        const MeasureLeafItem & operator=(const MeasureLeafItem & other);
    };
  }
}

#endif
