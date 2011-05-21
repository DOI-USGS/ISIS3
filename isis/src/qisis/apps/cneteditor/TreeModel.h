#ifndef TreeModel_H
#define TreeModel_H

#include <QAbstractItemModel>


class QModelIndex;
class QTreeView;

namespace Isis
{
  class AbstractTreeItem;
  class ControlNet;
  class FilterWidget;
  class RootItem;

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
      
      void setFilter(FilterWidget * newFilter);

      void saveViewState();
      void loadViewState();
      
      
    public slots:
      virtual void rebuildItems() = 0;


      // disable copying of this class
    private:
      TreeModel(const TreeModel &);
      const TreeModel & operator=(const TreeModel &);


    protected:
      void clear();


    private:
      AbstractTreeItem * indexToItem(const QModelIndex & index) const;
      QModelIndex itemToIndex(AbstractTreeItem * item) const;
//       int itemToRow(AbstractTreeItem * item) const;


    protected: // data
      ControlNet * cNet;
      QString * headerTitle;
      RootItem * rootItem;
      QList< QPair< QString, QString > > * expandedState;
      QList< QPair< QString, QString > > * selectedState;
      QTreeView * view;
      bool drivable;
      FilterWidget * filter;
  };
}

#endif
