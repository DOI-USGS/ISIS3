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
#include "CnetTableColumn.h"
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
    connect(model, SIGNAL(selectionChanged(QList< AbstractTreeItem * >)),
            this, SLOT(handleSelectionChanged(QList< AbstractTreeItem * >)));
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


  QString CnetMeasureTableModel::getWarningMessage(AbstractTreeItem const * row,
      CnetTableColumn const * column, QString valueToSave) const {
    return getMeasureWarningMessage(row, column, valueToSave);
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
  
  
  int CnetMeasureTableModel::indexOfVisibleItem(
      AbstractTreeItem const * item) const
  {
    return getDataModel()->indexOfVisibleItem(item, TreeModel::MeasureItems,
                                              true);
  }

  
  QString CnetMeasureTableModel::getMeasureWarningMessage(
      AbstractTreeItem const * row, CnetTableColumn const * column,
      QString valueToSave) {
    QString colTitle = column->getTitle();
    AbstractMeasureItem::Column colType =
        AbstractMeasureItem::getColumn(colTitle);

    QString warningText;

    if (colType == AbstractMeasureItem::EditLock &&
        valueToSave.toLower() == "no" &&
        row->getData(colTitle).toLower() == "yes") {
      QString pointColTitle =
          AbstractMeasureItem::getColumnName(AbstractMeasureItem::PointId);
      warningText = "Are you sure you want to unlock control measure [" +
          row->getData() + "] in point [" + row->getData(pointColTitle) +
          "] for editing?";
    }

    return warningText;
  }
  
  
  void CnetMeasureTableModel::handleSelectionChanged(
      QList< AbstractTreeItem * > newlySelectedItems)
  {
    QList< AbstractTreeItem * > interestingSelectedItems;
    foreach (AbstractTreeItem * item, newlySelectedItems)
    {
      if (item->getPointerType() == AbstractTreeItem::Measure)
        interestingSelectedItems.append(item);
    }
    
    emit selectionChanged(interestingSelectedItems);
  }
}

