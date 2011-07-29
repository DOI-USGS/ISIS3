#include "IsisDebug.h"

#include "CnetMeasureTableModel.h"

#include <iostream>

#include <QList>
#include <QMessageBox>
#include <QStringList>

#include "ControlMeasure.h"
#include "ControlMeasure.h"
#include "Distance.h"
#include "iException.h"
#include "iString.h"
#include "Latitude.h"
#include "Longitude.h"
#include "SpecialPixel.h"

#include "AbstractCnetTableDelegate.h"
#include "AbstractMeasureItem.h"
#include "CnetTableColumnList.h"
#include "CnetMeasureTableDelegate.h"
#include "TreeModel.h"


using std::cerr;


namespace Isis
{
  CnetMeasureTableModel::CnetMeasureTableModel(TreeModel * model) :
    AbstractCnetTableModel(model, new CnetMeasureTableDelegate)
  {
    connect(model, SIGNAL(filterCountsChanged(int, int)),
        this, SLOT(calculateFilterCounts()));
  }


  CnetMeasureTableModel::~CnetMeasureTableModel()
  {
  }


  QList< AbstractTreeItem * > CnetMeasureTableModel::getItems(
    int start, int end)
  {
    return getDataModel()->getItems(start, end, TreeModel::MeasureItems, true);
  }


  QList< AbstractTreeItem * > CnetMeasureTableModel::getItems(
    AbstractTreeItem * item1, AbstractTreeItem * item2)
  {
    return getDataModel()->getItems(item1, item2, TreeModel::MeasureItems,
        true);
  }


  CnetTableColumnList CnetMeasureTableModel::createColumns()
  {
    return AbstractMeasureItem::createColumns();
  }


  int CnetMeasureTableModel::getVisibleRowCount() const
  {
    return getDataModel()->getVisibleItemCount(TreeModel::MeasureItems, true);
  }


  QList< AbstractTreeItem * > CnetMeasureTableModel::getSelectedItems()
  {
    return getDataModel()->getSelectedItems(TreeModel::MeasureItems, true);
  }


  void CnetMeasureTableModel::setGlobalSelection(bool selected)
  {
    return getDataModel()->setGlobalSelection(selected,
        TreeModel::MeasureItems);
  }


  void CnetMeasureTableModel::calculateFilterCounts()
  {
    int visible =
      getDataModel()->getVisibleItemCount(TreeModel::MeasureItems, true);
    int total = getDataModel()->getItemCount(TreeModel::MeasureItems);

    emit filterCountsChanged(visible, total);
  }
}

