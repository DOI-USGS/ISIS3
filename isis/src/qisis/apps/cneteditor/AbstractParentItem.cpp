#include "IsisDebug.h"

#include "AbstractParentItem.h"

#include <QList>

#include "RootItem.h"


namespace Isis
{
  namespace CnetViz
  {
    AbstractParentItem::AbstractParentItem(AbstractTreeItem * parent) :
        AbstractTreeItem(parent)
    {
      children = NULL;
      firstVisibleChild = NULL;
      lastVisibleChild = NULL;
      children = new QList< AbstractTreeItem * >;
    }


    AbstractParentItem::~AbstractParentItem()
    {
      if (children)
      {
        for (int i = 0; i < children->size(); i++)
        {
          delete(*children)[i];
          (*children)[i] = NULL;
        }
        delete children;
        children = NULL;
      }

      if (firstVisibleChild)
      {
        firstVisibleChild = NULL;
      }

      if (lastVisibleChild)
      {
        lastVisibleChild = NULL;
      }
    }


    AbstractTreeItem * AbstractParentItem::childAt(int row) const
    {
      ASSERT(children);
      ASSERT(row >= 0 && row < children->size());
      return children->value(row);
    }


    QList< AbstractTreeItem * > AbstractParentItem::getChildren() const
    {
      ASSERT(children);
      return *children;
    }


    AbstractTreeItem * AbstractParentItem::getFirstVisibleChild() const
    {
      return firstVisibleChild;
    }


    AbstractTreeItem * AbstractParentItem::getLastVisibleChild() const
    {
      return lastVisibleChild;
    }


    int AbstractParentItem::indexOf(AbstractTreeItem * child) const
    {
      ASSERT(child);
      ASSERT(children);
      return children->indexOf(child);
    }


    int AbstractParentItem::childCount() const
    {
      ASSERT(children);
      return children->size();
    }


    void AbstractParentItem::addChild(AbstractTreeItem * child)
    {
      ASSERT(child);
      ASSERT(children);
      ASSERT(!dynamic_cast< RootItem * >(child));

  //     if (!firstVisibleChild && child->isVisible())
  //     {
  //       firstVisibleChild = child;
  //       lastVisibleChild = child;
  //     }
  // 
  //     AbstractTreeItem * childWithNewNext = NULL;
  //     if (lastVisibleChild && firstVisibleChild != child)
  //     {
  //       childWithNewNext = lastVisibleChild;
  //     }

      children->append(child);
      child->setParent(this);

  //     if (childWithNewNext && child->isVisible())
  //     {
  //       childWithNewNext->setNextVisiblePeer(child);
  //       lastVisibleChild = child;
  //     }
    }


    void AbstractParentItem::setFirstVisibleChild(AbstractTreeItem * child)
    {
      firstVisibleChild = child;
    }


    void AbstractParentItem::setLastVisibleChild(AbstractTreeItem * child)
    {
      lastVisibleChild = child;
    }
  }
}
