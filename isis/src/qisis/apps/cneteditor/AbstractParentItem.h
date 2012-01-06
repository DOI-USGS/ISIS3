#ifndef AbstractParentItem_H
#define AbstractParentItem_H


#include "AbstractTreeItem.h"


template< typename T > class QList;
class QVariant;


namespace Isis
{
  namespace CnetViz
  {

    /**
     * @brief Base class for an item that is a parent in the tree
     *
     * This class represents an item in the tree that is a parent (i.e. has
     * children items). Item types that have children should derive from this
     * class.
     *
     * @author ????-??-?? Eric Hyer
     *
     * @internal
     */
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
