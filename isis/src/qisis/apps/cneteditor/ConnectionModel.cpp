#include "IsisDebug.h"

#include "ConnectionModel.h"

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

#include "SerialConnectionParentItem.h"
#include "SerialParentItem.h"
#include "PointChildItem.h"
#include "TreeItem.h"


using std::cerr;

namespace Isis
{
  ConnectionModel::ConnectionModel(ControlNet * controlNet,
      QObject * parent) : QAbstractItemModel(parent), cNet(controlNet)
  {
    ASSERT(cNet);

    parentItems = NULL;
    parentItems = new QList< TreeItem * >;

    QList< ControlCubeGraphNode * > nodes = cNet->GetCubeGraphNodes();

    for (int i = 0; i < nodes.size(); i++)
    {
      ControlCubeGraphNode * node = nodes[i];
      SerialConnectionParentItem * parentItem =
        new SerialConnectionParentItem(node);
      parentItems->append(parentItem);

      QList< ControlCubeGraphNode * > connectedNodes = node->getAdjacentNodes();
      for (int j = 0; j < connectedNodes.size(); j++)
      {
        ControlCubeGraphNode * connectedNode = connectedNodes[j];
        SerialParentItem * serialItem = new SerialParentItem(connectedNode,
            parentItem);

        QList< ControlMeasure * > measures = connectedNode->getMeasures();
        for (int k = 0; k < measures.size(); k++)
        {
          ControlPoint * point = measures[k]->Parent();
          PointChildItem * pointItem = new PointChildItem(point, serialItem);
          serialItem->addChild(pointItem);
        }

        parentItem->addChild(serialItem);
      }
    }
  }


  ConnectionModel::~ConnectionModel()
  {
    cNet = NULL;
  }


  QVariant ConnectionModel::data(const QModelIndex & index, int role) const
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
      int row = index.row();
      return parentItem->childAt(row)->data(0);
    }
    else
    {
      return item->data(0);
    }
  }


  QVariant ConnectionModel::headerData(int section, Qt::Orientation orientation,
      int role) const
  {
    QVariant header;

    if (role == Qt::DisplayRole && orientation == Qt::Horizontal &&
        section == 0)
    {
      header = QVariant::fromValue(QString("Cube Connection View"));
    }

    return header;
  }


  QModelIndex ConnectionModel::index(int row, int column,
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
        modelIndex = createIndex(row, column, parentItems->at(row));
      }
    }

    return modelIndex;
  }


  QModelIndex ConnectionModel::parent(const QModelIndex & index) const
  {
    QModelIndex pIndex;

    if (index.isValid())
    {
      TreeItem * childItem = getItem(index);
      TreeItem * parentItem = childItem->parent();

      if (parentItem)
      {
        TreeItem * grandParentItem = parentItem->parent();
        if (grandParentItem)
          pIndex = createIndex(parentItem->row(), 0, parentItem);
        else
          pIndex = createIndex(parentItems->indexOf(parentItem), 0, parentItem);
      }
    }

    return pIndex;
  }



  int ConnectionModel::rowCount(const QModelIndex & parent) const
  {
    if (parent.isValid())
    {
      TreeItem * parentItem = getItem(parent);

      if (parentItem)
        return parentItem->childCount();
    }

    return parentItems->size();
  }


  int ConnectionModel::columnCount(const QModelIndex & parent) const
  {
    return 1;
  }


  Qt::ItemFlags ConnectionModel::flags(const QModelIndex & index) const
  {
    Qt::ItemFlags flags = 0;
    if (index.isValid())
    {
      flags = Qt::ItemIsEnabled;
      if (selectionEnabled)
        flags = flags | Qt::ItemIsSelectable;
    }

    return flags;
  }


  TreeItem * ConnectionModel::getItem(const QModelIndex & index) const
  {
    if (!index.isValid())
    {
      iString msg = "index invalid!";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return static_cast< TreeItem * >(index.internalPointer());
  }

}














