#ifndef PointLeafItem_H
#define PointLeafItem_H


#include "AbstractPointItem.h"
#include "AbstractLeafItem.h"


class QVariant;

namespace Isis
{
  class ControlPoint;

  class PointLeafItem : public AbstractPointItem, public AbstractLeafItem
  {
    public:
      PointLeafItem(Isis::ControlPoint * cp,
          AbstractTreeItem * parent = 0);
      virtual ~PointLeafItem();


    private: // Disallow copying of this class
      PointLeafItem(const PointLeafItem & other);
      const PointLeafItem & operator=(const PointLeafItem & other);
  };
}

#endif
