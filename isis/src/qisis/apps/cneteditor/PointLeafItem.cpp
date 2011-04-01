#include "IsisDebug.h"

#include "PointLeafItem.h"


namespace Isis
{
  PointLeafItem::PointLeafItem(ControlPoint * cp,
      AbstractTreeItem * parent) : AbstractTreeItem(parent),
    AbstractPointItem(cp)
  {
  }


  PointLeafItem::~PointLeafItem()
  {
  }
}
