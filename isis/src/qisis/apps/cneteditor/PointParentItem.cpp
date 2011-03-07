#include "IsisDebug.h"

#include "PointParentItem.h"

#include <iostream>

#include <QVariant>

#include "ControlPoint.h"
#include "ControlNet.h"
#include "iException.h"
#include "iString.h"

#include "MeasureChildItem.h"


using std::cerr;


namespace Isis
{
  PointParentItem::PointParentItem(ControlPoint * cp,
      TreeItem * parent) : TreeItem(parent)
  {
    point = cp;
    ASSERT(cp);
  }


  PointParentItem::~PointParentItem()
  {
    point = NULL;
  }


  void PointParentItem::addChild(TreeItem * child)
  {
    // Only MeasureChildItems should be children of PointParentItems
    ASSERT(dynamic_cast< MeasureChildItem * >(child));

    children->append(child);
  }


  void PointParentItem::removeChild(int row)
  {
    children->removeAt(row);
  }


  QVariant PointParentItem::data(int column) const
  {
    ASSERT(point);
    validateColumn(column);
//     return QVariant((measure->*cmGetter(column))());

    return QVariant((QString) point->GetId());
  }


  void PointParentItem::setData(int column, const QVariant & value)
  {
    validateColumn(column);
  }


  void PointParentItem::deleteSource()
  {
    ASSERT(point);

    point->Parent()->DeletePoint(point);
    point = NULL;
  }


  TreeItem::InternalPointerType PointParentItem::pointerType() const
  {
    return TreeItem::Point;
  }

}
