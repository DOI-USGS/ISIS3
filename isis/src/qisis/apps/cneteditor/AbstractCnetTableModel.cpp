#include "IsisDebug.h"

#include <iostream>

#include "AbstractCnetTableDelegate.h"
#include "AbstractCnetTableModel.h"

#include "TreeModel.h"


namespace Isis
{
  AbstractCnetTableModel::AbstractCnetTableModel(TreeModel * model,
      AbstractCnetTableDelegate * someDelegate)
  {
    dataModel = NULL;
    delegate = NULL;

    dataModel = model;
    delegate = someDelegate;

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
  }


  bool AbstractCnetTableModel::isFiltering()
  {
    return dataModel && dataModel->isFiltering();
  }


  const AbstractCnetTableDelegate * AbstractCnetTableModel::getDelegate() const
  {
    return delegate;
  }
  
  
  void AbstractCnetTableModel::applyFilter()
  {
    getDataModel()->applyFilter();
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
}
