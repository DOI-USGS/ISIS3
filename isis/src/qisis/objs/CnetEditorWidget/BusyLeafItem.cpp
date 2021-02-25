/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "BusyLeafItem.h"

#include <QVariant>

#include "IException.h"
#include "IString.h"

#include "AbstractTreeItem.h"


namespace Isis {
  BusyLeafItem::BusyLeafItem(AbstractTreeItem *parent)
    : AbstractTreeItem(parent), AbstractNullDataItem() {
    calcDataWidth(1);
  }


  BusyLeafItem::~BusyLeafItem() {
  }


  QVariant BusyLeafItem::getData() const {
    return QVariant("Working...");
  }


  bool BusyLeafItem::isSelectable() const {
    return false;
  }
}
