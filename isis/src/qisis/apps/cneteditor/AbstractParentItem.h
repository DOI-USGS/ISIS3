#ifndef AbstractParentItem_H
#define AbstractParentItem_H


#include "AbstractTreeItem.h"


template< typename T > class QList;
class QVariant;


namespace Isis
{
  class AbstractParentItem : public virtual AbstractTreeItem
  {
    public:
      AbstractParentItem(AbstractTreeItem * parent = 0);
      virtual ~AbstractParentItem();

      AbstractTreeItem * childAt(int row) const;
      int indexOf(AbstractTreeItem * child) const;
      int childCount() const;
      virtual void addChild(AbstractTreeItem * child);
      virtual void removeChild(int row);


    private: // disable copying of this class
      AbstractParentItem(const AbstractParentItem &);
      const AbstractParentItem & operator=(const AbstractParentItem &);


    private:
      QList< AbstractTreeItem * > * children;
  };
}

#endif
