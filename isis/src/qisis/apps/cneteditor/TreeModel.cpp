#include "IsisDebug.h"

#include "TreeModel.h"

#include <algorithm>
#include <iostream>

#include <QFutureWatcher>
#include <QList>
#include <QModelIndex>
#include <QMutex>
#include <QPair>
#include <QStack>
#include <QString>
#include <QtConcurrentFilter>

#include <QtConcurrentMap>

#include <QFlags>
#include <QtGlobal>
#include <QVariant>

#include "BusyLeafItem.h"
#include "CnetTreeView.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "iException.h"

#include "AbstractTreeItem.h"
#include "FilterWidget.h"
#include "RootItem.h"

using std::cerr;


namespace Isis
{
  TreeModel::TreeModel(ControlNet * controlNet, CnetTreeView * v,
      QObject * parent) : QObject(parent), view(v), cNet(controlNet)
  {
    ASSERT(cNet);

    filterWatcher = NULL;
    rebuildWatcher = NULL;
    busyItem = NULL;
    rootItem = NULL;
    expandedState = NULL;
    selectedState = NULL;
    guisFilterWidget = NULL;
    localFilterWidgetCopy = NULL;
    mutex = NULL;

    busyItem = new BusyLeafItem(NULL);
    rootItem = new RootItem;
    expandedState = new QList< QPair< QString, QString > >;
    selectedState = new QList< QPair< QString, QString > >;
    mutex = new QMutex;

    filterWatcher = new QFutureWatcher< QAtomicPointer< AbstractTreeItem > >;
    rebuildWatcher = new QFutureWatcher< QAtomicPointer< RootItem > >;

    connect(filterWatcher, SIGNAL(finished()), this, SLOT(applyFilterDone()));
    connect(rebuildWatcher, SIGNAL(finished()), this, SLOT(rebuildItemsDone()));

    connect(filterWatcher, SIGNAL(progressValueChanged(int)),
        this, SIGNAL(filterProgressChanged(int)));
    connect(filterWatcher, SIGNAL(progressRangeChanged(int, int)),
        this, SIGNAL(filterProgressRangeChanged(int, int)));
    connect(rebuildWatcher, SIGNAL(progressValueChanged(int)),
        this, SIGNAL(rebuildProgressChanged(int)));
    connect(rebuildWatcher, SIGNAL(progressRangeChanged(int, int)),
        this, SIGNAL(rebuildProgressRangeChanged(int, int)));

    drivable = false;
    filterAgain = false;
    filterRunning = false;
  }


  TreeModel::~TreeModel()
  {
    delete filterWatcher;
    filterWatcher = NULL;

    delete rebuildWatcher;
    rebuildWatcher = NULL;

    delete busyItem;
    busyItem = NULL;

    delete rootItem;
    rootItem = NULL;

    delete expandedState;
    expandedState = NULL;

    delete selectedState;
    selectedState = NULL;

    delete mutex;
    mutex = NULL;

    delete localFilterWidgetCopy;
    localFilterWidgetCopy = NULL;

    guisFilterWidget = NULL;
    cNet = NULL;
    view = NULL;
  }


  QList< AbstractTreeItem * > TreeModel::getItems(int start, int end) const
  {
    QList< AbstractTreeItem * > foundItems;
    int rowCount = end - start;
    const AbstractTreeItem * lastVisibleFilteredItem =
        rootItem->getLastVisibleFilteredItem();

    if (lastVisibleFilteredItem && rowCount > 0 && rootItem->childCount())
    {
      int row = 0;
      AbstractTreeItem * currentItem = rootItem->getFirstVisibleChild();

      bool listStillValid = true;

      while (row < start && listStillValid && currentItem)
      {
        row++;
        listStillValid = (currentItem != lastVisibleFilteredItem ||
            currentItem == currentItem->parent()->getLastVisibleChild());

        if (listStillValid)
          currentItem = nextItem(currentItem);
      }

      while (row < end && listStillValid && currentItem)
      {
        ASSERT(currentItem);
        foundItems.append(currentItem);
        listStillValid = (currentItem != lastVisibleFilteredItem ||
            currentItem == currentItem->parent()->getLastVisibleChild());
        row++;

        if (listStillValid)
          currentItem = nextItem(currentItem);
      }

      while (isFiltering() && foundItems.size() < rowCount)
      {
        foundItems.append(busyItem);
      }
    }

    return foundItems;
  }


