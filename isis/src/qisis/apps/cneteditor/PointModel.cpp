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


  PointModel::CreateRootItemFunctor::CreateRootItemFunctor(TreeModel * tm,
                                                           QThread * tt
  )
  {
    treeModel = tm;
    targetThread = tt;
    avgCharWidth = QFontMetrics(
        treeModel->getView()->getContentFont()).averageCharWidth();
  }


  PointModel::CreateRootItemFunctor::CreateRootItemFunctor(
    const CreateRootItemFunctor & other)
  {
    treeModel = other.treeModel;
    targetThread = other.targetThread;
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
    pointItem->moveToThread(targetThread);

    for (int j = 0; j < point->GetNumMeasures(); j++)
    {
      const ControlMeasure * measure = point->GetMeasure(j);
      ASSERT(measure);

      MeasureLeafItem * measureItem = new MeasureLeafItem(
        const_cast< ControlMeasure * >(measure), avgCharWidth, pointItem);
      measureItem->moveToThread(targetThread);

      pointItem->addChild(measureItem);
    }

    return pointItem;
  }


  void PointModel::CreateRootItemFunctor::addToRootItem(
    QAtomicPointer< RootItem > & root, PointParentItem * const & item)
  {
    if (!root) {
      root = new RootItem;
      root->moveToThread(item->thread());
    }

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
//     cerr << "PointModel::rebuildItems\n";
    if (!isFrozen())
    {
      emit filterCountsChanged(-1, getTopLevelItemCount());
      QFuture< QAtomicPointer< RootItem > > futureRoot;
      if (getRebuildWatcher()->isStarted())
      {
        futureRoot = getRebuildWatcher()->future();
        futureRoot.cancel();
      }
      
  //     cerr << "PointModel::rebulidItems... getPoints has size : "
  //          << getControlNetwork()->getPoints().size() << "\n";

      ASSERT(getControlNetwork());
      futureRoot = QtConcurrent::mappedReduced(
          getControlNetwork()->getPoints(),
          CreateRootItemFunctor(this, QThread::currentThread()),
          &CreateRootItemFunctor::addToRootItem,
          QtConcurrent::OrderedReduce | QtConcurrent::SequentialReduce);

      getRebuildWatcher()->setFuture(futureRoot);
    }
    else
    {
      queueRebuild();
    }
//     cerr << "/PointModel::rebuildItems\n";
  }
}
