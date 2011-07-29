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


  const AbstractCnetTableDelegate * AbstractCnetTableModel::getDelegate()
  const
  {
    return delegate;
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
}
