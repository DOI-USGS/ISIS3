#ifndef AbstractParentItem_H
#define AbstractParentItem_H

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
   * @brief Base class for an item that is a parent in the tree
   *
   * This class represents an item in the tree that is a parent (i.e. has
   * children items). Item types that have children should derive from this
   * class.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   */
  class AbstractParentItem : public virtual AbstractTreeItem {
    public:
      AbstractParentItem(AbstractTreeItem *parent = 0);
      virtual ~AbstractParentItem();

      virtual AbstractTreeItem *childAt(int row) const;
      virtual QList< AbstractTreeItem * > getChildren() const;
      virtual AbstractTreeItem *getFirstVisibleChild() const;
      virtual AbstractTreeItem *getLastVisibleChild() const;
      virtual int indexOf(AbstractTreeItem *child) const;
      virtual int childCount() const;
      virtual void addChild(AbstractTreeItem *child);
      virtual void setFirstVisibleChild(AbstractTreeItem *child);
      virtual void setLastVisibleChild(AbstractTreeItem *child);


    private: // disable copying of this class
      AbstractParentItem(const AbstractParentItem &);
      const AbstractParentItem &operator=(const AbstractParentItem &);


    private:
      QList< AbstractTreeItem * > * m_children;
      AbstractTreeItem *m_firstVisibleChild;
      AbstractTreeItem *m_lastVisibleChild;
  };
}

#endif
