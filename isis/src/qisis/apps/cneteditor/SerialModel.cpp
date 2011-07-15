#include "IsisDebug.h"

#include "SerialModel.h"

#include <iostream>

#include <QFuture>
#include <QFutureWatcher>
#include <QList>
#include <QModelIndex>
#include <QString>
#include <QtConcurrentMap>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"

#include "CnetView.h"
#include "CnetViewContent.h"
#include "PointLeafItem.h"
#include "RootItem.h"
#include "SerialParentItem.h"

#include <QTime>


using std::cerr;


namespace Isis
{
  SerialModel::SerialModel(ControlNet * controlNet, CnetView * v,
      QObject * parent) : TreeModel(controlNet, v, parent)
  {
    rebuildItems();
  }


  SerialModel::~SerialModel()
  {
  }


  SerialModel::CreateRootItemFunctor::CreateRootItemFunctor(TreeModel * tm)
  {
    treeModel = tm;
    avgCharWidth = QFontMetrics(
        treeModel->getView()->getContentFont()).averageCharWidth();
  }


  SerialModel::CreateRootItemFunctor::CreateRootItemFunctor(
    const CreateRootItemFunctor & other)
  {
    treeModel = other.treeModel;
    avgCharWidth = other.avgCharWidth;
  }


  SerialModel::CreateRootItemFunctor::~CreateRootItemFunctor()
  {
    treeModel = NULL;
  }


  SerialParentItem * SerialModel::CreateRootItemFunctor::operator()(
    ControlCubeGraphNode * const & node) const
  {
    SerialParentItem * serialItem = NULL;

    serialItem = new SerialParentItem(node, avgCharWidth);
    serialItem->setSelectable(false);
    QList< ControlMeasure * > measures = node->getMeasures();
    for (int j = 0; j < measures.size(); j++)
    {
      ASSERT(measures[j]);
      ControlPoint * point = measures[j]->Parent();
      ASSERT(point);
      PointLeafItem * pointItem = new PointLeafItem(
        point, avgCharWidth, serialItem);
      pointItem->setSelectable(false);
      serialItem->addChild(pointItem);
    }

    return serialItem;
  }


  void SerialModel::CreateRootItemFunctor::addToRootItem(
    QAtomicPointer< RootItem > & root, SerialParentItem * const & item)
  {
    if (!root)
      root = new RootItem;

    if (item)
      root->addChild(item);
  }


  SerialModel::CreateRootItemFunctor &
  SerialModel::CreateRootItemFunctor::operator=(
    const CreateRootItemFunctor & other)
  {
    if (this != &other)
    {
      treeModel = other.treeModel;
      avgCharWidth = other.avgCharWidth;
    }

    return *this;
  }


  void SerialModel::rebuildItems()
  {
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
        CreateRootItemFunctor(this),
        &CreateRootItemFunctor::addToRootItem,
        QtConcurrent::OrderedReduce | QtConcurrent::SequentialReduce);

    getRebuildWatcher()->setFuture(futureRoot);
  }
}

