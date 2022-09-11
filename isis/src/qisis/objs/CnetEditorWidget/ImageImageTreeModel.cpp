/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ImageImageTreeModel.h"

#include <iostream>

#include <QFuture>
#include <QFutureWatcher>
#include <QList>
#include <QString>
#include <QtConcurrentMap>

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
    AbstractTreeModel *tm, ControlNet *net, QThread *tt) {
    m_treeModel = tm;
    m_controlNet = net;
    m_targetThread = tt;
    m_avgCharWidth = QFontMetrics(
        m_treeModel->getView()->getContentFont()).averageCharWidth();
  }


  ImageImageTreeModel::CreateRootItemFunctor::CreateRootItemFunctor(
    const CreateRootItemFunctor &other) {
    m_treeModel = other.m_treeModel;
    m_controlNet = other.m_controlNet;
    m_targetThread = other.m_targetThread;
    m_avgCharWidth = other.m_avgCharWidth;
  }


  ImageImageTreeModel::CreateRootItemFunctor::~CreateRootItemFunctor() {
    m_treeModel = NULL;
    m_controlNet = NULL;
    m_targetThread = NULL;
  }


  ImageParentItem *ImageImageTreeModel::CreateRootItemFunctor::operator()(
    const QString imageSerial) const {

    ImageParentItem *parentItem =
          new ImageParentItem(imageSerial, m_controlNet, m_avgCharWidth);
    parentItem->setSelectable(false);
    parentItem->moveToThread(m_targetThread);

    QList< QString > connectedImages = m_controlNet->getAdjacentImages(imageSerial);

    for (int j = 0; j < connectedImages.size(); j++) {
      ImageLeafItem *childItem =
            new ImageLeafItem(connectedImages[j], m_controlNet, m_avgCharWidth, parentItem);
      childItem->setSelectable(false);
      childItem->moveToThread(m_targetThread);

      parentItem->addChild(childItem);
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
          getControlNetwork()->GetCubeSerials(),
          CreateRootItemFunctor(this, getControlNetwork(), QThread::currentThread()),
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
