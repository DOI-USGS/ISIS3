#include "IsisDebug.h"

#include "PointParentItem.h"

#include "MeasureLeafItem.h"


namespace Isis
{
  PointParentItem::PointParentItem(ControlPoint * cp,
      AbstractTreeItem * parent) : AbstractTreeItem(parent),
    AbstractPointItem(cp)
  {
  }


  PointParentItem::~PointParentItem()
  {
  }


  void PointParentItem::addChild(AbstractTreeItem * child)
  {
    // Only MeasureLeafItems should be children of PointParentItems
    ASSERT(dynamic_cast< MeasureLeafItem * >(child));
    AbstractParentItem::addChild(child);
  }
}
