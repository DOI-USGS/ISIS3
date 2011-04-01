#include "IsisDebug.h"

#include "AbstractParentItem.h"

#include <QList>

#include "RootItem.h"


namespace Isis
{
  AbstractParentItem::AbstractParentItem(AbstractTreeItem * parent) :
    AbstractTreeItem(parent)
  {
    children = NULL;
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
  }


  AbstractTreeItem * AbstractParentItem::childAt(int row) const
  {
    ASSERT(children);
    ASSERT(row >= 0 && row < children->size());
    return children->value(row);
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
    children->append(child);
  }


  void AbstractParentItem::removeChild(int row)
  {
    ASSERT(row >= 0 && row < children->size());
    children->removeAt(row);
  }
}
