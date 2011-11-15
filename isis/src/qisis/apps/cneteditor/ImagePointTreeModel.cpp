#include "IsisDebug.h"

#include "ImagePointTreeModel.h"

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

#include "TreeView.h"
#include "TreeViewContent.h"
#include "PointLeafItem.h"
#include "RootItem.h"
#include "ImageParentItem.h"

#include <QTime>


using std::cerr;


namespace Isis
{
  namespace CnetViz
  {
    ImagePointTreeModel::ImagePointTreeModel(ControlNet * controlNet,
        TreeView * v, QObject * parent) :
        AbstractTreeModel(controlNet, v, parent)
    {
      rebuildItems();
    }


    ImagePointTreeModel::~ImagePointTreeModel()
    {
    }


    ImagePointTreeModel::CreateRootItemFunctor::CreateRootItemFunctor(
        AbstractTreeModel * tm, QThread * tt)
    {
      treeModel = tm;
      targetThread = tt;
      avgCharWidth = QFontMetrics(
          treeModel->getView()->getContentFont()).averageCharWidth();
    }


    ImagePointTreeModel::CreateRootItemFunctor::CreateRootItemFunctor(
        const CreateRootItemFunctor & other)
    {
      treeModel = other.treeModel;
      targetThread = other.targetThread;
      avgCharWidth = other.avgCharWidth;
    }


    ImagePointTreeModel::CreateRootItemFunctor::~CreateRootItemFunctor()
    {
      targetThread = NULL;
      treeModel = NULL;
    }


    ImageParentItem * ImagePointTreeModel::CreateRootItemFunctor::operator()(
        ControlCubeGraphNode * const & node) const
    {
      ImageParentItem * imageItem = NULL;

      imageItem = new ImageParentItem(node, avgCharWidth);
      imageItem->setSelectable(false);
      imageItem->moveToThread(targetThread);
      QList< ControlMeasure * > measures = node->getMeasures();
      for (int j = 0; j < measures.size(); j++)
      {
        ASSERT(measures[j]);
        ControlPoint * point = measures[j]->Parent();
        
        ASSERT(point);
        PointLeafItem * pointItem = new PointLeafItem(
          point, avgCharWidth, imageItem);
        pointItem->setSelectable(false);
        pointItem->moveToThread(targetThread);

        imageItem->addChild(pointItem);
      }

      return imageItem;
    }


    void ImagePointTreeModel::CreateRootItemFunctor::addToRootItem(
        QAtomicPointer< RootItem > & root, ImageParentItem * const & item)
    {
      if (!root) {
        root = new RootItem;
        root->moveToThread(item->thread());
      }

      if (item)
        root->addChild(item);
    }


    ImagePointTreeModel::CreateRootItemFunctor &
        ImagePointTreeModel::CreateRootItemFunctor::operator=(
        const CreateRootItemFunctor & other)
    {
      if (this != &other)
      {
        treeModel = other.treeModel;
        avgCharWidth = other.avgCharWidth;
      }

      return *this;
    }


    void ImagePointTreeModel::rebuildItems()
    {
  //     cerr << "ImagePointTreeModel::rebuildItems\n";
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
      }
      else
      {
        queueRebuild();
      }
  //     cerr << "/ImagePointTreeModel::rebuildItems\n";
    }
  }
}
