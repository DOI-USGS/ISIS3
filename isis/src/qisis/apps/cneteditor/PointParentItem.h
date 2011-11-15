#ifndef PointParentItem_H
#define PointParentItem_H


#include "AbstractParentItem.h"
#include "AbstractPointItem.h"


class QVariant;

namespace Isis
{
  class ControlPoint;
  
  namespace CnetViz
  {
    class PointParentItem : public AbstractPointItem, public AbstractParentItem
    {
      public:
        PointParentItem(ControlPoint * cp, int avgCharWidth,
            AbstractTreeItem * parent = 0);
        virtual ~PointParentItem();

        void addChild(AbstractTreeItem * child);


      private: // Disallow copying of this class
        PointParentItem(const PointParentItem & other);
        const PointParentItem & operator=(const PointParentItem & other);
    };
  }
}

#endif
