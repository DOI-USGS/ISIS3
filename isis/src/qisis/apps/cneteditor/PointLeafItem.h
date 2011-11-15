#ifndef PointLeafItem_H
#define PointLeafItem_H


#include "AbstractPointItem.h"
#include "AbstractLeafItem.h"


namespace Isis
{
  class ControlPoint;

  namespace CnetViz
  {
    class PointLeafItem : public AbstractPointItem, public AbstractLeafItem
    {
      public:
        PointLeafItem(ControlPoint * cp, int avgCharWidth,
            AbstractTreeItem * parent = 0);
        virtual ~PointLeafItem();


      private: // Disallow copying of this class
        PointLeafItem(const PointLeafItem & other);
        const PointLeafItem & operator=(const PointLeafItem & other);
    };
  }
}

#endif
