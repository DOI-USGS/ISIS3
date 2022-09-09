/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractNullDataItem.h"

#include <QString>
#include <QVariant>

#include "IException.h"
#include "IString.h"


namespace Isis {
  AbstractNullDataItem::AbstractNullDataItem(AbstractTreeItem *parent)
    : AbstractTreeItem(parent) {
  }


  AbstractNullDataItem::~AbstractNullDataItem() {
  }


  QVariant AbstractNullDataItem::getData() const {
    return QVariant();
  }


  QVariant AbstractNullDataItem::getData(QString columnTitle) const {
    return QVariant();
  }


  void AbstractNullDataItem::setData(QString const &columnTitle, QString const &newData) {
    IString msg = "Cannot set data on an AbstractNullDataItem";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  bool AbstractNullDataItem::isDataEditable(QString columnTitle) const {
    return false;
  }


  void AbstractNullDataItem::deleteSource() {
    IString msg = "deleteSource called on an AbstractNullDataItem";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  AbstractTreeItem::InternalPointerType AbstractNullDataItem::getPointerType() const {
    return AbstractTreeItem::None;
  }


  void *AbstractNullDataItem::getPointer() const {
    return NULL;
  }


  bool AbstractNullDataItem::operator<(AbstractTreeItem const &other) const {
    IString msg = "operator<() called on an AbstractNullDataItem";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  void AbstractNullDataItem::sourceDeleted() {
  }
}
