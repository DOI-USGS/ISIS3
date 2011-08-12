#ifndef RootItem_H
#define RootItem_H


#include "AbstractNullDataItem.h"
#include "AbstractParentItem.h"


class QString;


namespace Isis
{
  class ControlPoint;

  class RootItem : public AbstractNullDataItem, public AbstractParentItem
  {
      Q_OBJECT
    
    public:
      RootItem();
      virtual ~RootItem();
      void setLastVisibleFilteredItem(AbstractTreeItem * item);
      const AbstractTreeItem * getLastVisibleFilteredItem() const;


    private: // disable copying of this class
      RootItem(const RootItem & other);
      const RootItem & operator=(const RootItem & other);


    private:
      AbstractTreeItem * lastVisibleFilteredItem;
  };
}

#endif
