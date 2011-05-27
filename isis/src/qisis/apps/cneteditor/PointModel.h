#ifndef PointModel_H
#define PointModel_H


// parent
#include "TreeModel.h"

// parent of inner class
#include <functional>


template <typename A> class QFutureWatcher;
class QString;
class QTreeView;


namespace Isis
{
  class ControlNet;
  class ControlPoint;
  class PointParentItem;
  class RootItem;

  class PointModel : public TreeModel
  {
      Q_OBJECT

    public:
      PointModel(Isis::ControlNet * cNet, QString name, QTreeView * tv,
          QObject * parent = 0);
      virtual ~PointModel();

      // This is a slot!!!  There is no "pubic slots:" because it has already
      // been marked as a slot in the parent (pure virtual).  Adding the slots
      // keyword here would do nothing except make more work for both MOC and
      // the compiler!
      void rebuildItems();
      
      
    private slots:
      void rebuildItemsDone();
      
      
    private:
      QFutureWatcher< QAtomicPointer< RootItem > > * watcher;
      
      
    private:
      class CreateRootItemFunctor : public std::unary_function<
          ControlPoint * const &, PointParentItem * >
      {
        public:
          CreateRootItemFunctor(FilterWidget * fw);
          PointParentItem * operator()(ControlPoint * const &) const;
          
          static void addToRootItem(QAtomicPointer< RootItem > &,
              PointParentItem * const &);
          
        private:
          FilterWidget * filter;
      };
  };
}

#endif
