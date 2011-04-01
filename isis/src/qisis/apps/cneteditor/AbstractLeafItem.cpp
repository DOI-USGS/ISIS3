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
    iString msg = "childAt called on an AbstractLeafItem!";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    return NULL;
  }


  int AbstractLeafItem::indexOf(AbstractTreeItem * child) const
  {
    iString msg = "indexOf called on an AbstractLeafItem!";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    return -1;
  }


  int AbstractLeafItem::childCount() const
  {
    return 0;
  }


  void AbstractLeafItem::addChild(AbstractTreeItem * child)
  {
    iString msg = "addChild called on an AbstractLeafItem!";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }


  void AbstractLeafItem::removeChild(int row)
  {
    iString msg = "removeChild called on an AbstractLeafItem!";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }
}
