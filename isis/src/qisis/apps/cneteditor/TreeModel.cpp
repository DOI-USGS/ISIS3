#include "IsisDebug.h"

#include "TreeModel.h"

#include <iostream>

#include <QList>
#include <QModelIndex>
#include <QString>
#include <QTreeView>
#include <QVariant>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "iException.h"

#include "TreeItem.h"

namespace Isis
{
  TreeModel::TreeModel(ControlNet * controlNet, QString name,
      QObject * parent) : QAbstractItemModel(parent), cNet(controlNet)
  {
    ASSERT(cNet);

    headerTitle = NULL;
    parentItems = NULL;
    expandedItems = NULL;
    views = NULL;

    headerTitle = new QString(name);
    parentItems = new QList< TreeItem * >;
    expandedItems = new QList< QString >;
    views = new QList< QTreeView * >;

    connect(cNet, SIGNAL(networkStructureModified()),
        this, SLOT(rebuildItems()));

    drivable = false;
  }


  TreeModel::~TreeModel()
  {
    if (headerTitle)
    {
      delete headerTitle;
      headerTitle = NULL;
    }

    if (parentItems)
    {
      for (int i = 0; i < parentItems->size(); i++)
      {
        if (parentItems->at(i))
        {
          delete (*parentItems)[i];
          (*parentItems)[i] = NULL;
        }
      }
      delete parentItems;
      parentItems = NULL;
    }
    
    if (expandedItems)
    {
      delete expandedItems;
      expandedItems = NULL;
    }
    
    if (views)
    {
      delete views;
      views = NULL;
    }

    cNet = NULL;
  }


  QVariant TreeModel::data(const QModelIndex & index, int role) const
  {
    if (role != Qt::DisplayRole)
      return QVariant();

    if (!index.isValid())
      return QVariant();

    if (index.column() != 0)
      return QVariant();

    TreeItem * item = getItem(index);
    ASSERT(item);

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


  QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
      int role) const
  {
    QVariant header;

    if (role == Qt::DisplayRole && orientation == Qt::Horizontal &&
        section == 0)
    {
      header = QVariant::fromValue(*headerTitle);
    }

    return header;
  }


  QModelIndex TreeModel::index(int row, int column,
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


  QModelIndex TreeModel::parent(const QModelIndex & index) const
  {
    QModelIndex parentIndex;

    if (index.isValid())
    {
      TreeItem * childItem = getItem(index);
      TreeItem * parentItem = childItem->parent();

      if (parentItem)
      {
        TreeItem * grandParentItem = parentItem->parent();
        if (grandParentItem)
          parentIndex = createIndex(parentItem->row(), 0, parentItem);
        else
          parentIndex = createIndex(
              parentItems->indexOf(parentItem), 0, parentItem);
      }
    }

    return parentIndex;
  }


  int TreeModel::rowCount(const QModelIndex & parent) const
  {
    if (parent.isValid())
    {
      TreeItem * parentItem = getItem(parent);

      if (parentItem)
        return parentItem->childCount();
    }

    return parentItems->size();
  }


  int TreeModel::columnCount(const QModelIndex & parent) const
  {
    return 1;
  }


  Qt::ItemFlags TreeModel::flags(const QModelIndex & index) const
  {
    Qt::ItemFlags flags = Qt::ItemIsSelectable;

    if (index.isValid() && drivable)
      flags = flags | Qt::ItemIsEnabled;

    return flags;
  }
  
  
  void TreeModel::addView(QTreeView * newView)
  {
    views->append(newView);
  }
  

  void TreeModel::setDrivable(bool drivableStatus)
  {
    if (drivable != drivableStatus)
    {
      drivable = drivableStatus;
      emit(dataChanged(QModelIndex(), QModelIndex()));
    }
  }


  void TreeModel::clearParentItems()
  {
    ASSERT(parentItems);
    ASSERT(expandedItems);
    
    expandedItems->clear();

    beginRemoveRows(QModelIndex(), 0, parentItems->size() - 1);
    for (int i = 0; i < parentItems->size(); i++)
    {
      if (parentItems->at(i))
      {
        // save off expanded items
        if (parentItems->at(i)->isExpanded())
          expandedItems->append(parentItems->at(i)->data(0).toString());
          
        delete(*parentItems)[i];
        (*parentItems)[i] = NULL;
      }
    }
    parentItems->clear();
    endRemoveRows();
  }


  TreeItem * TreeModel::getItem(const QModelIndex & index) const
  {
    if (!index.isValid())
    {
      iString msg = "index invalid!";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return static_cast< TreeItem * >(index.internalPointer());
  }
}
