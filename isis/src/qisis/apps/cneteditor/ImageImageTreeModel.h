#ifndef ImageImageTreeModel_H
#define ImageImageTreeModel_H


// parent
#include "AbstractTreeModel.h"

// parent of inner class
#include <functional>


template <typename A> class QFutureWatcher;
class QString;


namespace Isis
{
  class ControlCubeGraphNode;
  class ControlNet;
  
  namespace CnetViz
  {
    class TreeView;
    class ImageParentItem;

    class ImageImageTreeModel : public AbstractTreeModel
    {
        Q_OBJECT

      public:
        ImageImageTreeModel(ControlNet * cNet, TreeView * v,
            QObject * parent = 0);
        virtual ~ImageImageTreeModel();

        // This is a slot!!!  There is no "pubic slots:" because it has already
        // been marked as a slot in the parent (pure virtual).  Adding the slots
        // keyword here would do nothing except make more work for both MOC and
        // the compiler!
        void rebuildItems();


      private:
        class CreateRootItemFunctor : public std::unary_function <
          ControlCubeGraphNode * const &, ImageParentItem * >
        {
          public:
            CreateRootItemFunctor(AbstractTreeModel * tm, QThread * tt);
            CreateRootItemFunctor(const CreateRootItemFunctor &);
            ~CreateRootItemFunctor();
            ImageParentItem * operator()(ControlCubeGraphNode * const &)
            const;
            CreateRootItemFunctor & operator=(const CreateRootItemFunctor &);

            static void addToRootItem(QAtomicPointer< RootItem > &,
                ImageParentItem * const &);

          private:
            int avgCharWidth;
            AbstractTreeModel * treeModel;
            QThread * targetThread;
        };
    };
  }
}

#endif
