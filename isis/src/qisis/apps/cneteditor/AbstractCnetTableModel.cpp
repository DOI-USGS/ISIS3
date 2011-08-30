#include "IsisDebug.h"

#include <iostream>

#include <QDateTime>
#include <QSettings>

#include "iException.h"
#include "iString.h"

#include "AbstractCnetTableDelegate.h"
#include "AbstractCnetTableModel.h"
#include "BusyLeafItem.h"
#include "CnetTableColumn.h"
#include "CnetTableColumnList.h"
#include "CnetTableView.h"
#include "TreeModel.h"


using std::cerr;


namespace Isis
{
  AbstractCnetTableModel::AbstractCnetTableModel(TreeModel * model,
      AbstractCnetTableDelegate * someDelegate)
  {
    nullify();

    dataModel = model;
    delegate = someDelegate;
    sortedItems = new QList<AbstractTreeItem *>;
    busyItem = new BusyLeafItem;
    sortingEnabled = false;
    
    // Signal forwarding
    connect(model, SIGNAL(modelModified()), SLOT(rebuildSort()));
    connect(model, SIGNAL(modelModified()), this, SIGNAL(modelModified()));

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


  AbstractCnetTableModel::~AbstractCnetTableModel()
  {
    dataModel = NULL;

    delete delegate;
    delegate = NULL;

    delete sortedItems;
    sortedItems = NULL;

    delete busyItem;
    busyItem = NULL;

    if (columns)
    {
      for (int i = 0; i < columns->size(); i++)
        delete (*columns)[i];

      delete columns;
      columns = NULL;
    }
  }


  bool AbstractCnetTableModel::isFiltering() const
  {
    return dataModel && dataModel->isFiltering();
  }


  bool AbstractCnetTableModel::sortingIsEnabled() const
  {
    return sortingEnabled;
  }


  void AbstractCnetTableModel::setSortingEnabled(bool enabled)
  {
    if (sortingEnabled != enabled)
    {
      sortingEnabled = enabled;
      rebuildSort();
    }
  }


  CnetTableColumnList * AbstractCnetTableModel::getColumns()
  {
    if (!columns)
    {
      columns = createColumns();
      connect(columns, SIGNAL(sortOutDated()), this, SLOT(sort()));
    }

    return columns;
  }


  const AbstractCnetTableDelegate * AbstractCnetTableModel::getDelegate() const
  {
    return delegate;
  }
  
  
  void AbstractCnetTableModel::applyFilter()
  {
    getDataModel()->applyFilter();
  }


  void AbstractCnetTableModel::sort()
  {
//     cerr << "AbstractCnetTableModel::sort called\n";
    
    // Go through the columns in reverse order of priority and sort on each
    // one.
    QList< CnetTableColumn * > columnsToSortOn = columns->getSortingOrder();
    if (sortingIsEnabled())
    {
      qStableSort(sortedItems->begin(), sortedItems->end(),
                  LessThanFunctor(columnsToSortOn.at(0)));
      
//       for (int i = columnsToSortOn.size() - 1; i >= 0; i--)
//         qStableSort(sortedItems->begin(), sortedItems->end(),
//                     LessThanFunctor(columnsToSortOn.at(i)));
    }
    
    emit modelModified();
//     cerr << "AbstractCnetTableModel::sort done\n";
  }


  void AbstractCnetTableModel::reverseOrder(CnetTableColumn * column)
  {
  }


  void AbstractCnetTableModel::updateSort()
  {
  }


  TreeModel * AbstractCnetTableModel::getDataModel()
  {
    ASSERT(dataModel);
    return dataModel;
  }


  const TreeModel * AbstractCnetTableModel::getDataModel() const
  {
    ASSERT(dataModel);
    return dataModel;
  }


  QList< AbstractTreeItem * > AbstractCnetTableModel::getSortedItems(
      int start, int end, TreeModel::InterestingItems flags)
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
  
  
  QList< AbstractTreeItem * > AbstractCnetTableModel::getSortedItems(
      AbstractTreeItem * item1, AbstractTreeItem * item2,
      TreeModel::InterestingItems flags)
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
  
  
  void AbstractCnetTableModel::handleTreeSelectionChanged(
      QList< AbstractTreeItem * > newlySelectedItems,
      AbstractTreeItem::InternalPointerType pointerType)
  {
    QList< AbstractTreeItem * > interestingSelectedItems;
    foreach (AbstractTreeItem * item, newlySelectedItems)
    {
      if (item->getPointerType() == pointerType)
        interestingSelectedItems.append(item);
    }

    if (interestingSelectedItems.size()) {
      emit treeSelectionChanged(interestingSelectedItems);
    }
  }

 
  void AbstractCnetTableModel::nullify()
  {
    dataModel = NULL;
    delegate = NULL;
    sortedItems = NULL;
    busyItem = NULL;
    columns = NULL;
  }
  
  
  void AbstractCnetTableModel::rebuildSort()
  {
//     cerr << "AbstractCnetTableModel::rebuildSort called\n";
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


  AbstractCnetTableModel::LessThanFunctor::LessThanFunctor(
        CnetTableColumn const * someColumn) : column(someColumn)
  {
  }


  AbstractCnetTableModel::LessThanFunctor::LessThanFunctor(
    LessThanFunctor const & other)
  {
    column = other.column;
  }
  
  
  AbstractCnetTableModel::LessThanFunctor::~LessThanFunctor()
  {
    column = NULL;
  }
  
  
  bool AbstractCnetTableModel::LessThanFunctor::operator()(
      AbstractTreeItem * const & left, AbstractTreeItem * const & right)
  {
    if (left->getPointerType() != right->getPointerType())
    {
      iString msg = "Tried to compare apples to oranges";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    QString leftData = left->getData(column->getTitle());
    QString rightData = right->getData(column->getTitle());

    bool leftOk;
    double doubleData = leftData.toDouble(&leftOk);
    
    bool rightOk;
    double rightDoubleData = rightData.toDouble(&rightOk);
    
    QString busy = BusyLeafItem().getData();
    
    bool lessThan =
        (leftOk && rightOk && doubleData < rightDoubleData) ||
        ((leftOk || rightOk) && leftData.toLower() == "null") ||
        ((leftData == busy || rightData == busy) && leftData == busy) ||
        (leftData < rightData);

    return lessThan ^ column->sortAscending();
  }


  AbstractCnetTableModel::LessThanFunctor &
      AbstractCnetTableModel::LessThanFunctor::operator=(
      LessThanFunctor const & other)
  {
    if (this != &other)
    {
      column = other.column;
    }

    return *this;
  }
}

