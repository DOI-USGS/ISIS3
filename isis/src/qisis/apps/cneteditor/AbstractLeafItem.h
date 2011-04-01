#ifndef AbstractLeafItem_H
#define AbstractLeafItem_H


#include "AbstractTreeItem.h"


template< typename T > class QList;
class QVariant;


namespace Isis
{
  class AbstractLeafItem : public virtual AbstractTreeItem
  {
    public:
      AbstractLeafItem(AbstractTreeItem * parent = 0);
      virtual ~AbstractLeafItem();

      AbstractTreeItem * childAt(int row) const;
      int indexOf(AbstractTreeItem * child) const;
      int childCount() const;
      virtual void addChild(AbstractTreeItem * child);
      virtual void removeChild(int row);


    private: // disable copying of this class
      AbstractLeafItem(const AbstractLeafItem &);
      const AbstractLeafItem & operator=(const AbstractLeafItem &);
  };
}

#endif
