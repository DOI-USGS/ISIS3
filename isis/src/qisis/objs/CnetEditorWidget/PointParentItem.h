#ifndef PointParentItem_H
#define PointParentItem_H


#include "AbstractParentItem.h"
#include "AbstractPointItem.h"


class QVariant;

namespace Isis {
  class ControlPoint;

  /**
   * @brief Tree item that is a parent and represents a control point
   *
   * This class represents a parent item in a tree structure that holds
   * control point data. It can have children items added to it, but they must
   * be of type MeasureLeafItem.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal 
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054. 
   */
  class PointParentItem : public AbstractPointItem, public AbstractParentItem {
    public:
      PointParentItem(ControlPoint *cp, int avgCharWidth,
          AbstractTreeItem *parent = 0);
      virtual ~PointParentItem();

      void addChild(AbstractTreeItem *child);


    private: // Disallow copying of this class
      PointParentItem(const PointParentItem &other);
      const PointParentItem &operator=(const PointParentItem &other);
  };
}

#endif
