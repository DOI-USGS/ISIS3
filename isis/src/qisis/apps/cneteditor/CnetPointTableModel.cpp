#include "IsisDebug.h"

#include "CnetPointTableModel.h"

#include <iostream>

#include <QList>
#include <QMessageBox>
#include <QStringList>

#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "Distance.h"
#include "iException.h"
#include "iString.h"
#include "Latitude.h"
#include "Longitude.h"
#include "SpecialPixel.h"

#include "AbstractCnetTableDelegate.h"
#include "AbstractPointItem.h"
#include "CnetTableColumn.h"
#include "CnetTableColumnList.h"
#include "CnetPointTableDelegate.h"
#include "TreeModel.h"


using std::cerr;


namespace Isis
{
  CnetPointTableModel::CnetPointTableModel(TreeModel * model) :
      AbstractCnetTableModel(model, new CnetPointTableDelegate)
  {
    connect(model, SIGNAL(filterCountsChanged(int, int)),
            this, SIGNAL(filterCountsChanged(int, int)));
    connect(model, SIGNAL(treeSelectionChanged(QList< AbstractTreeItem * >)),
            this, SLOT(handleTreeSelectionChanged(QList< AbstractTreeItem * >)));
  }


  CnetPointTableModel::~CnetPointTableModel()
  {
  }


  QList< AbstractTreeItem * > CnetPointTableModel::getItems(
      int start, int end)
  {
    return getDataModel()->getItems(start, end, TreeModel::PointItems, true);
  }


  QList< AbstractTreeItem * > CnetPointTableModel::getItems(
      AbstractTreeItem * item1, AbstractTreeItem * item2)
  {
    return getDataModel()->getItems(item1, item2, TreeModel::PointItems, true);
  }


  CnetTableColumnList CnetPointTableModel::createColumns()
  {
    return AbstractPointItem::createColumns();
  }


  int CnetPointTableModel::getVisibleRowCount() const
  {
    return getDataModel()->getVisibleItemCount(TreeModel::PointItems, true);
  }


  QList< AbstractTreeItem * > CnetPointTableModel::getSelectedItems()
  {
    return getDataModel()->getSelectedItems(TreeModel::PointItems, true);
  }


  QString CnetPointTableModel::getWarningMessage(AbstractTreeItem const * row,
      CnetTableColumn const * column, QString valueToSave) const {
    return getPointWarningMessage(row, column, valueToSave);
  }


  void CnetPointTableModel::setGlobalSelection(bool selected)
  {
    return getDataModel()->setGlobalSelection(selected, TreeModel::AllItems);
  }
  
  
  int CnetPointTableModel::indexOfVisibleItem(
      AbstractTreeItem const * item) const
  {
    return getDataModel()->indexOfVisibleItem(item, TreeModel::PointItems,
                                              true);
  }
  
  
  QString CnetPointTableModel::getPointWarningMessage(
      AbstractTreeItem const * row, CnetTableColumn const * column,
      QString valueToSave) {
    QString colTitle = column->getTitle();
    AbstractPointItem::Column colType = AbstractPointItem::getColumn(colTitle);

    QString warningText;

    if (colType == AbstractPointItem::EditLock &&
        valueToSave.toLower() == "no" &&
        row->getData(colTitle).toLower() == "yes") {
      warningText = "Are you sure you want to unlock control point [" +
          row->getData() + "] for editing?";
    }

    return warningText;
  }
  
  
  void CnetPointTableModel::handleTreeSelectionChanged(
      QList< AbstractTreeItem * > newlySelectedItems)
  {
    AbstractCnetTableModel::handleTreeSelectionChanged(
        newlySelectedItems, AbstractTreeItem::Point);
  }
}