  QList< AbstractTreeItem * > TreeModel::getItems(
    AbstractTreeItem * item1, AbstractTreeItem * item2) const
  {
    QList< AbstractTreeItem * > foundItems;

    if (rootItem->childCount() && item1 != item2)
    {
      AbstractTreeItem * start = NULL;

      AbstractTreeItem * curItem = rootItem->getFirstVisibleChild();

      while (!start && curItem)
      {
        if (curItem == item1)
          start = item1;
        else
          if (curItem == item2)
            start = item2;

        curItem = nextItem(curItem);
      }

      if (!start)
      {
        iString msg = "The first item passed to getItems(AbstractTreeItem*, "
            "AbstractTreeItem*) does not exist in this model's tree";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }

      foundItems.append(start);

      AbstractTreeItem * end = item2;

      if (start == item2)
        end = item1;

      while (curItem && curItem != end)
      {
        foundItems.append(curItem);
        curItem = nextItem(curItem);
      }

      if (!curItem)
      {
        iString msg = "The second item passed to getItems(AbstractTreeItem*, "
            "AbstractTreeItem*) does not exist in this model's tree";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }

      foundItems.append(end);
    }

    return foundItems;
  }


  QMutex * TreeModel::getMutex() const
  {
    return mutex;
  }


  QList< AbstractTreeItem * > TreeModel::getSelectedItems() const
  {
    QList< AbstractTreeItem * > selectedItems;

    ASSERT(rootItem);

    if (!isFiltering())
    {
      AbstractTreeItem * currentItem = rootItem->getFirstVisibleChild();

      while (currentItem)
      {
        if (currentItem->isSelected())
          selectedItems.append(currentItem);

        currentItem = nextItem(currentItem);
      }
    }

    return selectedItems;
  }


  int TreeModel::getTopLevelItemCount() const
  {
    return rootItem->childCount();
  }


  int TreeModel::getVisibleTopLevelItemCount() const
  {
    int visiblePeerCount = -1;

    if (!isFiltering())
    {
      AbstractTreeItem * current = rootItem->getFirstVisibleChild();
      while (current)
      {
        current = current->getNextVisiblePeer();
        visiblePeerCount++;
      }

      // started at -1 so we were one off.
      visiblePeerCount++;
    }

    return visiblePeerCount;
  }


  void TreeModel::setDrivable(bool drivableStatus)
  {
    if (drivable != drivableStatus)
    {
      drivable = drivableStatus;
      ASSERT(view);

      if (view)
        view->activate();
    }
  }


  bool TreeModel::isDrivable() const
  {
    return drivable;
  }


  bool TreeModel::isFiltering() const
  {
    return filterRunning;
  }


  void TreeModel::setFilter(FilterWidget * fw)
  {
    guisFilterWidget = fw;
    if (fw)
    {
      connect(guisFilterWidget, SIGNAL(filterChanged()),
          this, SLOT(applyFilter()));
      applyFilter();
    }
  }


  void TreeModel::clear()
  {
    ASSERT(rootItem);

    delete rootItem;
    rootItem = NULL;
    rootItem = new RootItem;
  }


  ControlNet * TreeModel::getControlNetwork() const
  {
    return cNet;
  }


//   FilterWidget * TreeModel::getFilterWidget() const
//   {
//     return filter;
//   }


  QFutureWatcher< QAtomicPointer< RootItem > > *
  TreeModel::getRebuildWatcher() const
  {
    return rebuildWatcher;
  }


  RootItem * TreeModel::getRootItem() const
  {
    return rootItem;
  }


  CnetTreeView * TreeModel::getView() const
  {
    return view;
  }


  void TreeModel::saveViewState()
  {
//     cerr << "    TreeModel::saveViewState called\n";
    expandedState->clear();
    selectedState->clear();

    ASSERT(rootItem);

    QStack< AbstractTreeItem * > stack;
    stack.push(rootItem);

    while (!stack.isEmpty())
    {
      AbstractTreeItem * item = stack.pop();

      QPair< QString, QString > newPair = qMakePair(item->getData(),
          item->parent() ? item->parent()->getData() : QString());

      if (item->isExpanded())
      {
//         cerr << "      [" << qPrintable(newPair.first) << "]\t["
//             << qPrintable(newPair.second) << "]\n";

        expandedState->append(newPair);
      }

      if (item->isSelected())
      {
//         cerr << "      [" << qPrintable(newPair.first) << "]\t["
//             << qPrintable(newPair.second) << "]\n";
        selectedState->append(newPair);
      }

      for (int i = item->childCount() - 1; i >= 0; i--)
        stack.push(item->childAt(i));
    }

//     cerr << "    TreeModel::saveViewState done\n";
  }


