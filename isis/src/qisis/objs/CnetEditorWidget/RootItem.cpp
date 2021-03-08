/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "IsisDebug.h"

#include "RootItem.h"


namespace Isis {
  RootItem::RootItem() : AbstractTreeItem(NULL), AbstractNullDataItem() {
    m_lastVisibleFilteredItem = NULL;
    setExpanded(true);
  }


  RootItem::~RootItem() {
    m_lastVisibleFilteredItem = NULL;
  }


  void RootItem::setLastVisibleFilteredItem(AbstractTreeItem *item) {
    m_lastVisibleFilteredItem = item;
  }


  const AbstractTreeItem *RootItem::getLastVisibleFilteredItem() const {
    return m_lastVisibleFilteredItem;
  }
}
