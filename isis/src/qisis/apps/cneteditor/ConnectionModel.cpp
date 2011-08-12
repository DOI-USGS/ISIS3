#include "IsisDebug.h"

#include "ConnectionModel.h"

#include <iostream>

#include <QFuture>
#include <QFutureWatcher>
#include <QList>
#include <QString>
#include <QtConcurrentMap>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlNet.h"

#include "CnetTreeView.h"
#include "CnetTreeViewContent.h"
#include "PointLeafItem.h"
#include "RootItem.h"
#include "SerialLeafItem.h"
#include "SerialParentItem.h"


#include <QTime>

using std::cerr;


namespace Isis
{
  ConnectionModel::ConnectionModel(ControlNet * cNet, CnetTreeView * v,
      QObject * parent) : TreeModel(cNet, v, parent)
  {
    rebuildItems();
  }


  ConnectionModel::~ConnectionModel()
  {
  }


  ConnectionModel::CreateRootItemFunctor::CreateRootItemFunctor(TreeModel * tm,
                                                                QThread * tt)
  {
    treeModel = tm;
    targetThread = tt;
    avgCharWidth = QFontMetrics(
        treeModel->getView()->getContentFont()).averageCharWidth();
  }


  ConnectionModel::CreateRootItemFunctor::CreateRootItemFunctor(
    const CreateRootItemFunctor & other)
  {
    treeModel = other.treeModel;
    targetThread = other.targetThread;
    avgCharWidth = other.avgCharWidth;
  }


  ConnectionModel::CreateRootItemFunctor::~CreateRootItemFunctor()
  {
    treeModel = NULL;
    targetThread = NULL;
  }


  SerialParentItem * ConnectionModel::CreateRootItemFunctor::operator()(
    ControlCubeGraphNode * const & node) const
  {
    SerialParentItem * parentItem =
        new SerialParentItem(node, avgCharWidth);
    parentItem->setSelectable(false);
    parentItem->moveToThread(targetThread);

    QList< ControlCubeGraphNode * > connectedNodes = node->getAdjacentNodes();

    for (int j = 0; j < connectedNodes.size(); j++)
    {
      ControlCubeGraphNode * connectedNode = connectedNodes[j];
      SerialLeafItem * serialItem =
        new SerialLeafItem(connectedNode, avgCharWidth, parentItem);
      serialItem->setSelectable(false);
      serialItem->moveToThread(targetThread);

      parentItem->addChild(serialItem);
    }

    return parentItem;
  }


  void ConnectionModel::CreateRootItemFunctor::addToRootItem(
    QAtomicPointer< RootItem > & root, SerialParentItem * const & item)
  {
    if (!root) {
      root = new RootItem;
      root->moveToThread(item->thread());
    }

    if (item)
      root->addChild(item);
  }


  ConnectionModel::CreateRootItemFunctor &
  ConnectionModel::CreateRootItemFunctor::operator=(
    const CreateRootItemFunctor & other)
  {
    if (this != &other)
    {
      treeModel = other.treeModel;
      avgCharWidth = other.avgCharWidth;
    }

    return *this;
  }


  void ConnectionModel::rebuildItems()
  {
//     cerr << "ConnectionModel::rebuildItems called\n";
    emit filterCountsChanged(-1, getTopLevelItemCount());
    QFuture< QAtomicPointer< RootItem > > futureRoot;

    if (getRebuildWatcher()->isStarted())
    {
      futureRoot = getRebuildWatcher()->future();
      futureRoot.cancel();
//       futureRoot.waitForFinished();
//       if (futureRoot.result())
//         delete futureRoot.result();
    }

    futureRoot = QtConcurrent::mappedReduced(
        getControlNetwork()->GetCubeGraphNodes(),
        CreateRootItemFunctor(this, QThread::currentThread()),
        &CreateRootItemFunctor::addToRootItem,
        QtConcurrent::OrderedReduce | QtConcurrent::SequentialReduce);

    getRebuildWatcher()->setFuture(futureRoot);
//     cerr << "/ConnectionModel::rebuildItems\n";
  }
}