  void TreeModel::loadViewState()
  {
//     cerr << "    TreeModel::loadViewState called... " << expandedState->size() << "\n";
//     for (int i = 0; i < selectedState->size(); i++)
//       cerr << "      [" << qPrintable(selectedState->at(i).first) << "]\t["
//           << qPrintable(selectedState->at(i).second) << "]\n";

//     view->setUpdatesEnabled(false);

    ASSERT(rootItem);

    QStack< AbstractTreeItem * > stack;
    stack.push(rootItem);

    while (!stack.isEmpty() && (expandedState->size() || selectedState->size()))
    {
      AbstractTreeItem * item = stack.pop();

      if (item->parent())
      {
//         int row = item->row();
//         ASSERT(row != -1);

        QPair< QString, QString > occurrence = qMakePair(
            item->getData(), item->parent()->getData());

//         QModelIndex index = createIndex(row, 0, item);

        if (expandedState->contains(occurrence))
        {

//           cerr << "      occurrence: [" << qPrintable(occurrence.first) << "]\t["
//               << qPrintable(occurrence.second) << "]\n";

          item->setExpanded(true);
//           ASSERT(index.isValid());
//           view->expand(index);
//           expandedState->removeOne(occurrence);
        }


        if (selectedState->contains(occurrence))
        {
          item->setSelected(true);
//           view->selectionModel()->select(index, QItemSelectionModel::Select);
          //         selectedState->removeOne(occurrence);
        }
      }

      for (int i = item->childCount() - 1; i >= 0; i--)
        stack.push(item->childAt(i));
    }

//     view->setUpdatesEnabled(true);
//     emit(dataChanged(QModelIndex(), QModelIndex()));
//     cerr << "    TreeModel::loadViewState done... " << expandedState->size() << "\n";
  }


//! indentation is in pixels
  QSize TreeModel::getVisibleSize(int indentation) const
  {
    QSize size;

    if (!isFiltering())
    {
      int visibleRowCount = 0;
      int maxWidth = 0;

      if (rootItem && rootItem->getFirstVisibleChild())
      {
        AbstractTreeItem * current = rootItem->getFirstVisibleChild();

        while (current != NULL)
        {
          int depth = current->getDepth();

          visibleRowCount++;
          maxWidth = qMax(maxWidth, current->getDataWidth() + indentation * depth);
          current = nextItem(current);
        }
      }

      size = QSize(maxWidth, visibleRowCount);
    }

    return size;
  }


  void TreeModel::applyFilter()
  {
    // If filterAgain is true, then this method will be recalled later
    // with filterAgain = false.
    if (!filterAgain && guisFilterWidget)
    {
      QFuture< QAtomicPointer< AbstractTreeItem> > futureRoot;

      if (filterRunning)
      {
        filterAgain = true;
        futureRoot = filterWatcher->future();
        futureRoot.cancel();
      }
      else
      {
        // filterCounts are unknown and invalid and this fact is shared to
        // users of this class by emitting invalid (negative) information.
        emit filterCountsChanged(-1, getTopLevelItemCount());

        // update our local copy of the gui widget
        if (localFilterWidgetCopy)
        {
          delete localFilterWidgetCopy;
          localFilterWidgetCopy = NULL;
        }
        localFilterWidgetCopy = new FilterWidget(*guisFilterWidget);

        // using the local copy (NOT the GUI's FilterWidget!!!) apply then
        // the filter using qtconcurrent's filteredReduced.  ApplyFilterDone()
        // will get called when the filtering is finished.
        filterRunning = true;
        rootItem->setLastVisibleFilteredItem(NULL);
        futureRoot = QtConcurrent::filteredReduced(rootItem->getChildren(),
            FilterFunctor(localFilterWidgetCopy),
            &FilterFunctor::updateTopLevelLinks,
            QtConcurrent::OrderedReduce | QtConcurrent::SequentialReduce);

        filterWatcher->setFuture(futureRoot);
      }
    }
  }


  void TreeModel::setGlobalSelection(bool selected)
  {
    selectItems(rootItem, selected);
  }


  void TreeModel::selectItems(AbstractTreeItem * item, bool selected)
  {
    if (item)
    {
      item->setSelected(selected);

      if (item->childCount())
      {
        foreach(AbstractTreeItem * childItem, item->getChildren())
        {
          selectItems(childItem, selected);
        }
      }
    }
  }


