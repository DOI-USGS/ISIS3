#ifndef RootItem_H
#define RootItem_H


#include "AbstractParentItem.h"


class QVariant;


namespace Isis
{
  class ControlPoint;

  class RootItem : public AbstractParentItem
  {
    public:
      RootItem(AbstractTreeItem * parent = 0);
      virtual ~RootItem();

      QVariant data() const;
      void deleteSource();
      InternalPointerType pointerType() const;


    private: // disable copying of this class
      RootItem(const RootItem & other);
      const RootItem & operator=(const RootItem & other);
  };
}

#endif
