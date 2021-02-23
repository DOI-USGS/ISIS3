#ifndef AbstractTreeModel_H
#define AbstractTreeModel_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QObject>


class QFont;
template <typename A> class QFutureWatcher;
class QModelIndex;
class QMutex;
template< typename A, typename B > struct QPair;
class QSize;
class QString;


namespace Isis {
  class AbstractTreeItem;
  class BusyLeafItem;
  class ControlNet;
  class FilterWidget;
  class RootItem;
  class TreeView;

  /**
   * @brief Base class for tree models
   *
   * This class is a base class for models that store data in a tree-like
   * structure. There is also a linked-list for iterating over the filtered
   * items in an efficient manner. This handles the filtering of items and
   * provides an interface for rebuilding.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2016-06-21 Kris Becker - Properly forward declare QPair as struct not class
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   */
  class AbstractTreeModel : public QObject {
      Q_OBJECT

    public:
      enum InterestingItems {
        PointItems = 1,
        MeasureItems = 2,
        ImageItems = 4,
        AllItems = PointItems | MeasureItems | ImageItems
      };
      Q_DECLARE_FLAGS(InterestingItemsFlag, InterestingItems)


    public:
      AbstractTreeModel(ControlNet *controlNet, TreeView *v,
          QObject *parent = 0);
      virtual ~AbstractTreeModel();

      QList< AbstractTreeItem * > getItems(int, int,
          InterestingItemsFlag = AllItems, bool = false);
      QList< AbstractTreeItem * > getItems(AbstractTreeItem *,
          AbstractTreeItem *, InterestingItemsFlag = AllItems, bool = false);
      QList< AbstractTreeItem * > getSelectedItems(
        InterestingItemsFlag = AllItems, bool = false);
      QMutex *getMutex() const;
      int getItemCount(InterestingItemsFlag) const;
      int getTopLevelItemCount() const;
      int getVisibleItemCount(InterestingItemsFlag, bool) const;
      int getVisibleTopLevelItemCount() const;
      TreeView *getView() const;
      void setDrivable(bool drivableStatus);
      bool isDrivable() const;
      bool isFiltering() const;
      bool isRebuilding() const;
      void setRebuilding(bool running) { m_rebuildRunning = running; }
      void setFilter(FilterWidget *newFilter);
      void setGlobalSelection(bool selected, InterestingItemsFlag = AllItems);
      void stopWorking();
      QSize getVisibleSize(int indentation) const;
      int indexOfVisibleItem(AbstractTreeItem const *item,
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
      AbstractTreeModel &operator=(const AbstractTreeModel &);


    private:
      AbstractTreeItem *nextItem(AbstractTreeItem *current,
          InterestingItemsFlag flags, bool ignoreExpansion) const;
      void selectItems(AbstractTreeItem *item, bool select,
          InterestingItemsFlag flags);
      static bool itemIsInteresting(
        AbstractTreeItem *, InterestingItemsFlag);
      int getItemCount(AbstractTreeItem *, InterestingItemsFlag) const;


    private slots:
      void applyFilterDone();
      void rebuildItemsDone();


    protected:
      void clear();
      ControlNet *getControlNetwork() const;
      FilterWidget *getFilterWidget() const;

      QFutureWatcher< QAtomicPointer< RootItem > > *
      getRebuildWatcher() const;

      RootItem *getRootItem() const;


    protected:
      RootItem *rootItem;


    private: // data
      QFutureWatcher< QAtomicPointer< AbstractTreeItem > > * m_filterWatcher;
      QFutureWatcher< QAtomicPointer< RootItem > > * m_rebuildWatcher;
      QList< QPair< QString, QString > > * m_expandedState;
      QList< QPair< QString, QString > > * m_selectedState;
      QMutex *m_mutex;
      BusyLeafItem *m_busyItem;
      TreeView *m_view;
      ControlNet *m_cNet;
      FilterWidget *m_guisFilterWidget;
      FilterWidget *m_localFilterWidgetCopy;
      bool m_drivable;
      bool m_filterAgain;
      bool m_filterRunning;
      bool m_rebuildRunning;
      bool m_frozen;
      bool m_rebuildPending;


    private:
      /**
       * @author ????-??-?? Eric Hyer
       *
       * @internal
       */
      class FilterFunctor
          : public std::unary_function< AbstractTreeItem *const &, bool > {
        public:
          FilterFunctor(FilterWidget *fw);
          FilterFunctor(FilterFunctor const &other);
          ~FilterFunctor();
          bool operator()(AbstractTreeItem *const &) const;
          FilterFunctor &operator=(FilterFunctor const &);
          void filterWorker(AbstractTreeItem *) const;

          static void updateTopLevelLinks(
            QAtomicPointer< AbstractTreeItem > & root,
            AbstractTreeItem *const &item);


        private:
          FilterWidget *m_filter;
      };
  };

  Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractTreeModel::InterestingItemsFlag)

}


#endif
