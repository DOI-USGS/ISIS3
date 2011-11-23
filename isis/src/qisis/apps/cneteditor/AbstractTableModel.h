#ifndef AbstractTableModel_H
#define AbstractTableModel_H


#include <QExplicitlySharedDataPointer>
#include <QObject>
#include <qtconcurrentexception.h>

// can't forward declare the InternalPointerType or InterestingItems enums
#include "AbstractTreeItem.h"
#include "AbstractTreeModel.h"


template< typename T > class QFutureWatcher;
class QTimer;
template< typename T > class QList;


namespace Isis
{
  namespace CnetViz
  {
    class AbstractTableDelegate;
    class BusyLeafItem;
    class TableColumn;
    class TableColumnList;
    class AbstractTreeModel;

    class AbstractTableModel : public QObject
    {
      class LessThanFunctor;

        Q_OBJECT

      public:
        AbstractTableModel(AbstractTreeModel *, AbstractTableDelegate *);
        virtual ~AbstractTableModel();


        virtual QList< AbstractTreeItem * > getItems(int, int) = 0;
        virtual QList< AbstractTreeItem * > getItems(AbstractTreeItem *,
            AbstractTreeItem *) = 0;
        virtual QList< AbstractTreeItem * > getSelectedItems() = 0;
        virtual int getVisibleRowCount() const = 0;
        virtual QString getWarningMessage(AbstractTreeItem const *,
            TableColumn const *, QString valueToSave) const = 0;
        virtual int indexOfVisibleItem(AbstractTreeItem const * item) const = 0;

        virtual bool isSorting() const;
        virtual bool isFiltering() const;
        virtual bool sortingIsEnabled() const;
        virtual void setSortingEnabled(bool);
        virtual TableColumnList * getColumns();
        virtual const AbstractTableDelegate * getDelegate() const;
        

      public slots:
        virtual void setGlobalSelection(bool selected) = 0;
        virtual void applyFilter();
        virtual void sort();
        virtual void reverseOrder(TableColumn *);
        virtual void updateSort();
        virtual void rebuildSort();


      signals:
        void modelModified();
        void filterProgressChanged(int);
        void rebuildProgressChanged(int);
        void sortProgressChanged(int);
        void filterProgressRangeChanged(int, int);
        void rebuildProgressRangeChanged(int, int);
        void sortProgressRangeChanged(int, int);
        void filterCountsChanged(int visibleRows, int totalRows);
        void treeSelectionChanged(QList<AbstractTreeItem *>);
        void tableSelectionChanged(QList<AbstractTreeItem *>);

        
      protected:
        virtual TableColumnList * createColumns() = 0;
        AbstractTreeModel * getDataModel();
        const AbstractTreeModel * getDataModel() const;
        virtual QList< AbstractTreeItem * > getSortedItems(int, int,
            AbstractTreeModel::InterestingItems);
        virtual QList< AbstractTreeItem * > getSortedItems(
            AbstractTreeItem *, AbstractTreeItem *,
            AbstractTreeModel::InterestingItems);
        void handleTreeSelectionChanged(
            QList< AbstractTreeItem * > newlySelectedItems,
            AbstractTreeItem::InternalPointerType);


      private: // disable copying of this class (these are not implemented)
        AbstractTableModel(AbstractTableModel const &);
        AbstractTableModel & operator=(AbstractTableModel const &);


      private slots:
        void cancelSort();
        void sortStatusUpdated();
        void sortFinished();


      private:
        QList< AbstractTreeItem * > doSort(QList< AbstractTreeItem * >);
        void nullify();
        void setSorting(bool sorting);


      private:
        AbstractTreeModel * dataModel;
        AbstractTableDelegate * delegate;
        QList< AbstractTreeItem * > * sortedItems;
        BusyLeafItem * busyItem;
        TableColumnList * columns;
        QTimer * sortStatusPoller;
        LessThanFunctor * lessThanFunctor;
        bool sortingEnabled;
        bool sorting;
        QFutureWatcher< QList< AbstractTreeItem * > > * sortingWatcher;
        
        static const int SORT_UPDATE_FREQUENCY = 50;  // in milliseconds


      private:
        class LessThanFunctorData;

        class LessThanFunctor : public std::binary_function<
            AbstractTreeItem * const &, AbstractTreeItem * const &, bool >
        {
          public:
            LessThanFunctor(TableColumn const * someColumn);
            LessThanFunctor(LessThanFunctor const &);
            ~LessThanFunctor();

            int getCompareCount() const;
            void interrupt();
            bool interrupted();
            void reset();

            bool operator()(AbstractTreeItem * const &,
                            AbstractTreeItem * const &);
            LessThanFunctor & operator=(LessThanFunctor const &);


          private:
            TableColumn const * column;
            QExplicitlySharedDataPointer<LessThanFunctorData> sharedData;
        };

        // For explicit sharing of the comparison counter between multiple
        // copies of a LessThanFunctor object. This bypasses the need for a
        // static member in LessThanFunctor.
        class LessThanFunctorData : public QSharedData
        {
          public:
            LessThanFunctorData();
            LessThanFunctorData(LessThanFunctorData const &);
            ~LessThanFunctorData();

            int getCompareCount() const;
            void incrementCompareCount();
            
            void setInterrupted(bool);
            bool interrupted();


          private:
            QAtomicInt compareCount;
            QAtomicInt interruptFlag;
        };
        
        
        class SortingCanceledException : public QtConcurrent::Exception
        {
          public:
            void raise() const { throw *this; }
            Exception * clone() const
            {
              return new SortingCanceledException(*this);
            }
        };
    };
  }
}

#endif
