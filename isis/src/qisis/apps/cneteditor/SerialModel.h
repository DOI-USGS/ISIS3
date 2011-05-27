#ifndef SerialModel_H
#define SerialModel_H

// parent
#include "TreeModel.h"

// parent of inner class
#include <functional>


template <typename A> class QFutureWatcher;
class QString;
class QTreeView;


namespace Isis
{
  class ControlCubeGraphNode;
  class ControlNet;
  class SerialParentItem;

  class SerialModel : public TreeModel
  {
      Q_OBJECT

    public:
      SerialModel(Isis::ControlNet * cNet, QString name, QTreeView * tv,
          QObject * parent = 0);
      virtual ~SerialModel();

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
          ControlCubeGraphNode * const &, SerialParentItem * >
      {
        public:
          CreateRootItemFunctor(FilterWidget * fw);
          SerialParentItem * operator()(ControlCubeGraphNode * const &) const;
          
          static void addToRootItem(QAtomicPointer< RootItem > &,
              SerialParentItem * const &);
          
        private:
          FilterWidget * filter;
      };
  };
}

#endif
