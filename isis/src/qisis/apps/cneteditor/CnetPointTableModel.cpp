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
    return QString();
  }


  void CnetPointTableModel::setGlobalSelection(bool selected)
  {
    return getDataModel()->setGlobalSelection(selected, TreeModel::PointItems);
  }
}

