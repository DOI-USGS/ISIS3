/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ImagePointTreeModel.h"

#include <iostream>

#include <QFuture>
#include <QFutureWatcher>
#include <QList>
#include <QModelIndex>
#include <QString>
#include <QtConcurrentMap>

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


namespace Isis {
  ImagePointTreeModel::ImagePointTreeModel(ControlNet *controlNet,
      TreeView *v, QObject *parent) :
    AbstractTreeModel(controlNet, v, parent) {
    rebuildItems();
  }


  ImagePointTreeModel::~ImagePointTreeModel() {
  }


  ImagePointTreeModel::CreateRootItemFunctor::CreateRootItemFunctor(
    AbstractTreeModel *tm, ControlNet *net, QThread *tt) {
    m_treeModel = tm;
    m_controlNet = net;
    m_targetThread = tt;
    m_avgCharWidth = QFontMetrics(
        m_treeModel->getView()->getContentFont()).averageCharWidth();
  }


  ImagePointTreeModel::CreateRootItemFunctor::CreateRootItemFunctor(
    const CreateRootItemFunctor &other) {
    m_treeModel = other.m_treeModel;
    m_controlNet = other.m_controlNet;
    m_targetThread = other.m_targetThread;
    m_avgCharWidth = other.m_avgCharWidth;
  }


  ImagePointTreeModel::CreateRootItemFunctor::~CreateRootItemFunctor() {
    m_targetThread = NULL;
    m_controlNet = NULL;
    m_treeModel = NULL;
  }


  ImageParentItem *ImagePointTreeModel::CreateRootItemFunctor::operator()(
    QString imageSerial) const {
    ImageParentItem *imageItem = NULL;

    // TODO connect parent item destroy to image removed from network

    imageItem = new ImageParentItem(imageSerial, m_controlNet, m_avgCharWidth);
    imageItem->setSelectable(false);
    imageItem->moveToThread(m_targetThread);
    QList< ControlMeasure * > measures = m_controlNet->GetMeasuresInCube(imageSerial);
    for (int j = 0; j < measures.size(); j++) {
      ControlPoint *point = measures[j]->Parent();

      PointLeafItem *pointItem = new PointLeafItem(
        point, m_avgCharWidth, imageItem);
      pointItem->setSelectable(false);
      pointItem->moveToThread(m_targetThread);

      imageItem->addChild(pointItem);
    }

    return imageItem;
  }


  void ImagePointTreeModel::CreateRootItemFunctor::addToRootItem(
    QAtomicPointer< RootItem > & root, ImageParentItem *const &item) {

    // Allocate a new root item if our root is NULL
    if (root.testAndSetOrdered(NULL, new RootItem)) {
      root.loadAcquire()->moveToThread(item->thread());
    }

    if (item)
      root.loadAcquire()->addChild(item);
  }


  ImagePointTreeModel::CreateRootItemFunctor &
  ImagePointTreeModel::CreateRootItemFunctor::operator=(
    const CreateRootItemFunctor &other) {
    if (this != &other) {
      m_treeModel = other.m_treeModel;
      m_avgCharWidth = other.m_avgCharWidth;
    }

    return *this;
  }


  void ImagePointTreeModel::rebuildItems() {
    //     cerr << "ImagePointTreeModel::rebuildItems\n";
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
    //     cerr << "/ImagePointTreeModel::rebuildItems\n";
  }
}
