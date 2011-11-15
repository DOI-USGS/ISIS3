#include "IsisDebug.h"

#include <cmath>
#include <iostream>

#include <QDateTime>
#include <QFutureWatcher>
#include <QSettings>
#include <QtConcurrentRun>
#include <QTimer>

#include "iException.h"
#include "iString.h"

#include "AbstractTableDelegate.h"
#include "AbstractTableModel.h"
#include "BusyLeafItem.h"
#include "TableColumn.h"
#include "TableColumnList.h"
#include "TableView.h"
#include "AbstractTreeModel.h"


using std::cerr;


namespace Isis
{
  namespace CnetViz
  {
    AbstractTableModel::AbstractTableModel(AbstractTreeModel * model,
        AbstractTableDelegate * someDelegate)
    {
      nullify();

      dataModel = model;
      connect(model, SIGNAL(cancelSort()), this, SLOT(cancelSort()));
      
      delegate = someDelegate;
      
      setSortingEnabled(false);
      setSorting(false);
      
      sortedItems = new QList<AbstractTreeItem *>;
      busyItem = new BusyLeafItem;
      sortStatusPoller = new QTimer;
      
      sortingWatcher = new QFutureWatcher< QList< AbstractTreeItem * > >;
      connect(sortingWatcher, SIGNAL(finished()), this, SLOT(sortFinished()));
      
      connect(sortStatusPoller, SIGNAL(timeout()),
              this, SLOT(sortStatusUpdated()));
      
      // Signal forwarding
      connect(model, SIGNAL(modelModified()), SLOT(rebuildSort()));
//       connect(model, SIGNAL(modelModified()), this, SIGNAL(modelModified()));

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
    }


    AbstractTableModel::~AbstractTableModel()
    {
      cancelSort();
      
      dataModel = NULL;

      delete delegate;
      delegate = NULL;

      delete sortedItems;
      sortedItems = NULL;

      delete busyItem;
      busyItem = NULL;

      delete sortStatusPoller;
      sortStatusPoller = NULL;

      delete lessThanFunctor;
      lessThanFunctor = NULL;

      if (columns)
      {
        for (int i = 0; i < columns->size(); i++)
          delete (*columns)[i];

        delete columns;
        columns = NULL;
      }
      
      delete sortingWatcher;
      sortingWatcher = NULL;
    }


    bool AbstractTableModel::isSorting() const
    {
      return sorting;
    }


    bool AbstractTableModel::isFiltering() const
    {
      return dataModel && dataModel->isFiltering();
    }


    bool AbstractTableModel::sortingIsEnabled() const
    {
      return sortingEnabled;
    }


    void AbstractTableModel::setSortingEnabled(bool enabled)
    {
      if (sortingEnabled != enabled)
      {
        sortingEnabled = enabled;
        
        if (sortingEnabled)
          rebuildSort();
        else
          cancelSort();  // can safely be called when not sorting
          
        emit modelModified();
      }
    }


    TableColumnList * AbstractTableModel::getColumns()
    {
      if (!columns)
      {
        columns = createColumns();
        connect(columns, SIGNAL(sortOutDated()), this, SLOT(sort()));
      }

      return columns;
    }


    const AbstractTableDelegate * AbstractTableModel::getDelegate() const
    {
      return delegate;
    }
    
    
    void AbstractTableModel::applyFilter()
    {
      getDataModel()->applyFilter();
    }


    void AbstractTableModel::sort()
    {
      if (sortedItems->size() && !dataModel->isFiltering() &&
          !dataModel->isRebuilding())
      {
        if (isSorting())
        {
          cancelSort();
        }
//         else
        {
          // sorting is always done on a COPY of the items list
          QList< AbstractTreeItem * > copy = *sortedItems;
  
          QFuture< QList< AbstractTreeItem * > > future =
              QtConcurrent::run(this, &AbstractTableModel::doSort, copy);
          sortingWatcher->setFuture(future);
        }
      }
    }


    void AbstractTableModel::reverseOrder(TableColumn * column)
    {
    }


