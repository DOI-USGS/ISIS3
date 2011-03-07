#include "TreeItem.h"

#include <QList>
#include <QStringList>
#include <QVariant>

#include "iException.h"
#include "iString.h"


namespace Isis
{
  TreeItem::TreeItem(TreeItem * parent) : parentItem(parent)
  {
    children = NULL;
    children = new QList< TreeItem * >;
    numColumns = 1;
  }


  TreeItem::~TreeItem()
  {
    if (children)
    {
      for (int i = 0; i < children->size(); i++)
      {
        delete(*children)[i];
        (*children)[i] = NULL;
      }
      delete children;
      children = NULL;
    }
  }


  TreeItem * TreeItem::parent()
  {
    return parentItem;
  }


  TreeItem * TreeItem::childAt(int row)
  {
    return children->value(row);
  }


  int TreeItem::childCount() const
  {
    return children->count();
  }


  int TreeItem::row() const
  {
    int rowIndex = 0;
    if (parentItem)
      rowIndex = parentItem->children->indexOf(const_cast< TreeItem * >(this));

    return rowIndex;
  }


  int TreeItem::columnCount() const
  {
    return numColumns;
  }


  void TreeItem::validateColumn(int column) const
  {
    if (column < 0 || column >= numColumns)
    {
      iString msg = "column [";
      msg += column + "] out of bounds.  Valid columns are 0-";
      msg += (numColumns - 1) + ".";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }

}
