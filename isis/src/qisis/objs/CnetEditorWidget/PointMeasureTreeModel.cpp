/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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


namespace Isis {
  PointMeasureTreeModel::PointMeasureTreeModel(ControlNet *controlNet,
      TreeView *v, QObject *parent) :
    AbstractTreeModel(controlNet, v, parent) {
    rebuildItems();
  }


  PointMeasureTreeModel::~PointMeasureTreeModel() {
  }


  PointMeasureTreeModel::CreateRootItemFunctor::CreateRootItemFunctor(
    AbstractTreeModel *tm, QThread *tt) {
    m_treeModel = tm;
    m_targetThread = tt;
    m_avgCharWidth = QFontMetrics(
        m_treeModel->getView()->getContentFont()).averageCharWidth();
  }


  PointMeasureTreeModel::CreateRootItemFunctor::CreateRootItemFunctor(
    const CreateRootItemFunctor &other) {
    m_treeModel = other.m_treeModel;
    m_targetThread = other.m_targetThread;
    m_avgCharWidth = other.m_avgCharWidth;
  }

  PointMeasureTreeModel::CreateRootItemFunctor::~CreateRootItemFunctor() {
    m_treeModel = NULL;
  }


  PointParentItem *PointMeasureTreeModel::CreateRootItemFunctor::operator()(
    ControlPoint *const &point) const {
    PointParentItem *pointItem = new PointParentItem(point, m_avgCharWidth);
    pointItem->moveToThread(m_targetThread);

    for (int j = 0; j < point->GetNumMeasures(); j++) {
      const ControlMeasure *measure = point->GetMeasure(j);

      MeasureLeafItem *measureItem = new MeasureLeafItem(
        const_cast< ControlMeasure * >(measure), m_avgCharWidth, pointItem);
      measureItem->moveToThread(m_targetThread);

      pointItem->addChild(measureItem);
    }

    return pointItem;
  }


  void PointMeasureTreeModel::CreateRootItemFunctor::addToRootItem(
    QAtomicPointer< RootItem > & root, PointParentItem *const &item) {

    // Allocate a new root item if our root is NULL
    if (root.testAndSetOrdered(NULL, new RootItem)) {
      root.loadAcquire()->moveToThread(item->thread());
    }

    if (item)
      root.loadAcquire()->addChild(item);
  }


  PointMeasureTreeModel::CreateRootItemFunctor &
  PointMeasureTreeModel::CreateRootItemFunctor::operator=(
    const CreateRootItemFunctor &other) {
    if (this != &other) {
      m_treeModel = other.m_treeModel;
      m_avgCharWidth = other.m_avgCharWidth;
    }

    return *this;
  }

  void PointMeasureTreeModel::rebuildItems() {
    if (!isFrozen()) {
      emit cancelSort();
      setRebuilding(true);
      emit filterCountsChanged(-1, getTopLevelItemCount());
      QFuture< QAtomicPointer< RootItem > > futureRoot;
      if (getRebuildWatcher()->isStarted()) {
        futureRoot = getRebuildWatcher()->future();
        futureRoot.cancel();
      }

      futureRoot = QtConcurrent::mappedReduced(
          getControlNetwork()->GetPoints(),
          CreateRootItemFunctor(this, QThread::currentThread()),
          &CreateRootItemFunctor::addToRootItem,
          QtConcurrent::OrderedReduce | QtConcurrent::SequentialReduce);

      getRebuildWatcher()->setFuture(futureRoot);
    }
    else {
      queueRebuild();
    }
  }
}
