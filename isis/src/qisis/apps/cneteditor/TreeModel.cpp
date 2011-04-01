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

using std::cerr;


namespace Isis
{
  TreeModel::TreeModel(ControlNet * controlNet, QString name, QTreeView * tv,
      QObject * parent) : QAbstractItemModel(parent), cNet(controlNet), view(tv)
  {
    ASSERT(cNet);

    headerTitle = NULL;
    parentItems = NULL;
    expandedState = NULL;
    selectedState = NULL;

    headerTitle = new QString(name);
    parentItems = new QList< AbstractTreeItem * >;
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

    if (parentItems)
    {
      for (int i = 0; i < parentItems->size(); i++)
      {
        if (parentItems->at(i))
        {
          delete(*parentItems)[i];
          (*parentItems)[i] = NULL;
        }
      }
      delete parentItems;
      parentItems = NULL;
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
    if (role != Qt::DisplayRole)
      return QVariant();

    if (!index.isValid())
      return QVariant();

    if (index.column() != 0)
      return QVariant();

    AbstractTreeItem * item = getItem(index);
    ASSERT(item);

    AbstractTreeItem * parentItem = item->parent();
    if (parentItem)
    {
      int row = index.row();
      return parentItem->childAt(row)->data();
    }
    else
    {
      return item->data();
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
        AbstractTreeItem * childItem = static_cast< AbstractTreeItem * >(
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
      AbstractTreeItem * childItem = getItem(index);
      AbstractTreeItem * parentItem = childItem->parent();

      if (parentItem)
      {
        AbstractTreeItem * grandParentItem = parentItem->parent();
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
      AbstractTreeItem * parentItem = getItem(parent);

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

    beginRemoveRows(QModelIndex(), 0, parentItems->size() - 1);
    for (int i = 0; i < parentItems->size(); i++)
    {
      if (parentItems->at(i))
      {
        delete(*parentItems)[i];
        (*parentItems)[i] = NULL;
      }
    }
    parentItems->clear();
    endRemoveRows();

//     view->reset();
//     view->repaint();
//     char ch;
//     std::cin >> ch;
  }


  void TreeModel::saveViewState()
  {
    cerr << "    TreeModel::saveViewState called\n";
    expandedState->clear();
    selectedState->clear();
    QStack< AbstractTreeItem * > stack;

    cerr << "      " << parentItems->at(1) << "\t" << parentItems->at(1)->isExpanded() << "\n";

    for (int i = parentItems->size() - 1; i >= 0; i--)
    {
      ASSERT(parentItems->at(i));
      stack.push((*parentItems)[i]);
    }

    while (!stack.isEmpty())
    {
      AbstractTreeItem * item = stack.pop();

      QPair< QString, QString > newPair = qMakePair(item->data().toString(),
          item->parent() ? item->parent()->data().toString() : QString());

      if (item->isExpanded())
      {
        cerr << "      [" << qPrintable(newPair.first) << "]\t["
            << qPrintable(newPair.second) << "]\n";

        expandedState->append(newPair);
      }

      if (item->isSelected())
      {
        selectedState->append(newPair);
      }

      for (int i = item->childCount() - 1; i >= 0; i--)
        stack.push(item->childAt(i));
    }
  }


  void TreeModel::loadViewState()
  {
    cerr << "    TreeModel::loadViewState called... " << expandedState->size() << "\n";
    for (int i = 0; i < expandedState->size(); i++)
      cerr << "      [" << qPrintable(expandedState->at(i).first) << "]\t["
          << qPrintable(expandedState->at(i).second) << "]\n";

//     view->setUpdatesEnabled(false);

    QStack< AbstractTreeItem * > stack;

    for (int i = parentItems->size() - 1; i >= 0; i--)
    {
      ASSERT(parentItems->at(i));
      stack.push((*parentItems)[i]);
    }

    while (!stack.isEmpty() && (expandedState->size() || selectedState->size()))
    {
      AbstractTreeItem * item = stack.pop();

      QPair< QString, QString > occurrence = qMakePair(item->data().toString(),
          item->parent() ? item->parent()->data().toString() : QString());

      int row = item->parent() ? item->row() : parentItems->indexOf(item);
      QModelIndex index = createIndex(row, 0, item);
      if (expandedState->contains(occurrence))
      {

        cerr << "      occurrence: [" << qPrintable(occurrence.first) << "]\t["
            << qPrintable(occurrence.second) << "]\n";

        item->setExpanded(true);
        ASSERT(index.isValid());
        view->expand(index);
//         expandedState->removeOne(occurrence);
      }


      if (selectedState->contains(occurrence))
      {
        item->setSelected(true);
        view->selectionModel()->select(index, QItemSelectionModel::Select);
//         selectedState->removeOne(occurrence);
      }

      for (int i = item->childCount() - 1; i >= 0; i--)
        stack.push(item->childAt(i));
    }

//     view->setUpdatesEnabled(true);
//     emit(dataChanged(QModelIndex(), QModelIndex()));
  }


  AbstractTreeItem * TreeModel::getItem(const QModelIndex & index) const
  {
    if (!index.isValid())
    {
      iString msg = "index invalid!";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return static_cast< AbstractTreeItem * >(index.internalPointer());
  }
}
