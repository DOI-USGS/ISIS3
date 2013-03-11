#include "IsisDebug.h"

#include "AbstractImageItem.h"

#include <iostream>

#include <QString>
#include <QVariant>

#include "ControlCubeGraphNode.h"
#include "ControlNet.h"


namespace Isis {
  namespace CnetViz {
    AbstractImageItem::AbstractImageItem(ControlCubeGraphNode *cubeGraphNode,
        int avgCharWidth, AbstractTreeItem *parent)
      : AbstractTreeItem(parent) {
      ASSERT(cubeGraphNode);
      m_ccgn = cubeGraphNode;
      calcDataWidth(avgCharWidth);

      connect(m_ccgn, SIGNAL(destroyed(QObject *)), this, SLOT(sourceDeleted()));
    }


    AbstractImageItem::~AbstractImageItem() {
      m_ccgn = NULL;
    }


    QVariant AbstractImageItem::getData() const {
      if (m_ccgn)
        return QVariant((QString)m_ccgn->getSerialNumber());
      else
        return QVariant();
    }


    QVariant AbstractImageItem::getData(QString columnTitle) const {
      return QVariant();
    }


    void AbstractImageItem::setData(QString const &columnTitle,
        QString const &newData) {
    }


    bool AbstractImageItem::isDataEditable(QString columnTitle) const {
      return false;
    }


    void AbstractImageItem::deleteSource() {
      // Shouldn't be deleting ControlCubeGraphNode's!
      ASSERT(0);
    }


    AbstractTreeItem::InternalPointerType AbstractImageItem::getPointerType() const {
      return AbstractTreeItem::CubeGraphNode;
    }


    void *AbstractImageItem::getPointer() const {
      return m_ccgn;
    }


    bool AbstractImageItem::hasNode(ControlCubeGraphNode *node) const {
      return m_ccgn == node || AbstractTreeItem::hasNode(node);
    }


    void AbstractImageItem::sourceDeleted() {
      m_ccgn = NULL;
    }
  }
}
