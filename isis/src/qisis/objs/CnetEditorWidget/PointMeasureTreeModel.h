#ifndef PointMeasureTreeModel_H
#define PointMeasureTreeModel_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// parent
#include "AbstractTreeModel.h"

// parent of inner class
#include <functional>


template <typename A> class QFutureWatcher;
class QString;


namespace Isis {
  class ControlNet;
  class ControlPoint;
  class PointParentItem;
  class RootItem;
  class TreeView;


  /**
   * @brief Tree model for control points and control measures
   *
   * This class represents a model that provides access to control points and
   * control measures in a tree-like fashion. The tree structure is designed
   * such that control points are parent nodes with control measures as
   * children (leaf) nodes.
   *
   * This class also provides functionality for the multi-threaded rebuilding
   * of the point-measure tree model structure, which is necessary when
   * certain types of changes are made to the underlying control network.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   */
  class PointMeasureTreeModel : public AbstractTreeModel {
      Q_OBJECT

    public:
      PointMeasureTreeModel(ControlNet *cNet, TreeView *v,
          QObject *parent = 0);
      virtual ~PointMeasureTreeModel();

      // These are slots!!!  There is no "pubic slots:" because it has already
      // been done in the parent (they are pure virtual).  Adding the slots
      // keyword here would do nothing except make more work for both MOC and
      // the compiler!
      void rebuildItems();


    private:
      /**
       * @author ????-??-?? Eric Hyer
       *
       * @internal
       */
      class CreateRootItemFunctor : public std::function <PointParentItem *(ControlPoint *const &)> {
        public:
          CreateRootItemFunctor(AbstractTreeModel *tm, QThread *tt);
          CreateRootItemFunctor(const CreateRootItemFunctor &);
          ~CreateRootItemFunctor();
          PointParentItem *operator()(ControlPoint *const &) const;

          static void addToRootItem(QAtomicPointer< RootItem > &,
              PointParentItem *const &);
          CreateRootItemFunctor &operator=(const CreateRootItemFunctor &);


        private:
          int m_avgCharWidth;
          AbstractTreeModel *m_treeModel;
          QThread *m_targetThread;
      };
  };
}

#endif