    void AbstractTableModel::updateSort()
    {
    }


    AbstractTreeModel * AbstractTableModel::getDataModel()
    {
      ASSERT(dataModel);
      return dataModel;
    }


    const AbstractTreeModel * AbstractTableModel::getDataModel() const
    {
      ASSERT(dataModel);
      return dataModel;
    }


    QList< AbstractTreeItem * > AbstractTableModel::getSortedItems(
        int start, int end, AbstractTreeModel::InterestingItems flags)
    {
      QList< AbstractTreeItem * > sortedSubsetOfItems;

      if (sortingIsEnabled())
      {
        while (start <= end)
        {
          if (start < sortedItems->size())
            sortedSubsetOfItems.append(sortedItems->at(start));
          else if (isFiltering())
            sortedSubsetOfItems.append(busyItem);

          start++;
        }
      }
      else
      {
        sortedSubsetOfItems = getDataModel()->getItems(start, end, flags, true);
      }
      
      return sortedSubsetOfItems;
    }
    
    
    QList< AbstractTreeItem * > AbstractTableModel::getSortedItems(
        AbstractTreeItem * item1, AbstractTreeItem * item2,
        AbstractTreeModel::InterestingItems flags)
    {
      QList< AbstractTreeItem * > sortedSubsetOfItems;

      if (!sortingIsEnabled())
      {
        sortedSubsetOfItems = getDataModel()->getItems(item1, item2, flags, true);
      }
      else
      {
        AbstractTreeItem * start = NULL;

        int currentIndex = 0;

        while (!start && currentIndex < sortedItems->size())
        {
          AbstractTreeItem * current = sortedItems->at(currentIndex);
          if (current == item1)
            start = item1;
          else if (current == item2)
            start = item2;

          if (!start)
            currentIndex++;
        }

        if (!start)
        {
          iString msg = "Could not find the first item";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
        }

        AbstractTreeItem * end = item2;
        
        // Sometimes we need to build the list forwards and sometimes backwards.
        // This is accomplished by using either append or prepend.  We abstract
        // away which of these we should use (why should we care) by using the
        // variable "someKindaPend" to store the appropriate method.
        void (QList< AbstractTreeItem * >::*someKindaPend)(
            AbstractTreeItem * const &);
        someKindaPend = &QList< AbstractTreeItem * >::append;
        
        if (start == item2)
        {
          end = item1;
          someKindaPend = &QList< AbstractTreeItem * >::prepend;
        }

        while (currentIndex < sortedItems->size() &&
            sortedItems->at(currentIndex) != end)
        {
          (sortedSubsetOfItems.*someKindaPend)(sortedItems->at(currentIndex));
          currentIndex++;
        }

        if (currentIndex >= sortedItems->size())
        {
          iString msg = "Could not find the second item";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
        }

        (sortedSubsetOfItems.*someKindaPend)(end);
      }

      return sortedSubsetOfItems;
    }
    
    
    void AbstractTableModel::handleTreeSelectionChanged(
        QList< AbstractTreeItem * > newlySelectedItems,
        AbstractTreeItem::InternalPointerType pointerType)
    {
      QList< AbstractTreeItem * > interestingSelectedItems;
      foreach (AbstractTreeItem * item, newlySelectedItems)
      {
        if (item->getPointerType() == pointerType)
          interestingSelectedItems.append(item);
      }

      if (interestingSelectedItems.size())
      {
        emit treeSelectionChanged(interestingSelectedItems);
      }
    }


