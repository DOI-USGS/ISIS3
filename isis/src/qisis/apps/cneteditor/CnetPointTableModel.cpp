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
    return getSortedItems(start, end, TreeModel::PointItems);
  }


  QList< AbstractTreeItem * > CnetPointTableModel::getItems(
      AbstractTreeItem * item1, AbstractTreeItem * item2)
  {
    return getSortedItems(item1, item2, TreeModel::PointItems);
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

    switch (colType)
    {
      case AbstractPointItem::EditLock:
        if (valueToSave.toLower() == "no" &&
            row->getData(colTitle).toLower() == "yes") {
          warningText = "Are you sure you want to unlock control point [" +
              row->getData() + "] for editing?";
        }
        break;
      case AbstractPointItem::APrioriSPLatSigma:
      case AbstractPointItem::APrioriSPLonSigma:
      case AbstractPointItem::APrioriSPRadiusSigma:
        {
          ASSERT(row->getPointerType() == AbstractTreeItem::Point);
          ControlPoint * point = (ControlPoint *) row->getPointer();

          // Check to see if any of the sigma values are null.
          bool latSigmaValid = (point->GetAprioriSurfacePoint().
              GetLatSigmaDistance().Valid());
          bool lonSigmaValid = (point->GetAprioriSurfacePoint().
              GetLonSigmaDistance().Valid());
          bool radiusSigmaValid = (point->GetAprioriSurfacePoint().
              GetLocalRadiusSigma().Valid());

          if (!latSigmaValid && !lonSigmaValid && !radiusSigmaValid &&
              valueToSave.toLower() != "null")
          {
            warningText = "The sigma values are currently null. The other "
                "sigmas will be set to 10,000, which currently represents "
                "'free'. Is this okay?";
          }
          break;
        }
      default:
        break;
    }

    return warningText;
  }
  
  
  void CnetPointTableModel::handleTreeSelectionChanged(
      QList< AbstractTreeItem * > newlySelectedItems)
  {
    AbstractCnetTableModel::handleTreeSelectionChanged(
        newlySelectedItems, AbstractTreeItem::Point);
  }


  CnetTableColumnList * CnetPointTableModel::createColumns()
  {
    return AbstractPointItem::createColumns();
  }
}

