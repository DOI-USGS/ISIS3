#include "IsisDebug.h"

#include "AbstractNullDataItem.h"

#include <QString>

#include "iException.h"
#include "iString.h"


namespace Isis
{
  AbstractNullDataItem::AbstractNullDataItem(AbstractTreeItem * parent)
      : AbstractTreeItem(parent)
  {
  }


  AbstractNullDataItem::~AbstractNullDataItem()
  {
  }


  QString AbstractNullDataItem::getData() const
  {
    return QString();
  }


  QString AbstractNullDataItem::getData(QString columnTitle) const
  {
    return QString();
  }


  void AbstractNullDataItem::setData(QString const & columnTitle, QString const & newData)
  {
    iString msg = "Cannot set data on an AbstractNullDataItem";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }


  void AbstractNullDataItem::deleteSource()
  {
    iString msg = "deleteSource called on an AbstractNullDataItem";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }


  AbstractTreeItem::InternalPointerType AbstractNullDataItem::getPointerType() const
  {
    return AbstractTreeItem::None;
  }


  void * AbstractNullDataItem::getPointer() const
  {
    return NULL;
  }
  
  
  bool AbstractNullDataItem::operator<(AbstractTreeItem const & other) const
  {
    iString msg = "operator<() called on an AbstractNullDataItem";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }
}
