#include "IsisDebug.h"

#include "PointModel.h"

#include <iostream>

#include <QAbstractItemModel>
#include <QMap>
#include <QModelIndex>
#include <QString>
#include <QVariant>
#include <QVector>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "iException.h"

#include "PointParentItem.h"
#include "MeasureChildItem.h"
#include "TreeItem.h"


using std::cerr;

namespace Isis
{
  PointModel::PointModel(ControlNet * controlNet,
      QObject * parent) : QAbstractItemModel(parent), cNet(controlNet)
  {
    ASSERT(cNet);

    pointItems = NULL;
    pointItems = new QList< TreeItem * >;

    for (int i = 0; i < cNet->GetNumPoints(); i++)
    {
      ControlPoint * point = cNet->GetPoint(i);
      PointParentItem * pointItem = new PointParentItem(point);
      pointItems->append(pointItem);
      for (int j = 0; j < point->GetNumMeasures(); j++)
      {
        ControlMeasure * measure = point->GetMeasure(j);
        MeasureChildItem * measureItem = new MeasureChildItem(measure, pointItem);
        pointItem->addChild(measureItem);
      }
    }
  }


  PointModel::~PointModel()
  {
    cNet = NULL;
  }


  QVariant PointModel::data(const QModelIndex & index, int role) const
  {
    if (role != Qt::DisplayRole)
      return QVariant();

    if (!index.isValid())
      return QVariant();

    if (index.column() != 0)
      return QVariant();

    TreeItem * item = getItem(index);
    if (!item)
      cerr << "item is NULL!\n";

    TreeItem * parentItem = item->parent();
    if (parentItem)
    {
      ASSERT(pointItems->contains(parentItem));
      int row = index.row();
      return parentItem->childAt(row)->data(0);
    }
    else
    {
      return item->data(0);
    }
  }


  QVariant PointModel::headerData(int section, Qt::Orientation orientation,
      int role) const
  {
    QVariant header;

    if (role == Qt::DisplayRole && orientation == Qt::Horizontal &&
        section == 0)
    {
      header = QVariant::fromValue(QString("Point View"));
    }

    return header;
  }


  QModelIndex PointModel::index(int row, int column,
      const QModelIndex & parent) const
  {
    QModelIndex modelIndex;

    if (hasIndex(row, column, parent))
    {
      if (parent.isValid())
      {
        TreeItem * childItem = static_cast< TreeItem * >(
            parent.internalPointer())->childAt(row);

        if (childItem)
          modelIndex = createIndex(row, column, childItem);
      }
      else
      {
        modelIndex = createIndex(row, column, pointItems->at(row));
      }
    }

    return modelIndex;
  }


  QModelIndex PointModel::parent(const QModelIndex & index) const
  {
    QModelIndex pIndex;

    if (index.isValid())
    {
      TreeItem * childItem = getItem(index);
      TreeItem * parentItem = childItem->parent();

      if (parentItem)
        pIndex = createIndex(pointItems->indexOf(parentItem), 0, parentItem);
    }

    return pIndex;
  }



  int PointModel::rowCount(const QModelIndex & parent) const
  {
    if (parent.isValid())
    {
      TreeItem * parentItem = getItem(parent);

      if (parentItem)
        return parentItem->childCount();
    }

    return pointItems->size();
  }


  int PointModel::columnCount(const QModelIndex & parent) const
  {
    return 1;
  }

  /*
    Qt::ItemFlags PointModel::flags(const QModelIndex & index) const
    {
  //    cerr << "PointModel::flags called\n";

      Qt::ItemFlags flags;
      if (index.isValid())
      {
  //      cerr << "\tindex is valid!\n";
        flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
      }
      else
      {
        flags = 0;
      }

  //    cerr << "PointModel::flags done\n\n";

      return flags;
    }
  */




  TreeItem * PointModel::getItem(const QModelIndex & index) const
  {
    if (!index.isValid())
    {
      iString msg = "index invalid!";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return static_cast< TreeItem * >(index.internalPointer());
  }
}