    void AbstractTableModel::sortStatusUpdated()
    {
//       cerr << this << " -- AbstractTableModel::sortStatusUpdated() - "
//            << lessThanFunctor->getCompareCount() << "\n";
      if (lessThanFunctor)
        emit sortProgressChanged(lessThanFunctor->getCompareCount());
    }
    
    
    void AbstractTableModel::sortFinished()
    {
      QList< AbstractTreeItem * > newSortedItems = sortingWatcher->result();
      if (!dataModel->isFiltering() && !dataModel->isRebuilding())
      {
        if (newSortedItems.size())
        {
          *sortedItems = newSortedItems;
          emit modelModified();
        }
        else
        {
          sort();
        }
      }
    }

    
    void AbstractTableModel::cancelSort()
    {
      if (isSorting() && lessThanFunctor)
      {
        lessThanFunctor->interrupt();
        while (lessThanFunctor->interrupted());
      }
    }
    
    
    QList< AbstractTreeItem * > AbstractTableModel::doSort(
        QList< AbstractTreeItem * > itemsToSort)
    {
      ASSERT(!isSorting());
      if (!isSorting())
      {
        //     cerr << "AbstractTableModel::sort called\n";
        //ASSERT(!isSorting());
        setSorting(true);

        QList< TableColumn * > columnsToSortOn = columns->getSortingOrder();
        if (sortingIsEnabled())
        {
          // Create a new comparison functor to be used in the sort. It will
          // keep track of the number of comparisons made so that we can make a
          // guess at the progress of the sort.
          delete lessThanFunctor;
          lessThanFunctor = NULL;
          lessThanFunctor = new LessThanFunctor(columnsToSortOn.at(0));

          // Reset the timer so that it will begin polling the status of the
          // sort.
          sortStatusPoller->start(SORT_UPDATE_FREQUENCY);

          // Use n*log2(n) as our estimate of the number of comparisons that it
          // should take to sort the list.
          int numItems = itemsToSort.size();
//           cerr << this << " -- AbstractTableModel::doSort() about to perform"
//                << "a new sort!\nSorting " << numItems << " items. Expected "
//                << "number of comparisons: " << numItems * (int)log2(numItems)
//                << "\n";
          double a = 1.0;
          double b = 1.0;
          emit sortProgressRangeChanged(0,
              (int) ((a * numItems) * (log2(b * numItems))));

          try
          {
            qStableSort(itemsToSort.begin(), itemsToSort.end(), *lessThanFunctor);
          }
          catch (SortingCanceledException & e)
          {
            sortStatusPoller->stop();
            emit sortProgressRangeChanged(0, 0);
            emit sortProgressChanged(0);
            emit modelModified();
            
            setSorting(false);
            lessThanFunctor->reset();
            
            return QList< AbstractTreeItem * >();
          }
          
    //       for (int i = columnsToSortOn.size() - 1; i >= 0; i--)
    //         qStableSort(sortedItems->begin(), sortedItems->end(),
    //                     LessThanFunctor(columnsToSortOn.at(i)));

          // The sort is done, so stop emiting status updates and make sure we let
          // the listeners know that the sort is done (since the status will not
          // always reach 100% as we are estimating the progress).
          sortStatusPoller->stop();
          emit sortProgressRangeChanged(0, 0);
          emit sortProgressChanged(0);
          emit modelModified();
//           cerr << this << " -- AbstractTableModel::doSort() is done with the "
//                << "sort!\n";

        }
        
        setSorting(false);
        
        //     cerr << "AbstractTableModel::sort done\n";
      }
      
      return itemsToSort;
    }

  
    void AbstractTableModel::nullify()
    {
      dataModel = NULL;
      delegate = NULL;
      sortedItems = NULL;
      busyItem = NULL;
      sortStatusPoller = NULL;
      lessThanFunctor = NULL;
      columns = NULL;
      sortingWatcher = NULL;
      lessThanFunctor = NULL;
    }


    void AbstractTableModel::setSorting(bool isSorting)
    {
      sorting = isSorting;
    }
    
    
    void AbstractTableModel::rebuildSort()
    {
  //     cerr << "AbstractTableModel::rebuildSort called\n";
      ASSERT(dataModel);
      ASSERT(sortedItems);
      if (sortingEnabled)
      {
        sortingEnabled = false;
        *sortedItems = getItems(0, -1);
        sortingEnabled = true;
        sort();
      }
      else
      {
        emit modelModified();
      }
    }

  
    // *********** LessThanFunctor implementation *************


