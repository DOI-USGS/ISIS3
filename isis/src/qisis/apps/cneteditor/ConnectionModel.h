#ifndef ConnectionModel_H
#define ConnectionModel_H

#include <QAbstractItemModel>


class QVariant;
class QModelIndex;


namespace Isis
{
  class ControlNet;
  class PointParentItem;
  class TreeItem;

  class ConnectionModel : public QAbstractItemModel
  {
      Q_OBJECT

    public:
      ConnectionModel(Isis::ControlNet * cNet, QObject * parent = 0);
      virtual ~ConnectionModel();

      QVariant data(const QModelIndex & index, int role) const;
      QVariant headerData(int section, Qt::Orientation orientation,
          int role = Qt::DisplayRole) const;

      QModelIndex index(int row, int column,
          const QModelIndex & parent = QModelIndex()) const;

      QModelIndex parent(const QModelIndex & index) const;


//       Qt::ItemFlags flags(const QModelIndex & index) const;
      int rowCount(const QModelIndex & parent = QModelIndex()) const;
      int columnCount(const QModelIndex & parent = QModelIndex()) const;


    private:
      TreeItem * getItem(const QModelIndex & index) const;


    private:
      Isis::ControlNet * cNet;
      QList< TreeItem * > * parentItems;
  };

}

#endif

