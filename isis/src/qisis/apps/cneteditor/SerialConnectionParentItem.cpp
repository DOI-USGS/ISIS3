#include "IsisDebug.h"

#include "SerialConnectionParentItem.h"

#include <iostream>

#include <QVariant>

#include "ControlCubeGraphNode.h"
#include "ControlNet.h"

#include "SerialParentItem.h"


using std::cerr;


namespace Isis
{
  SerialConnectionParentItem::SerialConnectionParentItem(
    ControlCubeGraphNode * cubeGraphNode, TreeItem * parent) : TreeItem(
        parent)
  {
    ccgn = cubeGraphNode;
    ASSERT(ccgn);
  }


  SerialConnectionParentItem::~SerialConnectionParentItem()
  {
    ccgn = NULL;
  }


  void SerialConnectionParentItem::addChild(TreeItem * child)
  {
    ASSERT(dynamic_cast< SerialParentItem * >(child));

    children->append(child);
  }


  void SerialConnectionParentItem::removeChild(int row)
  {
    children->removeAt(row);
  }


  QVariant SerialConnectionParentItem::data(int column) const
  {
    ASSERT(ccgn);
    validateColumn(column);
//     return QVariant((measure->*cmGetter(column))());

    return QVariant((QString) ccgn->getSerialNumber());
  }


  void SerialConnectionParentItem::setData(int column, const QVariant & value)
  {
    validateColumn(column);
  }


  void SerialConnectionParentItem::deleteSource()
  {
    ASSERT(ccgn);

    // not sure yet if we will be deleting serials from the network using
    // this class
  }


  TreeItem::InternalPointerType SerialConnectionParentItem::pointerType() const
  {
    return TreeItem::ConnectionParent;
  }
}
