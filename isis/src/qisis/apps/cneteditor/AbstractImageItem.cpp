#include "IsisDebug.h"

#include "AbstractImageItem.h"

#include <iostream>

#include <QString>
#include <QVariant>

#include "ControlCubeGraphNode.h"
#include "ControlNet.h"


namespace Isis
{
  namespace CnetViz
  {
    AbstractImageItem::AbstractImageItem(ControlCubeGraphNode * cubeGraphNode,
        int avgCharWidth, AbstractTreeItem * parent)
        : AbstractTreeItem(parent)
    {
      ASSERT(cubeGraphNode);
      ccgn = cubeGraphNode;
      calcDataWidth(avgCharWidth);

      connect(ccgn, SIGNAL(destroyed(QObject *)), this, SLOT(sourceDeleted()));
    }


    AbstractImageItem::~AbstractImageItem()
    {
      ccgn = NULL;
    }


    QVariant AbstractImageItem::getData() const
    {
      if (ccgn)
        return QVariant((QString)ccgn->getSerialNumber());
      else
        return QVariant();
    }


    QVariant AbstractImageItem::getData(QString columnTitle) const
    {
      return QVariant();
    }


    void AbstractImageItem::setData(QString const & columnTitle,
                                    QString const & newData)
    {
    }


    void AbstractImageItem::deleteSource()
    {
      // Shouldn't be deleting ControlCubeGraphNode's!
      ASSERT(0);
    }


    AbstractTreeItem::InternalPointerType AbstractImageItem::getPointerType() const
    {
      return AbstractTreeItem::CubeGraphNode;
    }


    void * AbstractImageItem::getPointer() const
    {
      return ccgn;
    }


    bool AbstractImageItem::hasNode(ControlCubeGraphNode * node) const
    {
      return ccgn == node || AbstractTreeItem::hasNode(node);
    }


    void AbstractImageItem::sourceDeleted() {
  //     std::cerr << "Serial item - " << ccgn << " lost\n";
      ccgn = NULL;
    }
  }
}
