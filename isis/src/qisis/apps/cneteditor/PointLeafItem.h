#ifndef PointLeafItem_H
#define PointLeafItem_H


#include "AbstractPointItem.h"
#include "AbstractLeafItem.h"


namespace Isis
{
  class ControlPoint;

  namespace CnetViz
  {

    /**
     * @brief A leaf node in the tree structure that represents a control point
     *
     * This class represents a leaf node in the tree model and has a control
     * point as data. When created, it was used in the serial model to show
     * which control points connect which images.
     *
     * @author ????-??-?? Eric Hyer
     *
     * @internal
     */
    class PointLeafItem : public AbstractPointItem, public AbstractLeafItem
    {
      public:
        PointLeafItem(ControlPoint * cp, int avgCharWidth,
            AbstractTreeItem * parent = 0);
        virtual ~PointLeafItem();


      private: // Disallow copying of this class
        PointLeafItem(const PointLeafItem & other);
        const PointLeafItem & operator=(const PointLeafItem & other);
    };
  }
}

#endif
