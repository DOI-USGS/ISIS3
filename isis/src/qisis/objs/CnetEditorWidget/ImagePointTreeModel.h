#ifndef ImagePointTreeModel_H
#define ImagePointTreeModel_H

// parent
#include "AbstractTreeModel.h"

// parent of inner class
#include <functional>


template <typename A> class QFutureWatcher;
class QString;


namespace Isis {
  class ControlCubeGraphNode;
  class ControlNet;

  namespace CnetViz {
    class TreeView;
    class ImageParentItem;

    /**
     * @brief Tree model for images and control points
     *
     * This class represents a model that provides access to images and the
     * control points that are contained within. The tree structure is designed
     * such that images are parent nodes with control points as children (leaf)
     * nodes.
     *
     * This class also provides functionality for the multi-threaded rebuilding
     * of the image-point tree model structure, which is necessary when certain
     * types of changes are made to the underlying control network.
     *
     * @author ????-??-?? Eric Hyer
     *
     * @internal
     *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
     */
    class ImagePointTreeModel : public AbstractTreeModel {
        Q_OBJECT

      public:
        ImagePointTreeModel(ControlNet *cNet, TreeView *v,
            QObject *parent = 0);
        virtual ~ImagePointTreeModel();

        // This is a slot!!!  There is no "pubic slots:" because it has already
        // been marked as a slot in the parent (pure virtual).  Adding the slots
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
            ControlCubeGraphNode *const &, ImageParentItem * > {
          public:
            CreateRootItemFunctor(AbstractTreeModel *tm, QThread *tt);
            CreateRootItemFunctor(const CreateRootItemFunctor &);
            ~CreateRootItemFunctor();
            ImageParentItem *operator()(ControlCubeGraphNode *const &) const;
            CreateRootItemFunctor &operator=(const CreateRootItemFunctor &);

            static void addToRootItem(QAtomicPointer< RootItem > &,
                ImageParentItem *const &);

          private:
            int m_avgCharWidth;
            AbstractTreeModel *m_treeModel;
            QThread *m_targetThread;
        };
    };
  }
}

#endif
