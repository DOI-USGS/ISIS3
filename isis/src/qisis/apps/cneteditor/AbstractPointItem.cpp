#include "IsisDebug.h"

#include "AbstractPointItem.h"

#include <QVariant>

#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis
{
  AbstractPointItem::AbstractPointItem(ControlPoint * cp,
      AbstractTreeItem * parent) : AbstractTreeItem(parent)
  {
    ASSERT(cp);
    point = cp;
  }


  AbstractPointItem::~AbstractPointItem()
  {
    point = NULL;
  }


  QVariant AbstractPointItem::data() const
  {
    ASSERT(point);
    return QVariant((QString) point->GetId());
  }


  void AbstractPointItem::deleteSource()
  {
    ASSERT(point);
    point->Parent()->DeletePoint(point);
    point = NULL;
  }


  AbstractTreeItem::InternalPointerType AbstractPointItem::pointerType() const
  {
    return AbstractTreeItem::Point;
  }
}
