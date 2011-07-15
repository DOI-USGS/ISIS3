#include "IsisDebug.h"

#include "PointLeafItem.h"


namespace Isis
{
  PointLeafItem::PointLeafItem(ControlPoint * cp, int avgCharWidth,
      AbstractTreeItem * parent) : AbstractTreeItem(parent),
    AbstractPointItem(cp, avgCharWidth)
  {
  }


  PointLeafItem::~PointLeafItem()
  {
  }
}
