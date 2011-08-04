#ifndef TreeModel_H
#define TreeModel_H

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
  class AbstractTreeItem;
  class BusyLeafItem;
  class CnetTreeView;
  class ControlNet;
  class FilterWidget;
  class RootItem;

  class TreeModel : public QObject
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
      TreeModel(ControlNet * controlNet, CnetTreeView * v,
          QObject * parent = 0);
      virtual ~TreeModel();

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
      CnetTreeView * getView() const;
      void setDrivable(bool drivableStatus);
      bool isDrivable() const;
      bool isFiltering() const;
      void setFilter(FilterWidget * newFilter);
      void saveViewState();
      void setGlobalSelection(bool selected, InterestingItemsFlag = AllItems);
      void loadViewState();
      void stopWorking();
      QSize getVisibleSize(int indentation) const;
      int indexOfVisibleItem(AbstractTreeItem const * item,
                             InterestingItemsFlag = AllItems,
                             bool = false) const;
      

    public slots:
      void applyFilter();


    signals:
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
      TreeModel(const TreeModel &);
      TreeModel & operator=(const TreeModel &);


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
      QFutureWatcher< QAtomicPointer< RootItem > > * getRebuildWatcher() const;
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
      CnetTreeView * view;
      ControlNet * cNet;
      FilterWidget * guisFilterWidget;
      FilterWidget * localFilterWidgetCopy;
      bool drivable;
      bool filterAgain;
      bool filterRunning;


    private:
      class FilterFunctor
        : public std::unary_function< AbstractTreeItem * const &, bool >
      {
        public:
          FilterFunctor(FilterWidget * fw);
          ~FilterFunctor();
          bool operator()(AbstractTreeItem * const &) const;
          void filterWorker(AbstractTreeItem *) const;

          static void updateTopLevelLinks(
            QAtomicPointer< AbstractTreeItem > & root,
            AbstractTreeItem * const & item);


        private:
          FilterWidget * filter;
      };
  };

  Q_DECLARE_OPERATORS_FOR_FLAGS(TreeModel::InterestingItemsFlag)
}

#endif

