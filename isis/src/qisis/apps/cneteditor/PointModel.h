#ifndef PointModel_H
#define PointModel_H


// parent
#include "TreeModel.h"

// ummm, yeah
#include <functional>

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
      
      
    private:
      class CreateRootItemFunctor : public std::unary_function<
          ControlPoint * const &, PointParentItem * >

      {
        public:
          CreateRootItemFunctor(FilterWidget * fw);
          PointParentItem * operator()(ControlPoint * const &) const;
          
          static void addToRootItem(RootItem *&, PointParentItem * const &);
          
        private:
          FilterWidget * filter;
          static bool rootInstantiated;
      };
  };
}

#endif
