#ifndef AbstractParentItem_H
#define AbstractParentItem_H


#include "AbstractTreeItem.h"


template< typename T > class QList;
class QVariant;


namespace Isis
{
  namespace CnetViz
  {
    class AbstractParentItem : public virtual AbstractTreeItem
    {
      public:
        AbstractParentItem(AbstractTreeItem * parent = 0);
        virtual ~AbstractParentItem();

        virtual AbstractTreeItem * childAt(int row) const;
        virtual QList< AbstractTreeItem * > getChildren() const;
        virtual AbstractTreeItem * getFirstVisibleChild() const;
        virtual AbstractTreeItem * getLastVisibleChild() const;
        virtual int indexOf(AbstractTreeItem * child) const;
        virtual int childCount() const;
        virtual void addChild(AbstractTreeItem * child);
        virtual void setFirstVisibleChild(AbstractTreeItem * child);
        virtual void setLastVisibleChild(AbstractTreeItem * child);


      private: // disable copying of this class
        AbstractParentItem(const AbstractParentItem &);
        const AbstractParentItem & operator=(const AbstractParentItem &);


      private:
        QList< AbstractTreeItem * > * children;
        AbstractTreeItem * firstVisibleChild;
        AbstractTreeItem * lastVisibleChild;
    };
  }
}

#endif
