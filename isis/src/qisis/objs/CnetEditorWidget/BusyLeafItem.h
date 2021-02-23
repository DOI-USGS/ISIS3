#ifndef BusyLeafItem_H
#define BusyLeafItem_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractLeafItem.h"
#include "AbstractNullDataItem.h"


class QString;
class QVariant;


namespace Isis {

  /**
   * @brief A leaf item that is not ready for user interaction
   *
   * This class represents a leaf item in the tree that is still being
   * calculated (i.e. during filtering).
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   */
  class BusyLeafItem : public AbstractNullDataItem, public AbstractLeafItem {
      Q_OBJECT

    public:
      BusyLeafItem(AbstractTreeItem *parent = 0);
      virtual ~BusyLeafItem();
      virtual QVariant getData() const;
      virtual bool isSelectable() const;


    private: // Disallow copying of this class
      BusyLeafItem(const BusyLeafItem &other);
      const BusyLeafItem &operator=(const BusyLeafItem &other);
  };
}

#endif
