#ifndef PointMeasureTreeModel_H
#define PointMeasureTreeModel_H


// parent
#include "AbstractTreeModel.h"

// parent of inner class
#include <functional>


template <typename A> class QFutureWatcher;
class QString;


namespace Isis
{
  class ControlNet;
  class ControlPoint;
  
  namespace CnetViz
  {
    class TreeView;
    class PointParentItem;
    class RootItem;

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
     */
    class PointMeasureTreeModel : public AbstractTreeModel
    {
        Q_OBJECT

      public:
        PointMeasureTreeModel(ControlNet * cNet, TreeView * v,
            QObject * parent = 0);
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
        class CreateRootItemFunctor : public std::unary_function <
          ControlPoint * const &, PointParentItem * >
        {
          public:
            CreateRootItemFunctor(AbstractTreeModel * tm, QThread * tt);
            CreateRootItemFunctor(const CreateRootItemFunctor &);
            ~CreateRootItemFunctor();
            PointParentItem * operator()(ControlPoint * const &) const;

            static void addToRootItem(QAtomicPointer< RootItem > &,
                PointParentItem * const &);
            CreateRootItemFunctor & operator=(const CreateRootItemFunctor &);


          private:
            int avgCharWidth;
            AbstractTreeModel * treeModel;
            QThread * targetThread;
        };
    };
  }
}

#endif
