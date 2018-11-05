
#ifndef SubTreeProxyModel_h
#define SubTreeProxyModel_h

#include <QIdentityProxyModel>
#include <QPersistentModelIndex>
#include <QSortFilterProxyModel>

class QAbstractProxyModel;
class QObject;
class QStandardItem;
class QVariant;

namespace Isis {
  
/**
 *
 * @author ????-??-?? Ian Humphrey
 * @internal ????-??-?? Ian Humphrey
 *
 *
 */

  class SubTreeProxyModel : public QIdentityProxyModel {
    Q_OBJECT

    public:
      explicit SubTreeProxyModel(QObject *parent = 0);

      QModelIndex mapFromSource(const QModelIndex &sourceIndex) const Q_DECL_OVERRIDE;
      QModelIndex mapToSource(const QModelIndex &proxyIndex) const Q_DECL_OVERRIDE;

      void setSourceModel(QAbstractItemModel *newSourceModel) Q_DECL_OVERRIDE;

      bool setRoot(const QStandardItem *item);


      // Allow reading of the model
      //QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
      //Qt::ItemFlags flags(const QModelIndex &index) const override;

      // Allow re-sizing the model
      //bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Use the method below to change how filtering chooses to accept the row
    // protected:
      // bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

    private:
      QPersistentModelIndex m_root;

  };

};


#endif
