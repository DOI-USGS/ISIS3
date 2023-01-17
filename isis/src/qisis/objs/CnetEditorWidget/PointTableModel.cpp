/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "PointTableModel.h"

#include <iostream>

#include <QDebug>
#include <QList>
#include <QMessageBox>
#include <QStringList>
#include <QVariant>

#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "Distance.h"
#include "IException.h"
#include "IString.h"
#include "Latitude.h"
#include "Longitude.h"
#include "SpecialPixel.h"

#include "AbstractTableDelegate.h"
#include "AbstractPointItem.h"
#include "TableColumn.h"
#include "TableColumnList.h"
#include "PointTableDelegate.h"
#include "AbstractTreeModel.h"


namespace Isis {
  PointTableModel::PointTableModel(AbstractTreeModel *model) :
    AbstractTableModel(model, new PointTableDelegate) {
    connect(model, SIGNAL(filterCountsChanged(int, int)),
        this, SIGNAL(filterCountsChanged(int, int)));
    connect(model,
        SIGNAL(treeSelectionChanged(QList< AbstractTreeItem * >)),
        this,
        SLOT(handleTreeSelectionChanged(QList< AbstractTreeItem * >)));
  }


  PointTableModel::~PointTableModel() {
  }


  QList< AbstractTreeItem * > PointTableModel::getItems(
    int start, int end) {
    return getSortedItems(start, end, AbstractTreeModel::PointItems);
  }


  QList< AbstractTreeItem * > PointTableModel::getItems(
    AbstractTreeItem *item1, AbstractTreeItem *item2) {
    return getSortedItems(item1, item2, AbstractTreeModel::PointItems);
  }


  int PointTableModel::getVisibleRowCount() const {
    return getDataModel()->getVisibleItemCount(AbstractTreeModel::PointItems,
        true);
  }


  QList< AbstractTreeItem * > PointTableModel::getSelectedItems() {
    return getDataModel()->getSelectedItems(AbstractTreeModel::PointItems,
        true);
  }


  QString PointTableModel::getWarningMessage(AbstractTreeItem const *row,
      TableColumn const *column, QString valueToSave) const {
    return getPointWarningMessage(row, column, valueToSave);
  }


  void PointTableModel::setGlobalSelection(bool selected) {
    return getDataModel()->setGlobalSelection(selected,
        AbstractTreeModel::AllItems);
  }


  int PointTableModel::indexOfVisibleItem(
    AbstractTreeItem const *item) const {
    return getDataModel()->indexOfVisibleItem(item,
        AbstractTreeModel::PointItems,
        true);
  }


  QString PointTableModel::getPointWarningMessage(
    AbstractTreeItem const *row, TableColumn const *column,
    QString valueToSave) {
    QString colTitle = column->getTitle();
    AbstractPointItem::Column colType =
      AbstractPointItem::getColumn(colTitle);

    QString warningText;

    switch (colType) {
      case AbstractPointItem::EditLock:
        if (valueToSave.toLower() == "no" &&
            row->getFormattedData(colTitle).toLower() == "yes") {
          warningText = "Are you sure you want to unlock control point [" +
              row->getFormattedData() + "] for editing?";
        }
        break;
      case AbstractPointItem::APrioriSPLatSigma:
      case AbstractPointItem::APrioriSPLonSigma:
      case AbstractPointItem::APrioriSPRadiusSigma: {
          ControlPoint *point = (ControlPoint *) row->getPointer();

          // Check to see if any of the sigma values are null.
          bool latSigmaValid = (point->GetAprioriSurfacePoint().
              GetLatSigmaDistance().isValid());
          bool lonSigmaValid = (point->GetAprioriSurfacePoint().
              GetLonSigmaDistance().isValid());
          bool radiusSigmaValid = (point->GetAprioriSurfacePoint().
              GetLocalRadiusSigma().isValid());

          if (!latSigmaValid && !lonSigmaValid && !radiusSigmaValid &&
              valueToSave.toLower() != "null") {
            warningText = "The sigma values are currently null. The other "
                "sigmas will be set to 10,000, which currently represents "
                "'free'. Is this okay?";
          }
          break;
        }
      case AbstractPointItem::APrioriSPLat:
      case AbstractPointItem::APrioriSPLon:
      case AbstractPointItem::APrioriSPRadius: {
          ControlPoint *point = (ControlPoint *) row->getPointer();

          // Check to see if any of the surface point values are null.
          bool latValid = (point->GetAprioriSurfacePoint().
              GetLatitude().isValid());
          bool lonValid = (point->GetAprioriSurfacePoint().
              GetLongitude().isValid());
          bool radiusValid = (point->GetAprioriSurfacePoint().
              GetLocalRadius().isValid());

          if (!latValid && !lonValid && !radiusValid &&
              valueToSave.toLower() != "null") {
            warningText = "Some of the a priori surface point values are "
                "currently null. The surface point lat and lon will be set "
                "to 0 if they are null, and the radius will be set to "
                "10,000 if it is null. Is this okay?";
          }
          break;
        }

      default:
        break;
    }

    return warningText;
  }


  void PointTableModel::handleTreeSelectionChanged(
    QList< AbstractTreeItem * > newlySelectedItems) {
    AbstractTableModel::handleTreeSelectionChanged(
      newlySelectedItems, AbstractTreeItem::Point);

    QList<AbstractTreeItem *> measureParentItems;
    foreach (AbstractTreeItem * item, newlySelectedItems) {
      if (item->getPointerType() == AbstractTreeItem::Measure) {
        measureParentItems.append(item->parent());
      }
    }

    if (measureParentItems.size()) {
      AbstractTableModel::handleTreeSelectionChanged(
        measureParentItems, AbstractTreeItem::Point);
    }
  }


  TableColumnList *PointTableModel::createColumns() {
    return AbstractPointItem::createColumns();
  }
}
