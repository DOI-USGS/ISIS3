#ifndef ImageImageTreeModel_H
#define ImageImageTreeModel_H


// parent
#include "AbstractTreeModel.h"

// parent of inner class
#include <functional>


template <typename A> class QFutureWatcher;
class QString;


namespace Isis {
  class ControlCubeGraphNode;
  class ControlNet;
  class ImageParentItem;
  class TreeView;

  /**
   * @brief Tree model for images and images
   *
   * This class represents a model that provides access to images that are
   * connected together through a control point. The tree structure is
   * designed such that images are parent nodes with images as children (leaf)
   * nodes.
   *
   * This class also provides functionality for the multi-threaded rebuilding
   * of the image-image tree model structure, which is necessary when certain
   * types of changes are made to the underlying control network.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   */
  class ImageImageTreeModel : public AbstractTreeModel {
      Q_OBJECT

    public:
      ImageImageTreeModel(ControlNet *cNet, TreeView *v,
          QObject *parent = 0);
      virtual ~ImageImageTreeModel();

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
       *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
       */
      class CreateRootItemFunctor : public std::unary_function <
          ControlCubeGraphNode *const &, ImageParentItem * > {
        public:
          CreateRootItemFunctor(AbstractTreeModel *tm, QThread *tt);
          CreateRootItemFunctor(const CreateRootItemFunctor &);
          ~CreateRootItemFunctor();
          ImageParentItem *operator()(ControlCubeGraphNode *const &)
          const;
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

#endif
