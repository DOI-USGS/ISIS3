#ifndef TreeModel_H
#define TreeModel_H

#include <QAbstractItemModel>


class QModelIndex;

namespace Isis
{
  class ControlNet;
  class TreeItem;

  class TreeModel : public QAbstractItemModel
  {
      Q_OBJECT

    public:
      TreeModel(ControlNet * controlNet, QString name, QObject * parent = 0);
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


    public slots:
      virtual void rebuildItems() = 0;


    protected:
      void clearParentItems();


    private:
      TreeItem * getItem(const QModelIndex & index) const;


    protected: // data
      ControlNet * cNet;
      QString * headerTitle;
      QList< TreeItem * > * parentItems;
      bool drivable;
  };
}

#endif
