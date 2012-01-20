#include "IsisDebug.h"

#include "AbstractNullDataItem.h"

#include <QString>
#include <QVariant>

#include "iException.h"
#include "iString.h"


namespace Isis
{
  namespace CnetViz
  {
    AbstractNullDataItem::AbstractNullDataItem(AbstractTreeItem * parent)
        : AbstractTreeItem(parent)
    {
    }


    AbstractNullDataItem::~AbstractNullDataItem()
    {
    }


    QVariant AbstractNullDataItem::getData() const
    {
      return QVariant();
    }


    QVariant AbstractNullDataItem::getData(QString columnTitle) const
    {
      return QVariant();
    }


    void AbstractNullDataItem::setData(QString const & columnTitle, QString const & newData)
    {
      iString msg = "Cannot set data on an AbstractNullDataItem";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }


    bool AbstractNullDataItem::isDataLocked(QString columnTitle) const {
      return false;
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
}
