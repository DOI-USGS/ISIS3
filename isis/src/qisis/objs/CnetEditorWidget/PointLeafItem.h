#ifndef PointLeafItem_H
#define PointLeafItem_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractPointItem.h"
#include "AbstractLeafItem.h"


namespace Isis {
  class ControlPoint;

  /**
   * @brief A leaf node in the tree structure that represents a control point
   *
   * This class represents a leaf node in the tree model and has a control
   * point as data. When created, it was used in the serial model to show
   * which control points connect which images.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   */
  class PointLeafItem : public AbstractPointItem, public AbstractLeafItem {
    public:
      PointLeafItem(ControlPoint *cp, int avgCharWidth,
          AbstractTreeItem *parent = 0);
      virtual ~PointLeafItem();


    private: // Disallow copying of this class
      PointLeafItem(const PointLeafItem &other);
      const PointLeafItem &operator=(const PointLeafItem &other);
  };
}

#endif
