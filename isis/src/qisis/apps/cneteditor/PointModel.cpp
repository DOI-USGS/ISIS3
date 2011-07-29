#include "IsisDebug.h"

#include "PointModel.h"

#include <iostream>

#include <QFuture>
#include <QFutureWatcher>
#include <QMutex>
#include <QString>
#include <QtConcurrentMap>

#include "ControlNet.h"
#include "ControlPoint.h"

#include "CnetTreeView.h"
#include "CnetTreeViewContent.h"
#include "PointParentItem.h"
#include "MeasureLeafItem.h"
#include "RootItem.h"

#include <QTime>
#include <QVariant>


using std::cerr;


namespace Isis
{
  PointModel::PointModel(ControlNet * controlNet, CnetTreeView * v,
      QObject * parent) : TreeModel(controlNet, v, parent)
  {
    rebuildItems();
  }


  PointModel::~PointModel()
  {
  }


  PointModel::CreateRootItemFunctor::CreateRootItemFunctor(TreeModel * tm)
  {
    treeModel = tm;
    avgCharWidth = QFontMetrics(
        treeModel->getView()->getContentFont()).averageCharWidth();
  }


  PointModel::CreateRootItemFunctor::CreateRootItemFunctor(
    const CreateRootItemFunctor & other)
  {
    treeModel = other.treeModel;
    avgCharWidth = other.avgCharWidth;
  }

  PointModel::CreateRootItemFunctor::~CreateRootItemFunctor()
  {
    treeModel = NULL;
  }


  PointParentItem * PointModel::CreateRootItemFunctor::operator()(
    ControlPoint * const & point) const
  {
    PointParentItem * pointItem = new PointParentItem(point, avgCharWidth);

    for (int j = 0; j < point->GetNumMeasures(); j++)
    {
      const ControlMeasure * measure = point->GetMeasure(j);
      ASSERT(measure);

      MeasureLeafItem * measureItem = new MeasureLeafItem(
        const_cast< ControlMeasure * >(measure), avgCharWidth, pointItem);
      pointItem->addChild(measureItem);
    }

    return pointItem;
  }


  void PointModel::CreateRootItemFunctor::addToRootItem(
    QAtomicPointer< RootItem > & root, PointParentItem * const & item)
  {
    if (!root)
      root = new RootItem;

    if (item)
      root->addChild(item);
  }


  PointModel::CreateRootItemFunctor &
  PointModel::CreateRootItemFunctor::operator=(
    const CreateRootItemFunctor & other)
  {
    if (this != &other)
    {
      treeModel = other.treeModel;
      avgCharWidth = other.avgCharWidth;
    }

    return *this;
  }

  void PointModel::rebuildItems()
  {
    emit filterCountsChanged(-1, getTopLevelItemCount());
    QFuture< QAtomicPointer< RootItem > > futureRoot;
    if (getRebuildWatcher()->isStarted())
    {
      futureRoot = getRebuildWatcher()->future();
      futureRoot.cancel();
    }

    futureRoot = QtConcurrent::mappedReduced(
        getControlNetwork()->getPoints(),
        CreateRootItemFunctor(this),
        &CreateRootItemFunctor::addToRootItem,
        QtConcurrent::OrderedReduce | QtConcurrent::SequentialReduce);

    getRebuildWatcher()->setFuture(futureRoot);
  }
}
