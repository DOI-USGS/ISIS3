#include "IsisDebug.h"

#include "RootItem.h"

#include <QVariant>

#include "iException.h"
#include "iString.h"


namespace Isis
{
  RootItem::RootItem() : AbstractParentItem(NULL)
  {
  }


  RootItem::~RootItem()
  {
  }


  QVariant RootItem::data() const
  {
//     iString msg = "data called on a RootItem!";
//     throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    return QVariant();
  }


  void RootItem::deleteSource()
  {
    iString msg = "deleteSource called on a RootItem!";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }


  AbstractTreeItem::InternalPointerType RootItem::pointerType() const
  {
    return AbstractTreeItem::None;
  }
}
