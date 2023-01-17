/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <cmath>
#include <iostream>

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QFutureWatcher>
#include <QSettings>
#include <QtConcurrentRun>
#include <QTimer>

#include "IException.h"
#include "IString.h"

#include "AbstractTableDelegate.h"
#include "AbstractTableModel.h"
#include "BusyLeafItem.h"
#include "TableColumn.h"
#include "TableColumnList.h"
#include "TableView.h"
#include "AbstractTreeModel.h"


namespace Isis {
  AbstractTableModel::AbstractTableModel(AbstractTreeModel *model,
      AbstractTableDelegate *someDelegate) {
    nullify();

    m_dataModel = model;
    connect(model, SIGNAL(cancelSort()), this, SLOT(cancelSort()));

    m_delegate = someDelegate;

    m_sortingEnabled = false;
    m_sortLimit = 10000;
    m_sorting = false;

    m_sortedItems = new QList<AbstractTreeItem *>;
    m_busyItem = new BusyLeafItem;
    m_sortStatusPoller = new QTimer;

    m_sortingWatcher = new QFutureWatcher< QList< AbstractTreeItem * > >;
    connect(m_sortingWatcher, SIGNAL(finished()), this, SLOT(sortFinished()));

    connect(m_sortStatusPoller, SIGNAL(timeout()),
        this, SLOT(sortStatusUpdated()));

    // Signal forwarding
    connect(model, SIGNAL(modelModified()), SLOT(rebuildSort()));

    connect(model, SIGNAL(filterProgressChanged(int)),
        this, SIGNAL(filterProgressChanged(int)));

    connect(model, SIGNAL(rebuildProgressChanged(int)),
        this, SIGNAL(rebuildProgressChanged(int)));

    connect(model, SIGNAL(filterProgressRangeChanged(int, int)),
        this, SIGNAL(filterProgressRangeChanged(int, int)));

    connect(model, SIGNAL(rebuildProgressRangeChanged(int, int)),
        this, SIGNAL(rebuildProgressRangeChanged(int, int)));

    connect(this, SIGNAL(tableSelectionChanged(QList<AbstractTreeItem *>)),
        model, SIGNAL(tableSelectionChanged(QList<AbstractTreeItem *>)));
    connect(model, SIGNAL(filterCountsChanged(int, int)),
        this, SIGNAL(filterCountsChanged(int, int)));
  }


  AbstractTableModel::~AbstractTableModel() {
    cancelSort();

    m_dataModel = NULL;

    delete m_delegate;
    m_delegate = NULL;

    delete m_sortedItems;
    m_sortedItems = NULL;

    delete m_busyItem;
    m_busyItem = NULL;

    delete m_sortStatusPoller;
    m_sortStatusPoller = NULL;

    delete m_lessThanFunctor;
    m_lessThanFunctor = NULL;

    if (m_columns) {
      for (int i = 0; i < m_columns->size(); i++)
        delete(*m_columns)[i];

      delete m_columns;
      m_columns = NULL;
    }

    delete m_sortingWatcher;
    m_sortingWatcher = NULL;
  }


  bool AbstractTableModel::isSorting() const {
    return m_sorting;
  }


  bool AbstractTableModel::isFiltering() const {
    return m_dataModel && m_dataModel->isFiltering();
  }


  bool AbstractTableModel::sortingIsEnabled() const {
    return m_sortingEnabled;
  }


  void AbstractTableModel::setSortingEnabled(bool enabled) {
    if (m_sortingEnabled != enabled) {
      m_sortingEnabled = enabled;
      rebuildSort();
    }
  }


  int AbstractTableModel::sortLimit() const {
    return m_sortLimit;
  }


  void AbstractTableModel::setSortLimit(int limit) {
    if (m_sortLimit != limit) {
      m_sortLimit = limit;
      rebuildSort();
    }
  }


  bool AbstractTableModel::sortingOn() const {
    return (sortingIsEnabled() && (getVisibleRowCount() <= sortLimit()));
  }


