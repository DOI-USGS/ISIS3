#ifndef BusyLeafItem_H
#define BusyLeafItem_H

#include "AbstractLeafItem.h"


class QString;


namespace Isis
{
  class BusyLeafItem : public AbstractLeafItem
  {
    public:
      BusyLeafItem(AbstractTreeItem * parent = 0);
      virtual ~BusyLeafItem();

      QString getData() const;
      void deleteSource();
      InternalPointerType getPointerType() const;
      void * getPointer() const;
      bool isSelectable() const;


    private: // Disallow copying of this class
      BusyLeafItem(const BusyLeafItem & other);
      const BusyLeafItem & operator=(const BusyLeafItem & other);
  };
}

#endif

