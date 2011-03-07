#include "IsisDebug.h"

#include "SerialModel.h"

#include <iostream>

#include <QAbstractItemModel>
#include <QMap>
#include <QModelIndex>
#include <QString>
#include <QVariant>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "iException.h"

#include "SerialParentItem.h"
#include "PointChildItem.h"
#include "TreeItem.h"


using std::cerr;

namespace Isis
{
  SerialModel::SerialModel(ControlNet * controlNet,
      QObject * parent) : QAbstractItemModel(parent), cNet(controlNet)
  {
    ASSERT(cNet);

    serialItems = NULL;
    serialItems = new QList< TreeItem * >;

    QList< ControlCubeGraphNode * > nodes = cNet->GetCubeGraphNodes();

    for (int i = 0; i < nodes.size(); i++)
    {
      ControlCubeGraphNode * node = nodes[i];
      SerialParentItem * serialItem = new SerialParentItem(node);
      serialItems->append(serialItem);

      QList< ControlMeasure * > measures = node->getMeasures();
      for (int j = 0; j < measures.size(); j++)
      {
        ControlPoint * point = measures[j]->Parent();
        PointChildItem * pointItem = new PointChildItem(point, serialItem);
        serialItem->addChild(pointItem);
      }
    }
  }


  SerialModel::~SerialModel()
  {
    cNet = NULL;
  }


  QVariant SerialModel::data(const QModelIndex & index, int role) const
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
      ASSERT(serialItems->contains(parentItem));
      int row = index.row();
      return parentItem->childAt(row)->data(0);
    }
    else
    {
      return item->data(0);
    }
  }


  QVariant SerialModel::headerData(int section, Qt::Orientation orientation,
      int role) const
  {
    QVariant header;

    if (role == Qt::DisplayRole && orientation == Qt::Horizontal &&
        section == 0)
    {
      header = QVariant::fromValue(QString("Cube View"));
    }

    return header;
  }


  QModelIndex SerialModel::index(int row, int column,
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
        modelIndex = createIndex(row, column, serialItems->at(row));
      }
    }

    return modelIndex;
  }


  QModelIndex SerialModel::parent(const QModelIndex & index) const
  {
    QModelIndex pIndex;

    if (index.isValid())
    {
      TreeItem * childItem = getItem(index);
      TreeItem * parentItem = childItem->parent();

      if (parentItem)
        pIndex = createIndex(serialItems->indexOf(parentItem), 0, parentItem);
    }

    return pIndex;
  }



  int SerialModel::rowCount(const QModelIndex & parent) const
  {
    if (parent.isValid())
    {
      TreeItem * parentItem = getItem(parent);

      if (parentItem)
        return parentItem->childCount();
    }

    return serialItems->size();
  }


  int SerialModel::columnCount(const QModelIndex & parent) const
  {
    return 1;
  }

  /*
    Qt::ItemFlags SerialModel::flags(const QModelIndex & index) const
    {
  //    cerr << "SerialModel::flags called\n";

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

  //    cerr << "SerialModel::flags done\n\n";

      return flags;
    }
  */




  TreeItem * SerialModel::getItem(const QModelIndex & index) const
  {
    if (!index.isValid())
    {
      iString msg = "index invalid!";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return static_cast< TreeItem * >(index.internalPointer());
  }
}














