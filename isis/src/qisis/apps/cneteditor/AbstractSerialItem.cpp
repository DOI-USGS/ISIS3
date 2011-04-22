#include "IsisDebug.h"

#include "AbstractSerialItem.h"

#include <QVariant>

#include "ControlCubeGraphNode.h"
#include "ControlNet.h"


namespace Isis
{
  AbstractSerialItem::AbstractSerialItem(ControlCubeGraphNode * cubeGraphNode,
      AbstractTreeItem * parent) : AbstractTreeItem(parent)
  {
    ASSERT(cubeGraphNode);
    ccgn = cubeGraphNode;
  }


  AbstractSerialItem::~AbstractSerialItem()
  {
    ccgn = NULL;
  }


  QVariant AbstractSerialItem::data() const
  {
    ASSERT(ccgn);
    return QVariant((QString) ccgn->getSerialNumber());
  }


  void AbstractSerialItem::deleteSource()
  {
    // Shouldn't be deleting ControlCubeGraphNode's!
    ASSERT(0);
  }


  AbstractTreeItem::InternalPointerType AbstractSerialItem::pointerType() const
  {
    return AbstractTreeItem::CubeGraphNode;
  }


  bool AbstractSerialItem::hasNode(ControlCubeGraphNode * node) const
  {
    return ccgn == node || AbstractTreeItem::hasNode(node);
  }

}
