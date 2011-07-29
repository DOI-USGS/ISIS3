#include "IsisDebug.h"

#include "RootItem.h"

#include <QString>

#include "iException.h"
#include "iString.h"


namespace Isis
{
  RootItem::RootItem() : AbstractParentItem(NULL)
  {
    lastVisibleFilteredItem = NULL;
    setExpanded(true);
  }


  RootItem::~RootItem()
  {
    lastVisibleFilteredItem = NULL;
  }


  QString RootItem::getData() const
  {
//     iString msg = "data called on a RootItem!";
//     throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    return QString();
  }


  QString RootItem::getData(QString columnTitle) const
  {
    return QString();
  }


  void RootItem::setData(QString columnTitle, QString newData)
  {
    iString msg = "Cannot set data on RootItem";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }


  void RootItem::deleteSource()
  {
    iString msg = "deleteSource called on a RootItem!";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }


  AbstractTreeItem::InternalPointerType RootItem::getPointerType() const
  {
    return AbstractTreeItem::None;
  }


  void * RootItem::getPointer() const
  {
    return NULL;
  }


  void RootItem::setLastVisibleFilteredItem(AbstractTreeItem * item)
  {
    lastVisibleFilteredItem = item;
  }


  const AbstractTreeItem * RootItem::getLastVisibleFilteredItem() const
  {
    return lastVisibleFilteredItem;
  }
}
