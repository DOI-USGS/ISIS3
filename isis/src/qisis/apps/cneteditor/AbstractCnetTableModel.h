#ifndef AbstractCnetTableModel_H
#define AbstractCnetTableModel_H


#include <QObject>

// can't forward declare the InternalPointerType or InterestingItems enums
#include "AbstractTreeItem.h"
#include "TreeModel.h"


template< class T > class QList;


namespace Isis
{
  class AbstractCnetTableDelegate;
  class BusyLeafItem;
  class CnetTableColumn;
  class CnetTableColumnList;
  class TreeModel;

  class AbstractCnetTableModel : public QObject
  {
      Q_OBJECT

    public:
      AbstractCnetTableModel(TreeModel *, AbstractCnetTableDelegate *);
      virtual ~AbstractCnetTableModel();


      virtual QList< AbstractTreeItem * > getItems(int, int) = 0;
      virtual QList< AbstractTreeItem * > getItems(AbstractTreeItem *,
          AbstractTreeItem *) = 0;
      virtual QList< AbstractTreeItem * > getSelectedItems() = 0;
      virtual bool isFiltering() const;
      virtual bool isSortingEnabled() const;
      virtual void setSortingEnabled(bool);
      virtual CnetTableColumnList * getColumns();
      virtual int getVisibleRowCount() const = 0;
      virtual QString getWarningMessage(AbstractTreeItem const *,
          CnetTableColumn const *, QString valueToSave) const = 0;
      virtual int indexOfVisibleItem(AbstractTreeItem const * item) const = 0;

      virtual const AbstractCnetTableDelegate * getDelegate() const;
      

    public slots:
      virtual void setGlobalSelection(bool selected) = 0;
      virtual void applyFilter();
      virtual void sort();
      virtual void reverseOrder(CnetTableColumn *);
      virtual void updateSort();


    signals:
      void modelModified();
      void filterProgressChanged(int);
      void rebuildProgressChanged(int);
      void filterProgressRangeChanged(int, int);
      void rebuildProgressRangeChanged(int, int);
      void filterCountsChanged(int visibleTopLevelItemCount,
          int topLevelItemCount);
      void treeSelectionChanged(QList<AbstractTreeItem *>);
      void tableSelectionChanged(QList<AbstractTreeItem *>);

      
    protected:
      virtual CnetTableColumnList * createColumns() = 0;
      TreeModel * getDataModel();
      const TreeModel * getDataModel() const;
      virtual QList<AbstractTreeItem *> getSortedItems(int, int,
          TreeModel::InterestingItems);
      virtual QList<AbstractTreeItem *> getSortedItems(
          AbstractTreeItem *, AbstractTreeItem *, TreeModel::InterestingItems);
      void handleTreeSelectionChanged(
          QList< AbstractTreeItem * > newlySelectedItems,
          AbstractTreeItem::InternalPointerType);


    private: // disable copying of this class (these are not implemented)
      AbstractCnetTableModel(AbstractCnetTableModel const &);
      AbstractCnetTableModel & operator=(AbstractCnetTableModel const &);


    private:
      void nullify();


    private:
      TreeModel * dataModel;
      AbstractCnetTableDelegate * delegate;
      QList<AbstractTreeItem *> * sortedRows;
      BusyLeafItem * busyItem;
      CnetTableColumnList * columns;
      bool sortingEnabled;


    private:
      class LessThanFunctor
          : public std::binary_function< AbstractTreeItem * const &,
                                         AbstractTreeItem * const &,
                                         bool >
      {
        public:
          LessThanFunctor(CnetTableColumn const * someColumn);
          LessThanFunctor(LessThanFunctor const &);
          ~LessThanFunctor();
          bool operator()(AbstractTreeItem * const &,
                          AbstractTreeItem * const &);
          LessThanFunctor & operator=(LessThanFunctor const &);


        private:
          CnetTableColumn const * column;
      };
  };
}

#endif

