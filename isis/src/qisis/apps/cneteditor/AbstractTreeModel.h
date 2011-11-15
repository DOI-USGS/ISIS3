#ifndef AbstractTreeModel_H
#define AbstractTreeModel_H

#include <QObject>


class QFont;
template <typename A> class QFutureWatcher;
class QModelIndex;
class QMutex;
template< typename A, typename B > class QPair;
class QSize;
class QString;


namespace Isis
{
  class ControlNet;
  
  namespace CnetViz
  {
    class AbstractTreeItem;
    class BusyLeafItem;
    class TreeView;
    class FilterWidget;
    class RootItem;

    class AbstractTreeModel : public QObject
    {
        Q_OBJECT

      public:
        enum InterestingItems
        {
          PointItems = 1,
          MeasureItems = 2,
          SerialItems = 4,
          AllItems = PointItems | MeasureItems | SerialItems
        };
        Q_DECLARE_FLAGS(InterestingItemsFlag, InterestingItems)


      public:
        AbstractTreeModel(ControlNet * controlNet, TreeView * v,
            QObject * parent = 0);
        virtual ~AbstractTreeModel();

        QList< AbstractTreeItem * > getItems(int, int,
            InterestingItemsFlag = AllItems, bool = false);
        QList< AbstractTreeItem * > getItems(AbstractTreeItem *,
            AbstractTreeItem *, InterestingItemsFlag = AllItems, bool = false);
        QList< AbstractTreeItem * > getSelectedItems(
            InterestingItemsFlag = AllItems, bool = false);
        QMutex * getMutex() const;
        int getItemCount(InterestingItemsFlag) const;
        int getTopLevelItemCount() const;
        int getVisibleItemCount(InterestingItemsFlag, bool) const;
        int getVisibleTopLevelItemCount() const;
        TreeView * getView() const;
        void setDrivable(bool drivableStatus);
        bool isDrivable() const;
        bool isFiltering() const;
        bool isRebuilding() const;
        void setRebuilding(bool running) { rebuildRunning = running; }
        void setFilter(FilterWidget * newFilter);
        void setGlobalSelection(bool selected, InterestingItemsFlag = AllItems);
        void stopWorking();
        QSize getVisibleSize(int indentation) const;
        int indexOfVisibleItem(AbstractTreeItem const * item,
                              InterestingItemsFlag = AllItems,
                              bool = false) const;
        void setFrozen(bool);
        bool isFrozen() const;
        void queueRebuild();
        

      public slots:
        void applyFilter();


      signals:
        void cancelSort();
        void modelModified();
        void filterProgressChanged(int);
        void filterProgressRangeChanged(int, int);
        void rebuildProgressChanged(int);
        void rebuildProgressRangeChanged(int, int);
        void treeSelectionChanged(QList<AbstractTreeItem *>);
        void tableSelectionChanged(QList<AbstractTreeItem *>);

        /**
         * This signal is emitted after filtering to provide the number of
         * visible top-level items remaining after the filter was applied,
         * as well as the total number of items that were possible
         */
        void filterCountsChanged(int visibleTopLevelItemCount,
            int topLevelItemCount);


      public:
        virtual void rebuildItems() = 0;


      private:
        AbstractTreeModel(const AbstractTreeModel &);
        AbstractTreeModel & operator=(const AbstractTreeModel &);


      private:
        AbstractTreeItem * nextItem(AbstractTreeItem * current,
            InterestingItemsFlag flags, bool ignoreExpansion) const;
        void selectItems(AbstractTreeItem * item, bool select,
            InterestingItemsFlag flags);
        static bool itemIsInteresting(
            AbstractTreeItem *, InterestingItemsFlag);
        int getItemCount(AbstractTreeItem *, InterestingItemsFlag) const;


      private slots:
        void applyFilterDone();
        void rebuildItemsDone();


      protected:
        void clear();
        ControlNet * getControlNetwork() const;
        FilterWidget * getFilterWidget() const;
        
        QFutureWatcher< QAtomicPointer< RootItem > > *
            getRebuildWatcher() const;
            
        RootItem * getRootItem() const;


      protected:
        RootItem * rootItem;


      private: // data
        QFutureWatcher< QAtomicPointer< AbstractTreeItem > > * filterWatcher;
        QFutureWatcher< QAtomicPointer< RootItem > > * rebuildWatcher;
        QList< QPair< QString, QString > > * expandedState;
        QList< QPair< QString, QString > > * selectedState;
        QMutex * mutex;
        BusyLeafItem * busyItem;
        TreeView * view;
        ControlNet * cNet;
        FilterWidget * guisFilterWidget;
        FilterWidget * localFilterWidgetCopy;
        bool drivable;
        bool filterAgain;
        bool filterRunning;
        bool rebuildRunning;
        bool frozen;
        bool rebuildPending;


      private:
        class FilterFunctor
            : public std::unary_function< AbstractTreeItem * const &, bool >
        {
          public:
            FilterFunctor(FilterWidget * fw);
            FilterFunctor(FilterFunctor const & other);
            ~FilterFunctor();
            bool operator()(AbstractTreeItem * const &) const;
            FilterFunctor & operator=(FilterFunctor const &);
            void filterWorker(AbstractTreeItem *) const;

            static void updateTopLevelLinks(
              QAtomicPointer< AbstractTreeItem > & root,
              AbstractTreeItem * const & item);


          private:
            FilterWidget * filter;
        };
    };

    Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractTreeModel::InterestingItemsFlag)
  }
}

#endif
