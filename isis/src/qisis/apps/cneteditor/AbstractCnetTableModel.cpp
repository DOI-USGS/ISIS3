#include "IsisDebug.h"

#include <iostream>

#include <QDateTime>

#include "iException.h"
#include "iString.h"

#include "AbstractCnetTableDelegate.h"
#include "AbstractCnetTableModel.h"
#include "BusyLeafItem.h"
#include "CnetTableColumn.h"
#include "CnetTableColumnList.h"
#include "TreeModel.h"


namespace Isis
{
  AbstractCnetTableModel::AbstractCnetTableModel(TreeModel * model,
      AbstractCnetTableDelegate * someDelegate)
  {
    nullify();

    dataModel = model;
    delegate = someDelegate;
    sortedRows = new QList<AbstractTreeItem *>;
    busyItem = new BusyLeafItem;
    sortingEnabled = false;

    // Signal forwarding
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

    delete sortedRows;
    sortedRows = NULL;

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


  bool AbstractCnetTableModel::isSortingEnabled() const
  {
    return sortingEnabled;
  }


  void AbstractCnetTableModel::setSortingEnabled(bool enabled)
  {
    sortingEnabled = enabled;
  }


  CnetTableColumnList * AbstractCnetTableModel::getColumns()
  {
    if (!columns)
    {
      columns = createColumns();
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


  void AbstractCnetTableModel::sort() {
    // Go through the columns in reverse order of priority and sort on each
    // one.
    QList<CnetTableColumn const *> columnsToSortOn = columns->getSortingOrder();
    if (isSortingEnabled())
      for (int i = columnsToSortOn.size() - 1; i >= 0; i--)
        qStableSort(sortedRows->begin(), sortedRows->end(),
                    LessThanFunctor(columnsToSortOn.at(i)));
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


  QList<AbstractTreeItem *> AbstractCnetTableModel::getSortedItems(
      int start, int end, TreeModel::InterestingItems flags)
  { 
    QList<AbstractTreeItem *> sortedItems;

    if (!isSortingEnabled())
    {
      sortedItems = getDataModel()->getItems(start, end, flags, true);
    }
    else
    {
      while (start <= end)
      {
        if (start < sortedRows->size())
          sortedItems.append(sortedRows->at(start));
        else if (isFiltering())
          sortedItems.append(busyItem);

        start++;
      }
    }

    return sortedItems;
  }
  
  
  QList<AbstractTreeItem *> AbstractCnetTableModel::getSortedItems(
      AbstractTreeItem * item1, AbstractTreeItem * item2,
      TreeModel::InterestingItems flags)
  {
    QList<AbstractTreeItem *> sortedItems;

    if (!isSortingEnabled())
    {
      sortedItems = getDataModel()->getItems(item1, item2, flags, true);
    }
    else
    {
      AbstractTreeItem * start = NULL;

      int currentIndex = 0;

      while (!start && currentIndex < sortedRows->size())
      {
        AbstractTreeItem * current = sortedRows->at(currentIndex);
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
      // variable "meth" which is a method pointer to the appropriate method.
      void (QList<AbstractTreeItem*>::*someKindaPend)(
          AbstractTreeItem * const &);
      
      someKindaPend = &QList<AbstractTreeItem *>::append;
      if (start == item2)
      {
        end = item1;
        someKindaPend = &QList<AbstractTreeItem *>::prepend;
      }

      while (currentIndex < sortedRows->size() &&
             sortedRows->at(currentIndex) != end)
      {
        (sortedItems.*someKindaPend)(sortedRows->at(currentIndex));
        currentIndex++;
      }

      if (currentIndex >= sortedRows->size())
      {
        iString msg = "Could not find the second item";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }

      (sortedItems.*someKindaPend)(end);
    }

    return sortedItems;
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
    
    if (interestingSelectedItems.size())
      emit treeSelectionChanged(interestingSelectedItems);
  }

 
  void AbstractCnetTableModel::nullify()
  {
    dataModel = NULL;
    delegate = NULL;
    sortedRows = NULL;
    busyItem = NULL;
    columns = NULL;
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

    QString format = "yyyy-MM-ddTHH:mm:ss";

    QDateTime leftDateTime = QDateTime::fromString(leftData, format);
    QDateTime rightDateTime = QDateTime::fromString(rightData, format);

    if (leftDateTime.isValid() && rightDateTime.isValid())
      return leftDateTime < rightDateTime;

    bool doubleDataOk, rightDoubleDataOk;
    double doubleData = leftData.toDouble(&doubleDataOk);
    double rightDoubleData = rightData.toDouble(&rightDoubleDataOk);
    if (doubleDataOk && rightDoubleDataOk)
      return doubleData < rightDoubleData;

    if (doubleDataOk || rightDoubleDataOk)
      return leftData.toLower() == "null";

    QString busyData = BusyLeafItem().getData();
    if (leftData == busyData || rightData == busyData)
      return leftData == busyData;

    return leftData < rightData;
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

