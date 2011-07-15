#include "IsisDebug.h"

#include "AbstractSerialItem.h"

#include <iostream>

#include <QString>

#include "ControlCubeGraphNode.h"
#include "ControlNet.h"


namespace Isis
{
  AbstractSerialItem::AbstractSerialItem(ControlCubeGraphNode * cubeGraphNode,
      int avgCharWidth, AbstractTreeItem * parent)
    : AbstractTreeItem(parent)
  {
    ASSERT(cubeGraphNode);
    ccgn = cubeGraphNode;
    calcDataWidth(avgCharWidth);
  }


  AbstractSerialItem::~AbstractSerialItem()
  {
    ccgn = NULL;
  }


  QString AbstractSerialItem::getData() const
  {
    ASSERT(ccgn);
    return (QString) ccgn->getSerialNumber();
  }


  void AbstractSerialItem::deleteSource()
  {
    // Shouldn't be deleting ControlCubeGraphNode's!
    ASSERT(0);
  }


  AbstractTreeItem::InternalPointerType AbstractSerialItem::getPointerType() const
  {
    return AbstractTreeItem::CubeGraphNode;
  }


  void * AbstractSerialItem::getPointer() const
  {
    return ccgn;
  }


  bool AbstractSerialItem::hasNode(ControlCubeGraphNode * node) const
  {
    return ccgn == node || AbstractTreeItem::hasNode(node);
  }

}