  TableColumnList *AbstractTableModel::getColumns() {
    if (!m_columns) {
      m_columns = createColumns();
      connect(m_columns, SIGNAL(sortOutDated()), this, SLOT(sort()));
    }

    return m_columns;
  }


  const AbstractTableDelegate *AbstractTableModel::getDelegate() const {
    return m_delegate;
  }


  void AbstractTableModel::applyFilter() {
    getDataModel()->applyFilter();
  }


  void AbstractTableModel::sort() {
    if (sortingOn() && m_sortedItems->size() && !m_dataModel->isFiltering() &&
        !m_dataModel->isRebuilding()) {
      if (isSorting()) {
        cancelSort();
      }
      else if (!m_lessThanFunctor) {
        // Create a new comparison functor to be used in the m_sort. It will
        // keep track of the number of comparisons made so that we can make a
        // guess at the progress of the m_sort.
        m_lessThanFunctor = new LessThanFunctor(
          m_columns->getSortingOrder().first());

        // Sorting is always done on a COPY of the items list.
        QFuture< QList< AbstractTreeItem * > > future =
          QtConcurrent::run(this, &AbstractTableModel::doSort,
              *m_sortedItems);
        m_sortingWatcher->setFuture(future);

        emit modelModified();
      }
    }
  }


  void AbstractTableModel::reverseOrder(TableColumn *column) {
  }


  void AbstractTableModel::updateSort() {
  }


  AbstractTreeModel *AbstractTableModel::getDataModel() {
    return m_dataModel;
  }


  const AbstractTreeModel *AbstractTableModel::getDataModel() const {
    return m_dataModel;
  }


  QList< AbstractTreeItem * > AbstractTableModel::getSortedItems(
    int start, int end, AbstractTreeModel::InterestingItems flags) {
    QList< AbstractTreeItem * > sortedSubsetOfItems;

    if (sortingOn()) {
      while (start <= end) {
        if (start < m_sortedItems->size())
          sortedSubsetOfItems.append(m_sortedItems->at(start));
        else if (isFiltering())
          sortedSubsetOfItems.append(m_busyItem);

        start++;
      }
    }
    else {
      sortedSubsetOfItems = getDataModel()->getItems(start, end, flags, true);
    }

    return sortedSubsetOfItems;
  }


  QList< AbstractTreeItem * > AbstractTableModel::getSortedItems(
    AbstractTreeItem *item1, AbstractTreeItem *item2,
    AbstractTreeModel::InterestingItems flags) {
    QList< AbstractTreeItem * > sortedSubsetOfItems;

    if (!sortingOn()) {
      sortedSubsetOfItems = getDataModel()->getItems(item1, item2, flags, true);
    }
    else {
      AbstractTreeItem *start = NULL;

      int currentIndex = 0;

      while (!start && currentIndex < m_sortedItems->size()) {
        AbstractTreeItem *current = m_sortedItems->at(currentIndex);
        if (current == item1)
          start = item1;
        else if (current == item2)
          start = item2;

        if (!start)
          currentIndex++;
      }

      if (!start) {
        IString msg = "Could not find the first item";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }

      AbstractTreeItem *end = item2;

      // Sometimes we need to build the list forwards and sometimes backwards.
      // This is accomplished by using either append or prepend.  We abstract
      // away which of these we should use (why should we care) by using the
      // variable "someKindaPend" to store the appropriate method.
      void (QList< AbstractTreeItem * >::*someKindaPend)(
        AbstractTreeItem * const &);
      someKindaPend = &QList< AbstractTreeItem * >::append;

      if (start == item2) {
        end = item1;
        someKindaPend = &QList< AbstractTreeItem * >::prepend;
      }

      while (currentIndex < m_sortedItems->size() &&
          m_sortedItems->at(currentIndex) != end) {
        (sortedSubsetOfItems.*someKindaPend)(m_sortedItems->at(currentIndex));
        currentIndex++;
      }

      if (currentIndex >= m_sortedItems->size()) {
        IString msg = "Could not find the second item";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }

      (sortedSubsetOfItems.*someKindaPend)(end);
    }

    return sortedSubsetOfItems;
  }


