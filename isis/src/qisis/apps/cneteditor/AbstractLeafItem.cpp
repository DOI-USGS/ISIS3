#include "IsisDebug.h"

#include "AbstractLeafItem.h"

#include "iException.h"
#include "iString.h"


namespace Isis
{
  AbstractLeafItem::AbstractLeafItem(AbstractTreeItem * parent) :
      AbstractTreeItem(parent)
  {
  }


  AbstractLeafItem::~AbstractLeafItem()
  {
  }


  AbstractTreeItem * AbstractLeafItem::childAt(int row) const
  {
    iString msg = "childAt() called on an AbstractLeafItem!";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    return NULL;
  }


  QList< AbstractTreeItem * > AbstractLeafItem::getChildren() const
  {
    iString msg = "getChildren() called on an AbstractLeafItem!";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    return QList< AbstractTreeItem * >();
  }


  int AbstractLeafItem::indexOf(AbstractTreeItem * child) const
  {
    iString msg = "indexOf() called on an AbstractLeafItem!";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    return -1;
  }


  int AbstractLeafItem::childCount() const
  {
    return 0;
  }


  void AbstractLeafItem::addChild(AbstractTreeItem * child)
  {
    iString msg = "addChild() called on an AbstractLeafItem!";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }


  AbstractTreeItem * AbstractLeafItem::getFirstVisibleChild() const
  {
    return NULL;
  }


  AbstractTreeItem * AbstractLeafItem::getLastVisibleChild() const
  {
    return NULL;
  }


  void AbstractLeafItem::setFirstVisibleChild(AbstractTreeItem *)
  {
    iString msg = "setFirstVisibleChild() called on an AbstractLeafItem!";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }


  void AbstractLeafItem::setLastVisibleChild(AbstractTreeItem *)
  {
    iString msg = "setLastVisibleChild() called on an AbstractLeafItem!";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }
}