  AbstractTreeItem * TreeModel::nextItem(AbstractTreeItem * current,
      InterestingItemsFlag flags) const
  {
    AbstractTreeItem * result = NULL;

    if (current)
    {
      if (current->isExpanded() && current->getFirstVisibleChild())
      {
        result = current->getFirstVisibleChild();
      }
      else
      {
        result = current->getNextVisiblePeer();

        if (!result)
          result = current->parent()->getNextVisiblePeer();
      }
    }
    
    if (result)
    {
      if (!flags.testFlag(AllItems))
      {
        AbstractTreeItem::InternalPointerType pointerType;
        pointerType = result->getPointerType();
        
        if ((pointerType == AbstractTreeItem::Point &&
            !flags.testFlag(PointItems)) ||
            (pointerType == AbstractTreeItem::Measure &&
            !flags.testFlag(MeasureItems)) ||
            (pointerType == AbstractTreeItem::CubeGraphNode &&
            !flags.testFlag(SerialItems)))
          result = nextItem(result, flags);
      }
    }

    return result;
  }


  void TreeModel::applyFilterDone()
  {
    filterRunning = false;

    //dudeWheresMyCar(rootItem, 0);

    if (filterAgain)
    {
      filterAgain = false;
      applyFilter();
    }
    else
    {
      emit modelModified();
      emit filterCountsChanged(getVisibleTopLevelItemCount(),
          getTopLevelItemCount());
    }
  }


  void TreeModel::rebuildItemsDone()
  {
    saveViewState();
    clear();
    QAtomicPointer< RootItem > newRoot = rebuildWatcher->future();

    if (newRoot && newRoot->childCount())
    {
      ASSERT(rootItem);
      delete rootItem;
      rootItem = NULL;
      rootItem = newRoot;
    }
    else
    {
      delete newRoot;
      newRoot = NULL;
    }

//     loadViewState();

    applyFilter();
  }


  TreeModel::FilterFunctor::FilterFunctor(FilterWidget * fw) : filter(fw)
  {
  }


  TreeModel::FilterFunctor::~FilterFunctor()
  {
  }


  bool TreeModel::FilterFunctor::operator()(
    AbstractTreeItem * const & item) const
  {
    filterWorker(item);
    return true;
  }


  void TreeModel::FilterFunctor::filterWorker(
    AbstractTreeItem * item) const
  {
    switch (item->getPointerType())
    {

      case AbstractTreeItem::Point:
        item->setVisible((!filter || filter->evaluate(
            (ControlPoint *) item->getPointer())) ? true : false);
        break;

      case AbstractTreeItem::Measure:
        item->setVisible((!filter || filter->evaluate(
            (ControlMeasure *) item->getPointer())) ? true : false);
        break;

      case AbstractTreeItem::CubeGraphNode:
        item->setVisible((!filter || filter->evaluate(
            (ControlCubeGraphNode *) item->getPointer())) ? true : false);
        break;

      case AbstractTreeItem::None:
        item->setVisible(true);
        break;
    }

    // Destroy peer link because it will need to be recreated later.
    if (item->getFirstVisibleChild())
      item->setFirstVisibleChild(NULL);

    if (item->getLastVisibleChild())
      item->setLastVisibleChild(NULL);

    item->setNextVisiblePeer(NULL);

    // Update each tree item's visible flag based on whether or not it is
    // accepted by the filter.
    if (item->childCount())
    {
      for (int i = 0; i < item->childCount(); i++)
      {
        AbstractTreeItem * child = item->childAt(i);
        filterWorker(child);

        if (child->isVisible())
        {
          if (!item->getFirstVisibleChild())
          {
            item->setFirstVisibleChild(child);
            item->setLastVisibleChild(child);
          }
          else
          {
            item->getLastVisibleChild()->setNextVisiblePeer(child);
            item->setLastVisibleChild(child);
          }
        }
      }
    }
  }


  void TreeModel::FilterFunctor::updateTopLevelLinks(
    QAtomicPointer< AbstractTreeItem > & root,
    AbstractTreeItem * const & item)
  {
    if (!root)
    {
      root = item->parent();
      root->setFirstVisibleChild(NULL);
      root->setLastVisibleChild(NULL);
      root->setLastVisibleFilteredItem(NULL);
    }

    if (item->isVisible())
    {
      if (!root->getFirstVisibleChild())
      {
        root->setFirstVisibleChild(item);
        root->setLastVisibleChild(item);
      }
      else
      {
        root->getLastVisibleChild()->setNextVisiblePeer(item);
        root->setLastVisibleChild(item);
      }

      root->setLastVisibleFilteredItem(item);
    }
  }
}