  void AbstractTableModel::handleTreeSelectionChanged(
    QList< AbstractTreeItem * > newlySelectedItems,
    AbstractTreeItem::InternalPointerType pointerType) {
    QList< AbstractTreeItem * > interestingSelectedItems;
    foreach (AbstractTreeItem * item, newlySelectedItems) {
      if (item->getPointerType() == pointerType)
        interestingSelectedItems.append(item);
    }

    if (interestingSelectedItems.size()) {
      emit treeSelectionChanged(interestingSelectedItems);
    }
  }


  void AbstractTableModel::sortStatusUpdated() {
    if (m_lessThanFunctor)
      emit sortProgressChanged(m_lessThanFunctor->getCompareCount());
  }


  void AbstractTableModel::sortFinished() {
    bool interrupted = m_lessThanFunctor->interrupted();
    delete m_lessThanFunctor;
    m_lessThanFunctor = NULL;

    if (!interrupted) {
      QList< AbstractTreeItem * > newSortedItems = m_sortingWatcher->result();

      if (!m_dataModel->isFiltering() && !m_dataModel->isRebuilding()) {
        *m_sortedItems = newSortedItems;
        emit modelModified();
      }
    }
    else {
      sort();
    }
  }


  void AbstractTableModel::cancelSort() {
    if (m_lessThanFunctor) {
      m_lessThanFunctor->interrupt();
      m_sortingWatcher->waitForFinished();
    }
  }


  void AbstractTableModel::itemsLost() {
    cancelSort();
    m_sortedItems->clear();
  }


  QList< AbstractTreeItem * > AbstractTableModel::doSort(
    QList< AbstractTreeItem * > itemsToSort) {
    if (!isSorting()) {
      setSorting(true);

      QList< TableColumn * > columnsToSortOn = m_columns->getSortingOrder();
      if (sortingOn()) {
        // Reset the timer so that it will begin polling the status of the
        // m_sort.
        m_sortStatusPoller->start(SORT_UPDATE_FREQUENCY);

        // Use n*log2(n) as our estimate of the number of comparisons that it
        // should take to m_sort the list.
        int numItems = itemsToSort.size();
        double a = 1.0;
        double b = 1.0;
        emit sortProgressRangeChanged(0,
            (int)((a * numItems) * (log2(b * numItems))));

        try {
          qStableSort(itemsToSort.begin(), itemsToSort.end(),
              *m_lessThanFunctor);
        }
        catch (SortingCanceledException &e) {
          m_sortStatusPoller->stop();
          emit sortProgressRangeChanged(0, 0);
          emit sortProgressChanged(0);
          emit modelModified();

          setSorting(false);
          return QList< AbstractTreeItem * >();
        }

        // The m_sort is done, so stop emiting status updates and make sure we
        // let the listeners know that the m_sort is done (since the status
        // will not always reach 100% as we are estimating the progress).
        m_sortStatusPoller->stop();
        emit sortProgressRangeChanged(0, 0);
        emit sortProgressChanged(0);
        emit modelModified();
      }

      setSorting(false);
    }

    return itemsToSort;
  }


  void AbstractTableModel::nullify() {
    m_dataModel = NULL;
    m_delegate = NULL;
    m_sortedItems = NULL;
    m_busyItem = NULL;
    m_sortStatusPoller = NULL;
    m_lessThanFunctor = NULL;
    m_columns = NULL;
    m_sortingWatcher = NULL;
  }


  void AbstractTableModel::setSorting(bool isSorting) {
    m_sorting = isSorting;
  }


  void AbstractTableModel::rebuildSort() {
    m_sortedItems->clear();
    cancelSort();

    if (sortingOn()) {
      m_sortingEnabled = false;
      *m_sortedItems = getItems(0, -1);

      foreach (AbstractTreeItem * item, *m_sortedItems) {
        connect(item, SIGNAL(destroyed(QObject *)), this, SLOT(itemsLost()));
      }

      m_sortingEnabled = true;
      sort();

      emit userWarning(None);
    }
    else {
      cancelSort();
      emit modelModified();

      if (!m_sortingEnabled)
        emit userWarning(SortingDisabled);
      else
        emit userWarning(SortingTableSizeLimitReached);
    }
  }


