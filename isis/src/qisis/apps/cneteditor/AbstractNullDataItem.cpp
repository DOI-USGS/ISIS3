#include "IsisDebug.h"

#include "AbstractNullDataItem.h"

#include <QString>
#include <QVariant>

#include "IException.h"
#include "IString.h"


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
      IString msg = "Cannot set data on an AbstractNullDataItem";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }


    bool AbstractNullDataItem::isDataEditable(QString columnTitle) const {
      return false;
    }


    void AbstractNullDataItem::deleteSource()
    {
      IString msg = "deleteSource called on an AbstractNullDataItem";
      throw IException(IException::Programmer, msg, _FILEINFO_);
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
      IString msg = "operator<() called on an AbstractNullDataItem";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }


    void AbstractNullDataItem::sourceDeleted() {
    }
  }
}
