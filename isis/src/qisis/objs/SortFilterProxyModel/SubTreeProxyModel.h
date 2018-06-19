
#ifndef SubTreeProxyModel_h
#define SubTreeProxyModel_h
#include <QList>
#include <QIdentityProxyModel>
#include <QModelIndex>
#include <QPersistentModelIndex>
#include <QSortFilterProxyModel>

class QAbstractProxyModel;
class QObject;
class QStandardItem;

class QVariant;

namespace Isis {
  
   class ProjectItem;
   class ProjectItemModel;
  //class SubTreeProxyModel : public QIdentityProxyModel {
    class SubTreeProxyModel : public QSortFilterProxyModel  {
    Q_OBJECT

    public:
      explicit SubTreeProxyModel(QObject *parent = 0);

      //QModelIndex mapFromSource(const QModelIndex &sourceIndex) const Q_DECL_OVERRIDE;
      //QModelIndex mapToSource(const QModelIndex &proxyIndex) const Q_DECL_OVERRIDE;

      //void setSourceModel(ProjectItemModel *newSourceModel) Q_DECL_OVERRIDE;
      void setSourceModel(ProjectItemModel *newSourceModel);

      bool setRoot(const QStandardItem *item);

      void setSelectedItems(QList<ProjectItem*> selected);


      // Allow reading of the model
      //QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
      //Qt::ItemFlags flags(const QModelIndex &index) const override;

      // Allow re-sizing the model
      //bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Use the method below to change how filtering chooses to accept the row
     protected:
       bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

    private:
      //QList<ProjectItem *> selectedItems;
      QList<QModelIndex> selectedIndices;
      QList<int> selectedIndexRows;
      QPersistentModelIndex m_root;
      QModelIndex root;


  };

};


#endif
