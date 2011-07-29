#ifndef RootItem_H
#define RootItem_H


#include "AbstractParentItem.h"


class QString;


namespace Isis
{
  class ControlPoint;

  class RootItem : public AbstractParentItem
  {
    public:
      RootItem();
      virtual ~RootItem();

      QString getData() const;
      QString getData(QString columnTitle) const;
      void setData(QString columnTitle, QString newData);
      void deleteSource();
      InternalPointerType getPointerType() const;
      void * getPointer() const;

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
