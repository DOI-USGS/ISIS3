#ifndef TreeModel_H
#define TreeModel_H

#include <QAbstractItemModel>


class QModelIndex;
class QTreeView;

namespace Isis
{
  class ControlNet;
  class AbstractTreeItem;

  class TreeModel : public QAbstractItemModel
  {
      Q_OBJECT

    public:
      TreeModel(ControlNet * controlNet, QString name, QTreeView * tv,
          QObject * parent = 0);
      virtual ~TreeModel();

      QVariant data(const QModelIndex & index, int role) const;
      QVariant headerData(int section, Qt::Orientation orientation,
          int role = Qt::DisplayRole) const;

      QModelIndex index(int row, int column,
          const QModelIndex & parent = QModelIndex()) const;

      QModelIndex parent(const QModelIndex & index) const;

      int rowCount(const QModelIndex & parent) const;
      int columnCount(const QModelIndex & parent) const;

      Qt::ItemFlags flags(const QModelIndex & index) const;

      void setDrivable(bool drivableStatus);
      bool isDrivable() { return drivable; }

      virtual void rebuildItems() = 0;
      void saveViewState();
      void loadViewState();


      // disable copying of this class
    private:
      TreeModel(const TreeModel &);
      const TreeModel & operator=(const TreeModel &);


    protected:
      void clearParentItems();


    private:
      AbstractTreeItem * getItem(const QModelIndex & index) const;


    protected: // data
      ControlNet * cNet;
      QString * headerTitle;
      QList< AbstractTreeItem * > * parentItems;
      QList< QPair< QString, QString > > * expandedState;
      QList< QPair< QString, QString > > * selectedState;
      QTreeView * view;
      bool drivable;
  };
}

#endif
