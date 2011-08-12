#ifndef BusyLeafItem_H
#define BusyLeafItem_H

#include "AbstractLeafItem.h"
#include "AbstractNullDataItem.h"


class QString;


namespace Isis
{
  class BusyLeafItem : public AbstractNullDataItem, public AbstractLeafItem
  {
      Q_OBJECT
    
    public:
      BusyLeafItem(AbstractTreeItem * parent = 0);
      virtual ~BusyLeafItem();
      virtual QString getData() const;
      virtual bool isSelectable() const;
      

    private: // Disallow copying of this class
      BusyLeafItem(const BusyLeafItem & other);
      const BusyLeafItem & operator=(const BusyLeafItem & other);
  };
}

#endif

