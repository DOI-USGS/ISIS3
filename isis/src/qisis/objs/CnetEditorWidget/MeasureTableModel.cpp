/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "MeasureTableModel.h"

#include <iostream>

#include <QDebug>
#include <QList>
#include <QMessageBox>
#include <QStringList>
#include <QVariant>

#include "ControlMeasure.h"
#include "ControlMeasure.h"
#include "Distance.h"
#include "IException.h"
#include "IString.h"
#include "Latitude.h"
#include "Longitude.h"
#include "SpecialPixel.h"

#include "AbstractTableDelegate.h"
#include "AbstractMeasureItem.h"
#include "TableColumn.h"
#include "TableColumnList.h"
#include "MeasureTableDelegate.h"
#include "AbstractTreeModel.h"


namespace Isis {
  MeasureTableModel::MeasureTableModel(AbstractTreeModel *model) :
    AbstractTableModel(model, new MeasureTableDelegate) {
    connect(model, SIGNAL(filterCountsChanged(int, int)),
        this, SLOT(calculateFilterCounts()));
    connect(model,
        SIGNAL(treeSelectionChanged(QList< AbstractTreeItem * >)),
        this,
        SLOT(handleTreeSelectionChanged(QList< AbstractTreeItem * >)));
  }


  MeasureTableModel::~MeasureTableModel() {
  }


  QList< AbstractTreeItem * > MeasureTableModel::getItems(
    int start, int end) {
    return getSortedItems(start, end, AbstractTreeModel::MeasureItems);
  }


  QList< AbstractTreeItem * > MeasureTableModel::getItems(
    AbstractTreeItem *item1, AbstractTreeItem *item2) {
    return getSortedItems(item1, item2, AbstractTreeModel::MeasureItems);
  }


  int MeasureTableModel::getVisibleRowCount() const {
    return getDataModel()->getVisibleItemCount(
        AbstractTreeModel::MeasureItems, true);
  }


  QList< AbstractTreeItem * > MeasureTableModel::getSelectedItems() {
    return getDataModel()->getSelectedItems(
        AbstractTreeModel::MeasureItems, true);
  }


  QString MeasureTableModel::getWarningMessage(AbstractTreeItem const *row,
      TableColumn const *column, QString valueToSave) const {
    return getMeasureWarningMessage(row, column, valueToSave);
  }


  void MeasureTableModel::setGlobalSelection(bool selected) {
    return getDataModel()->setGlobalSelection(selected,
        AbstractTreeModel::MeasureItems);
  }


  void MeasureTableModel::calculateFilterCounts() {
    int visible = getDataModel()->getVisibleItemCount(
        AbstractTreeModel::MeasureItems, true);
    int total = getDataModel()->getItemCount(
        AbstractTreeModel::MeasureItems);

    emit filterCountsChanged(visible, total);
  }


  int MeasureTableModel::indexOfVisibleItem(
    AbstractTreeItem const *item) const {
    return getDataModel()->indexOfVisibleItem(item,
        AbstractTreeModel::MeasureItems, true);
  }


  QString MeasureTableModel::getMeasureWarningMessage(
    AbstractTreeItem const *row, TableColumn const *column,
    QString valueToSave) {
    QString colTitle = column->getTitle();
    AbstractMeasureItem::Column colType =
      AbstractMeasureItem::getColumn(colTitle);

    QString warningText;

    if (colType == AbstractMeasureItem::EditLock &&
        valueToSave.toLower() == "no" &&
        row->getFormattedData(colTitle).toLower() == "yes") {
      QString pointColTitle =
        AbstractMeasureItem::getColumnName(AbstractMeasureItem::PointId);
      warningText = "Are you sure you want to unlock control measure [" +
          row->getFormattedData() + "] in point [" +
          row->getFormattedData(pointColTitle) + "] for editing?";
    }

    return warningText;
  }


  void MeasureTableModel::handleTreeSelectionChanged(
    QList< AbstractTreeItem * > newlySelectedItems) {
    AbstractTableModel::handleTreeSelectionChanged(
      newlySelectedItems, AbstractTreeItem::Measure);
  }


  TableColumnList *MeasureTableModel::createColumns() {
    return AbstractMeasureItem::createColumns();
  }
}