  // *********** LessThanFunctor implementation *************


  AbstractTableModel::LessThanFunctor::LessThanFunctor(
    TableColumn const *someColumn) : m_column(someColumn) {
    m_sharedData = new LessThanFunctorData;
  }


  AbstractTableModel::LessThanFunctor::LessThanFunctor(
    LessThanFunctor const &other) : m_sharedData(other.m_sharedData) {
    m_column = other.m_column;
  }


  AbstractTableModel::LessThanFunctor::~LessThanFunctor() {
    m_column = NULL;
  }


  int AbstractTableModel::LessThanFunctor::getCompareCount() const {
    return m_sharedData->getCompareCount();
  }


  void AbstractTableModel::LessThanFunctor::interrupt() {
    m_sharedData->setInterrupted(true);
  }


  bool AbstractTableModel::LessThanFunctor::interrupted() {
    return m_sharedData->interrupted();
  }


  void AbstractTableModel::LessThanFunctor::reset() {
    m_sharedData->setInterrupted(false);
  }


  bool AbstractTableModel::LessThanFunctor::operator()(
    AbstractTreeItem *const &left, AbstractTreeItem *const &right) {
    if (left->getPointerType() != right->getPointerType()) {
      IString msg = "Tried to compare apples to oranges";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (m_sharedData->interrupted()) {
      throw SortingCanceledException();
    }

    m_sharedData->incrementCompareCount();

    QVariant leftData = left->getData(m_column->getTitle());
    QVariant rightData = right->getData(m_column->getTitle());
    QString busy = BusyLeafItem().getData().toString();

    bool lessThan;
    if (leftData.type() == QVariant::String &&
        rightData.type() == QVariant::String) {
      lessThan = leftData.toString() < rightData.toString();
    }
    else if (leftData.type() == QVariant::Double &&
        rightData.type() == QVariant::Double) {
      lessThan = (leftData.toDouble() < rightData.toDouble());
    }
    else if (leftData.type() == QVariant::Double ||
        rightData.type() == QVariant::Double) {
      // We are comparing a BusyLeafItem to a double. BusyLeafItem's should
      // always be less than the double.
      lessThan = (leftData.toString() == busy);
    }
    else {
      lessThan = leftData.toString() < rightData.toString();
    }

    return lessThan ^ m_column->sortAscending();
  }


  AbstractTableModel::LessThanFunctor &
  AbstractTableModel::LessThanFunctor::operator=(
    LessThanFunctor const &other) {
    if (this != &other) {
      m_column = other.m_column;
      m_sharedData = other.m_sharedData;
    }

    return *this;
  }


  // *********** LessThanFunctorData implementation *************


  AbstractTableModel::LessThanFunctorData::LessThanFunctorData() {
    m_compareCount.fetchAndStoreRelaxed(0);
    m_interruptFlag.fetchAndStoreRelaxed(0);
  }


  AbstractTableModel::LessThanFunctorData::LessThanFunctorData(
    LessThanFunctorData const &other) : QSharedData(other),
    m_compareCount(other.m_compareCount), m_interruptFlag(other.m_interruptFlag) {
  }


  AbstractTableModel::LessThanFunctorData::~LessThanFunctorData() {
  }


  int AbstractTableModel::LessThanFunctorData::getCompareCount() const {
    return m_compareCount;
  }


  void AbstractTableModel::LessThanFunctorData::incrementCompareCount() {
    m_compareCount.fetchAndAddRelaxed(1);
  }


  void AbstractTableModel::LessThanFunctorData::setInterrupted(bool newStatus) {
    newStatus ? m_interruptFlag.fetchAndStoreRelaxed(1) :
    m_interruptFlag.fetchAndStoreRelaxed(0);
  }


  bool AbstractTableModel::LessThanFunctorData::interrupted() {
    return m_interruptFlag != 0;
  }
}
