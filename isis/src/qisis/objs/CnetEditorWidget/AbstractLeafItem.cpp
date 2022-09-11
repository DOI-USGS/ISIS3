/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractLeafItem.h"

#include "IException.h"
#include "IString.h"


namespace Isis {
  AbstractLeafItem::AbstractLeafItem(AbstractTreeItem *parent) :
    AbstractTreeItem(parent) {
  }


  AbstractLeafItem::~AbstractLeafItem() {
  }


  AbstractTreeItem *AbstractLeafItem::childAt(int row) const {
    IString msg = "childAt() called on an AbstractLeafItem!";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  QList< AbstractTreeItem * > AbstractLeafItem::getChildren() const {
    IString msg = "getChildren() called on an AbstractLeafItem!";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  int AbstractLeafItem::indexOf(AbstractTreeItem *child) const {
    IString msg = "indexOf() called on an AbstractLeafItem!";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  int AbstractLeafItem::childCount() const {
    return 0;
  }


  void AbstractLeafItem::addChild(AbstractTreeItem *child) {
    IString msg = "addChild() called on an AbstractLeafItem!";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  AbstractTreeItem *AbstractLeafItem::getFirstVisibleChild() const {
    return NULL;
  }


  AbstractTreeItem *AbstractLeafItem::getLastVisibleChild() const {
    return NULL;
  }


  void AbstractLeafItem::setFirstVisibleChild(AbstractTreeItem *) {
    IString msg = "setFirstVisibleChild() called on an AbstractLeafItem!";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  void AbstractLeafItem::setLastVisibleChild(AbstractTreeItem *) {
    IString msg = "setLastVisibleChild() called on an AbstractLeafItem!";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }
}