    AbstractTableModel::LessThanFunctor::LessThanFunctor(
        TableColumn const * someColumn) : column(someColumn)
    {
      sharedData = new LessThanFunctorData;
    }


    AbstractTableModel::LessThanFunctor::LessThanFunctor(
        LessThanFunctor const & other) : sharedData (other.sharedData)
    {
      column = other.column;
    }
    
    
    AbstractTableModel::LessThanFunctor::~LessThanFunctor()
    {
      column = NULL;
    }
    
    
    int AbstractTableModel::LessThanFunctor::getCompareCount() const
    {
      return sharedData->getCompareCount();
    }
    
    
    void AbstractTableModel::LessThanFunctor::interrupt()
    {
      sharedData->setInterrupted(true);
    }
    
    
    bool AbstractTableModel::LessThanFunctor::interrupted()
    {
      return sharedData->interrupted();
    }


    void AbstractTableModel::LessThanFunctor::reset()
    {
      sharedData->setInterrupted(false);
    }


    bool AbstractTableModel::LessThanFunctor::operator()(
        AbstractTreeItem * const & left, AbstractTreeItem * const & right)
    {
      if (left->getPointerType() != right->getPointerType())
      {
        iString msg = "Tried to compare apples to oranges";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }
      
      if (sharedData->interrupted())
      {
        throw SortingCanceledException();
      }
      
      sharedData->incrementCompareCount();
      //.fetchAndAddRelaxed(1);

      QVariant leftData = left->getData(column->getTitle());
      QVariant rightData = right->getData(column->getTitle());
      QString busy = BusyLeafItem().getData().toString();

      bool lessThan;
      if (leftData.type() == QVariant::String &&
          rightData.type() == QVariant::String)
      {
        lessThan = leftData.toString() < rightData.toString();
      }
      else if (leftData.type() == QVariant::Double &&
          rightData.type() == QVariant::Double)
      {
        lessThan = (leftData.toDouble() < rightData.toDouble());
      }
      else if (leftData.type() == QVariant::Double ||
          rightData.type() == QVariant::Double)
      {
        // We are comparing a BusyLeafItem to a double. BusyLeafItem's should
        // always be less than the double.
        lessThan = (leftData.toString() == busy);
      }
      else
      {
        lessThan = leftData.toString() < rightData.toString();
//         iString msg = "Could not determine the item's type while trying to "
//                       "compare it to another item";
//         throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }

      return lessThan ^ column->sortAscending();
    }


    AbstractTableModel::LessThanFunctor &
        AbstractTableModel::LessThanFunctor::operator=(
        LessThanFunctor const & other)
    {
      if (this != &other)
      {
        column = other.column;
        sharedData = other.sharedData;
      }

      return *this;
    }


    // *********** LessThanFunctorData implementation *************


    AbstractTableModel::LessThanFunctorData::LessThanFunctorData()
    {
      compareCount.fetchAndStoreRelaxed(0);
      interruptFlag.fetchAndStoreRelaxed(0);
    }


    AbstractTableModel::LessThanFunctorData::LessThanFunctorData(
        LessThanFunctorData const & other) : QSharedData(other),
        compareCount(other.compareCount), interruptFlag(other.interruptFlag)
    {
    }
    
    
    AbstractTableModel::LessThanFunctorData::~LessThanFunctorData()
    {
    }
    
    
    int AbstractTableModel::LessThanFunctorData::getCompareCount() const
    {
      return compareCount;
    }


    void AbstractTableModel::LessThanFunctorData::incrementCompareCount()
    {
      compareCount.fetchAndAddRelaxed(1);
    }
    
    
    void AbstractTableModel::LessThanFunctorData::setInterrupted(bool newStatus)
    {
      newStatus ? interruptFlag.fetchAndStoreRelaxed(1) : interruptFlag.fetchAndStoreRelaxed(0);
    }
    
    
    bool AbstractTableModel::LessThanFunctorData::interrupted()
    {
      return interruptFlag != 0;
    }
  }
}
