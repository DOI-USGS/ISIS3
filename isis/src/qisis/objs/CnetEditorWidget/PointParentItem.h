#ifndef PointParentItem_H
#define PointParentItem_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


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
