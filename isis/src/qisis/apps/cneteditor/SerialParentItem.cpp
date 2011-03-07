#include "IsisDebug.h"

#include "SerialParentItem.h"

#include <iostream>

#include <QVariant>

#include "ControlCubeGraphNode.h"
#include "ControlNet.h"
#include "MeasureChildItem.h"
#include "PointChildItem.h"


using std::cerr;


namespace Isis
{
  SerialParentItem::SerialParentItem(ControlCubeGraphNode * cubeGraphNode,
      TreeItem * parent) : TreeItem(parent)
  {
    ccgn = cubeGraphNode;
    ASSERT(ccgn);
  }


  SerialParentItem::~SerialParentItem()
  {
    ccgn = NULL;
  }


  void SerialParentItem::addChild(TreeItem * child)
  {
    // Only MeasureChildItems or PointChildItems should be children of
    // SerialParentItems
    ASSERT(dynamic_cast< MeasureChildItem * >(child) ||
        dynamic_cast< PointChildItem * >(child));

    children->append(child);
  }


  void SerialParentItem::removeChild(int row)
  {
    children->removeAt(row);
  }


  QVariant SerialParentItem::data(int column) const
  {
    ASSERT(ccgn);
    validateColumn(column);
//     return QVariant((measure->*cmGetter(column))());

    return QVariant((QString) ccgn->getSerialNumber());
  }


  void SerialParentItem::setData(int column, const QVariant & value)
  {
    validateColumn(column);
  }


  void SerialParentItem::deleteSource()
  {
    ASSERT(ccgn);

    // not sure yet if we will be deleting serials from the network using
    // this class
  }


  TreeItem::InternalPointerType SerialParentItem::pointerType() const
  {
    return TreeItem::Serial;
  }

}
