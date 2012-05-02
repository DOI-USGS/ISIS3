#include "IsisDebug.h"

#include "PointMeasureTreeModel.h"

#include <iostream>

#include <QFuture>
#include <QFutureWatcher>
#include <QMutex>
#include <QString>
#include <QtConcurrentMap>

#include "ControlNet.h"
#include "ControlPoint.h"

#include "TreeView.h"
#include "TreeViewContent.h"
#include "PointParentItem.h"
#include "MeasureLeafItem.h"
#include "RootItem.h"

#include <QTime>
#include <QVariant>


namespace Isis
{
  namespace CnetViz
  {
    PointMeasureTreeModel::PointMeasureTreeModel(ControlNet * controlNet,
        TreeView * v, QObject * parent) :
        AbstractTreeModel(controlNet, v, parent)
    {
      rebuildItems();
    }


    PointMeasureTreeModel::~PointMeasureTreeModel()
    {
    }


    PointMeasureTreeModel::CreateRootItemFunctor::CreateRootItemFunctor(
        AbstractTreeModel * tm, QThread * tt)
    {
      treeModel = tm;
      targetThread = tt;
      avgCharWidth = QFontMetrics(
          treeModel->getView()->getContentFont()).averageCharWidth();
    }


    PointMeasureTreeModel::CreateRootItemFunctor::CreateRootItemFunctor(
        const CreateRootItemFunctor & other)
    {
      treeModel = other.treeModel;
      targetThread = other.targetThread;
      avgCharWidth = other.avgCharWidth;
    }

    PointMeasureTreeModel::CreateRootItemFunctor::~CreateRootItemFunctor()
    {
      treeModel = NULL;
    }


    PointParentItem * PointMeasureTreeModel::CreateRootItemFunctor::operator()(
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


    void PointMeasureTreeModel::CreateRootItemFunctor::addToRootItem(
        QAtomicPointer< RootItem > & root, PointParentItem * const & item)
    {
      if (!root)
      {
        root = new RootItem;
        root->moveToThread(item->thread());
      }

      if (item)
        root->addChild(item);
    }


    PointMeasureTreeModel::CreateRootItemFunctor &
        PointMeasureTreeModel::CreateRootItemFunctor::operator=(
        const CreateRootItemFunctor & other)
    {
      if (this != &other)
      {
        treeModel = other.treeModel;
        avgCharWidth = other.avgCharWidth;
      }

      return *this;
    }

    void PointMeasureTreeModel::rebuildItems()
    {
      if (!isFrozen())
      {
        emit cancelSort();
        setRebuilding(true);
        emit filterCountsChanged(-1, getTopLevelItemCount());
        QFuture< QAtomicPointer< RootItem > > futureRoot;
        if (getRebuildWatcher()->isStarted())
        {
          futureRoot = getRebuildWatcher()->future();
          futureRoot.cancel();
        }
        
        ASSERT(getControlNetwork());
        futureRoot = QtConcurrent::mappedReduced(
            getControlNetwork()->GetPoints(),
            CreateRootItemFunctor(this, QThread::currentThread()),
            &CreateRootItemFunctor::addToRootItem,
            QtConcurrent::OrderedReduce | QtConcurrent::SequentialReduce);

        getRebuildWatcher()->setFuture(futureRoot);
      }
      else
      {
        queueRebuild();
      }
    }
  }
}

