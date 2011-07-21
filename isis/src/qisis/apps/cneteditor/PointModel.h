#ifndef PointModel_H
#define PointModel_H


// parent
#include "TreeModel.h"

// parent of inner class
#include <functional>


template <typename A> class QFutureWatcher;
class QString;


namespace Isis
{
  class CnetView;
  class ControlNet;
  class ControlPoint;
  class PointParentItem;
  class RootItem;

  class PointModel : public TreeModel
  {
      Q_OBJECT

    public:
      PointModel(Isis::ControlNet * cNet, CnetView * v, QObject * parent = 0);
      virtual ~PointModel();

      // These are slots!!!  There is no "pubic slots:" because it has already
      // been done in the parent (they are pure virtual).  Adding the slots
      // keyword here would do nothing except make more work for both MOC and
      // the compiler!
      void rebuildItems();


    private:
      class CreateRootItemFunctor : public std::unary_function <
          ControlPoint * const &, PointParentItem * >
      {
        public:
          CreateRootItemFunctor(TreeModel * tm);
          CreateRootItemFunctor(const CreateRootItemFunctor &);
          ~CreateRootItemFunctor();
          PointParentItem * operator()(ControlPoint * const &) const;

          static void addToRootItem(QAtomicPointer< RootItem > &,
              PointParentItem * const &);
          CreateRootItemFunctor & operator=(const CreateRootItemFunctor &);


        private:
          int avgCharWidth;
          TreeModel * treeModel;
      };
  };
}

#endif
