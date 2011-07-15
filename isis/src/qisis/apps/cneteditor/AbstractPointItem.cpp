#include "IsisDebug.h"

#include "AbstractPointItem.h"

#include <QString>

#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis
{
  AbstractPointItem::AbstractPointItem(ControlPoint * cp,
      int avgCharWidth, AbstractTreeItem * parent)
    : AbstractTreeItem(parent)
  {
    ASSERT(cp);
    point = cp;
    calcDataWidth(avgCharWidth);
  }


  AbstractPointItem::~AbstractPointItem()
  {
    point = NULL;
  }


  QString AbstractPointItem::getData() const
  {
    ASSERT(point);
    return (QString) point->GetId();
  }


  void AbstractPointItem::deleteSource()
  {
    ASSERT(point);
    point->Parent()->DeletePoint(point);
    point = NULL;
  }


  AbstractTreeItem::InternalPointerType AbstractPointItem::getPointerType() const
  {
    return AbstractTreeItem::Point;
  }


  void * AbstractPointItem::getPointer() const
  {
    return point;
  }


  bool AbstractPointItem::hasPoint(ControlPoint * p) const
  {
    return point == p;
  }
}
