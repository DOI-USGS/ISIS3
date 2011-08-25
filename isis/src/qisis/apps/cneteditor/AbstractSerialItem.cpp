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

    connect(ccgn, SIGNAL(destroyed(QObject *)), this, SLOT(sourceDeleted()));
  }


  AbstractSerialItem::~AbstractSerialItem()
  {
    ccgn = NULL;
  }


  QString AbstractSerialItem::getData() const
  {
    if (ccgn)
      return (QString) ccgn->getSerialNumber();
    else
      return QString();
  }


  QString AbstractSerialItem::getData(QString columnTitle) const
  {
    return QString();
  }


  void AbstractSerialItem::setData(QString const & columnTitle,
                                   QString const & newData)
  {
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


  void AbstractSerialItem::sourceDeleted() {
//     std::cerr << "Serial item - " << ccgn << " lost\n";
    ccgn = NULL;
  }
}

