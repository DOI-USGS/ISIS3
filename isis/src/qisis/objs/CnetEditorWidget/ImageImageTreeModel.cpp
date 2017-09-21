#include "IsisDebug.h"

#include "ImageImageTreeModel.h"

#include <iostream>

#include <QFuture>
#include <QFutureWatcher>
#include <QList>
#include <QString>
#include <QtConcurrentMap>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlNet.h"

#include "TreeView.h"
#include "TreeViewContent.h"
#include "PointLeafItem.h"
#include "RootItem.h"
#include "ImageLeafItem.h"
#include "ImageParentItem.h"


#include <QTime>

using std::cerr;


namespace Isis {
  ImageImageTreeModel::ImageImageTreeModel(ControlNet *cNet, TreeView *v,
      QObject *parent) : AbstractTreeModel(cNet, v, parent) {
    rebuildItems();
  }


  ImageImageTreeModel::~ImageImageTreeModel() {
  }


  ImageImageTreeModel::CreateRootItemFunctor::CreateRootItemFunctor(
    AbstractTreeModel *tm, QThread *tt) {
    m_treeModel = tm;
    m_targetThread = tt;
    m_avgCharWidth = QFontMetrics(
        m_treeModel->getView()->getContentFont()).averageCharWidth();
  }


  ImageImageTreeModel::CreateRootItemFunctor::CreateRootItemFunctor(
    const CreateRootItemFunctor &other) {
    m_treeModel = other.m_treeModel;
    m_targetThread = other.m_targetThread;
    m_avgCharWidth = other.m_avgCharWidth;
  }


  ImageImageTreeModel::CreateRootItemFunctor::~CreateRootItemFunctor() {
    m_treeModel = NULL;
    m_targetThread = NULL;
  }


  ImageParentItem *ImageImageTreeModel::CreateRootItemFunctor::operator()(
    ControlCubeGraphNode *const &node) const {
    ImageParentItem *parentItem =
      new ImageParentItem(node, m_avgCharWidth);
    parentItem->setSelectable(false);
    parentItem->moveToThread(m_targetThread);

    QList< ControlCubeGraphNode * > connectedNodes = node->getAdjacentNodes();

    for (int j = 0; j < connectedNodes.size(); j++) {
      ControlCubeGraphNode *connectedNode = connectedNodes[j];
      ImageLeafItem *serialItem =
        new ImageLeafItem(connectedNode, m_avgCharWidth, parentItem);
      serialItem->setSelectable(false);
      serialItem->moveToThread(m_targetThread);

      parentItem->addChild(serialItem);
    }

    return parentItem;
  }


  void ImageImageTreeModel::CreateRootItemFunctor::addToRootItem(
    QAtomicPointer< RootItem > & root, ImageParentItem *const &item) {

    // Allocate a new root item if our root is NULL
    if (root.testAndSetOrdered(NULL, new RootItem)) {
      root.loadAcquire()->moveToThread(item->thread());
    }

    if (item)
      root.loadAcquire()->addChild(item);
  }


  ImageImageTreeModel::CreateRootItemFunctor &
  ImageImageTreeModel::CreateRootItemFunctor::operator=(
    const CreateRootItemFunctor &other) {
    if (this != &other) {
      m_treeModel = other.m_treeModel;
      m_avgCharWidth = other.m_avgCharWidth;
    }

    return *this;
  }


  void ImageImageTreeModel::rebuildItems() {
    //     cerr << "ImageImageTreeModel::rebuildItems called\n";
    if (!isFrozen()) {
      emit cancelSort();
      setRebuilding(true);
      emit filterCountsChanged(-1, getTopLevelItemCount());
      QFuture< QAtomicPointer< RootItem > > futureRoot;

      if (getRebuildWatcher()->isStarted()) {
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
    else {
      queueRebuild();
    }
    //     cerr << "/ImageImageTreeModel::rebuildItems\n";
  }
}
