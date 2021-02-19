#ifndef MeasureLeafItem_H
#define MeasureLeafItem_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include "AbstractMeasureItem.h"
#include "AbstractLeafItem.h"


namespace Isis {
  class ControlMeasure;

  /**
   * A leaf node in the tree structure that represents a control measure.
   *
   * This class represents a leaf node in the tree model and has a control
   * measure as data. When created, it was used in the point model to show
   * which control measures are in a control point or on an image.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   */
  class MeasureLeafItem : public AbstractMeasureItem, public AbstractLeafItem {
    public:
      MeasureLeafItem(ControlMeasure *cm, int avgCharWidth,
          AbstractTreeItem *parent = 0);
      virtual ~MeasureLeafItem();


    private: // Disallow copying of this class
      MeasureLeafItem(const MeasureLeafItem &other);
      const MeasureLeafItem &operator=(const MeasureLeafItem &other);
  };
}

#endif
