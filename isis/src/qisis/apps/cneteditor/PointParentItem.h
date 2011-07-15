#ifndef PointParentItem_H
#define PointParentItem_H


#include "AbstractPointItem.h"
#include "AbstractParentItem.h"


class QVariant;

namespace Isis
{
  class ControlPoint;

  class PointParentItem : public AbstractPointItem, public AbstractParentItem
  {
    public:
      PointParentItem(Isis::ControlPoint * cp, int avgCharWidth,
          AbstractTreeItem * parent = 0);
      virtual ~PointParentItem();

      void addChild(AbstractTreeItem * child);


    private: // Disallow copying of this class
      PointParentItem(const PointParentItem & other);
      const PointParentItem & operator=(const PointParentItem & other);
  };
}

#endif
