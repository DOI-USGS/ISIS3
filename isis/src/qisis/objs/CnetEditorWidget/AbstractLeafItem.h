#ifndef AbstractLeafItem_H
#define AbstractLeafItem_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractTreeItem.h"


template< typename T > class QList;
class QVariant;


namespace Isis {

  /**
   * @brief Base class for leaf items in the tree
   *
   * This class represents a leaf item in the tree.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   */
  class AbstractLeafItem : public virtual AbstractTreeItem {
    public:
      AbstractLeafItem(AbstractTreeItem *parent = 0);
      virtual ~AbstractLeafItem();

      virtual AbstractTreeItem *childAt(int row) const;
      virtual QList< AbstractTreeItem * > getChildren() const;
      virtual int indexOf(AbstractTreeItem *child) const;
      virtual int childCount() const;
      virtual void addChild(AbstractTreeItem *child);
      virtual AbstractTreeItem *getFirstVisibleChild() const;
      virtual AbstractTreeItem *getLastVisibleChild() const;
      virtual void setFirstVisibleChild(AbstractTreeItem *);
      virtual void setLastVisibleChild(AbstractTreeItem *);


    private: // disable copying of this class
      AbstractLeafItem(const AbstractLeafItem &);
      const AbstractLeafItem &operator=(const AbstractLeafItem &);
  };
}

#endif
