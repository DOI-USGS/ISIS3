#ifndef SerialModel_H
#define SerialModel_H

// parent
#include "TreeModel.h"

// parent of inner class
#include <functional>


template <typename A> class QFutureWatcher;
class QString;


namespace Isis
{
  class CnetTreeView;
  class ControlCubeGraphNode;
  class ControlNet;
  class SerialParentItem;

  class SerialModel : public TreeModel
  {
      Q_OBJECT

    public:
      SerialModel(Isis::ControlNet * cNet, CnetTreeView * v,
          QObject * parent = 0);
      virtual ~SerialModel();

      // This is a slot!!!  There is no "pubic slots:" because it has already
      // been marked as a slot in the parent (pure virtual).  Adding the slots
      // keyword here would do nothing except make more work for both MOC and
      // the compiler!
      void rebuildItems();


    private:
      class CreateRootItemFunctor : public std::unary_function <
        ControlCubeGraphNode * const &, SerialParentItem * >
      {
        public:
          CreateRootItemFunctor(TreeModel * tm, QThread * tt);
          CreateRootItemFunctor(const CreateRootItemFunctor &);
          ~CreateRootItemFunctor();
          SerialParentItem * operator()(ControlCubeGraphNode * const &) const;
          CreateRootItemFunctor & operator=(const CreateRootItemFunctor &);

          static void addToRootItem(QAtomicPointer< RootItem > &,
              SerialParentItem * const &);

        private:
          int avgCharWidth;
          TreeModel * treeModel;
          QThread * targetThread;
      };
  };
}

#endif
