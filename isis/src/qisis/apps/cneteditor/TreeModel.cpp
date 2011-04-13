#include "IsisDebug.h"

#include "TreeModel.h"

#include <iostream>

#include <QList>
#include <QModelIndex>
#include <QPair>
#include <QStack>
#include <QString>
#include <QTreeView>
#include <QVariant>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "iException.h"

#include "AbstractTreeItem.h"
#include "RootItem.h"

using std::cerr;


namespace Isis
{
  TreeModel::TreeModel(ControlNet * controlNet, QString name, QTreeView * tv,
      QObject * parent) : QAbstractItemModel(parent), cNet(controlNet), view(tv)
  {
    ASSERT(cNet);

    headerTitle = NULL;
    rootItem = NULL;
    expandedState = NULL;
    selectedState = NULL;

    headerTitle = new QString(name);
    rootItem = new RootItem;
    expandedState = new QList< QPair< QString, QString > >;
    selectedState = new QList< QPair< QString, QString > >;

    drivable = false;
  }


  TreeModel::~TreeModel()
  {
    if (headerTitle)
    {
      delete headerTitle;
      headerTitle = NULL;
    }

    if (rootItem)
    {
      delete rootItem;
      rootItem = NULL;
    }

    if (expandedState)
    {
      delete expandedState;
      expandedState = NULL;
    }

    if (selectedState)
    {
      delete selectedState;
      selectedState = NULL;
    }

    cNet = NULL;
    view = NULL;
  }


  QVariant TreeModel::data(const QModelIndex & index, int role) const
  {
    QVariant variant;
    
    if (index.isValid() && role == Qt::DisplayRole)
    {
      AbstractTreeItem * item = indexToItem(index);
      variant = item->data();
    }

    return variant;
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
      AbstractTreeItem * parentItem = indexToItem(parent);
      AbstractTreeItem * childItem = parentItem->childAt(row);

      modelIndex = createIndex(row, column, childItem);
    }

    return modelIndex;
  }


  QModelIndex TreeModel::parent(const QModelIndex & index) const
  {
    AbstractTreeItem * childItem = indexToItem(index);
    AbstractTreeItem * parentItem = childItem->parent();
    
    QModelIndex parentIndex;
    
    int row = parentItem->row();
    if (row != -1)
      parentIndex = createIndex(row, 0, parentItem);
    
    return parentIndex;
  }


  int TreeModel::rowCount(const QModelIndex & parent) const
  {
    AbstractTreeItem * parentItem = indexToItem(parent);
    return parentItem->childCount();
  }


  int TreeModel::columnCount(const QModelIndex & parent) const
  {
    Q_UNUSED(parent);
    return 1;
  }


  Qt::ItemFlags TreeModel::flags(const QModelIndex & index) const
  {
    Qt::ItemFlags flags = Qt::ItemIsSelectable;

    if (index.isValid() && drivable)
      flags = flags | Qt::ItemIsEnabled;

    return flags;
  }


  void TreeModel::setDrivable(bool drivableStatus)
  {
    if (drivable != drivableStatus)
    {
      drivable = drivableStatus;
      emit(dataChanged(QModelIndex(), QModelIndex()));
    }
  }


  void TreeModel::clear()
  {
    ASSERT(rootItem);
    
    beginRemoveRows(QModelIndex(), 0, rootItem->childCount() - 1);
    delete rootItem;
    rootItem = NULL;
    rootItem = new RootItem;
    endRemoveRows();
  }


  void TreeModel::saveViewState()
  {
//     cerr << "    TreeModel::saveViewState called\n";
    expandedState->clear();
    selectedState->clear();

    ASSERT(rootItem);
    
    QStack< AbstractTreeItem * > stack;
    stack.push(rootItem);

    while (!stack.isEmpty())
    {
      AbstractTreeItem * item = stack.pop();

      QPair< QString, QString > newPair = qMakePair(item->data().toString(),
          item->parent() ? item->parent()->data().toString() : QString());

      if (item->isExpanded())
      {
//         cerr << "      [" << qPrintable(newPair.first) << "]\t["
//             << qPrintable(newPair.second) << "]\n";

        expandedState->append(newPair);
      }

      if (item->isSelected())
      {
        selectedState->append(newPair);
      }

      for (int i = item->childCount() - 1; i >= 0; i--)
        stack.push(item->childAt(i));
    }
//     cerr << "    TreeModel::saveViewState done\n";
  }


  void TreeModel::loadViewState()
  {
//     cerr << "    TreeModel::loadViewState called... " << expandedState->size() << "\n";
//     for (int i = 0; i < expandedState->size(); i++)
//       cerr << "      [" << qPrintable(expandedState->at(i).first) << "]\t["
//           << qPrintable(expandedState->at(i).second) << "]\n";

//     view->setUpdatesEnabled(false);

    ASSERT(rootItem);

    QStack< AbstractTreeItem * > stack;
    stack.push(rootItem);
    
    while (!stack.isEmpty() && (expandedState->size() || selectedState->size()))
    {
      AbstractTreeItem * item = stack.pop();

      if (item->parent())
      {
        int row = item->row();
        ASSERT(row != -1);
        
        QPair< QString, QString > occurrence = qMakePair(
            item->data().toString(), item->parent()->data().toString());
     
        QModelIndex index = createIndex(row, 0, item);
        if (expandedState->contains(occurrence))
        {
  
//           cerr << "      occurrence: [" << qPrintable(occurrence.first) << "]\t["
//               << qPrintable(occurrence.second) << "]\n";
  
          item->setExpanded(true);
          ASSERT(index.isValid());
          view->expand(index);
//           expandedState->removeOne(occurrence);
        }
  
  
        if (selectedState->contains(occurrence))
        {
          item->setSelected(true);
          view->selectionModel()->select(index, QItemSelectionModel::Select);
  //         selectedState->removeOne(occurrence);
        }
      }

      for (int i = item->childCount() - 1; i >= 0; i--)
        stack.push(item->childAt(i));
    }

    view->setUpdatesEnabled(true);
    emit(dataChanged(QModelIndex(), QModelIndex()));
//     cerr << "    TreeModel::loadViewState done... " << expandedState->size() << "\n";
  }


  AbstractTreeItem * TreeModel::indexToItem(const QModelIndex & index) const
  {
    AbstractTreeItem * item = rootItem;
    
    if (index.isValid())
      return static_cast< AbstractTreeItem * >(index.internalPointer());
      
    return item;
  }
  
  
  QModelIndex TreeModel::itemToIndex(AbstractTreeItem * item) const
  {
    QModelIndex index;
    int row = item->row();
    if (row != -1)
      index = createIndex(row, 0, item);
    
    return index;
  }
}
