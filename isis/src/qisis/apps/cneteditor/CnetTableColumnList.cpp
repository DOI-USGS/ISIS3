#include "IsisDebug.h"

#include "CnetTableColumnList.h"

#include <iostream>

#include <QList>
#include <QPair>

#include "CnetTableColumn.h"


using std::cerr;
using std::swap;


namespace Isis
{
  CnetTableColumnList::CnetTableColumnList()
  {
    nullify();
    
    cols = new QList< CnetTableColumn * >;
    sortingOrder = new QList< CnetTableColumn const * >;
  }
  
  
  CnetTableColumnList::CnetTableColumnList(CnetTableColumnList const & other)
  {
    nullify();
    
    cols = new QList< CnetTableColumn * >(*other.cols);
    sortingOrder = new QList< CnetTableColumn const * >(*other.sortingOrder);
  }


  CnetTableColumnList::~CnetTableColumnList()
  {
    delete cols;
    cols = NULL;
    
    delete sortingOrder;
    sortingOrder = NULL;
  }
  
  
  CnetTableColumn *& CnetTableColumnList::operator[](int index)
  {
    checkIndexRange(index);
    
    return (*cols)[index];
  }

  
  void CnetTableColumnList::append(CnetTableColumn * newCol)
  {
    cols->append(newCol);
    sortingOrder->append(newCol);
  }
  
  
  void CnetTableColumnList::prepend(CnetTableColumn * newCol)
  {
    cols->prepend(newCol);
    sortingOrder->append(newCol);
  }
  
  
  int CnetTableColumnList::indexOf(CnetTableColumn const * someCol)
  {
    int index = -1;
    for (int i = 0; index < 0 && i < cols->size(); i++)
      if ((*cols)[i] == someCol)
        index = i;
      
    return index;
  }
  
  
  bool CnetTableColumnList::contains(CnetTableColumn const * someCol)
  {
    return indexOf(someCol) != -1;
  }
  
  
  void CnetTableColumnList::lower(CnetTableColumn const * col)
  {
    int oldIndex = sortingOrder->indexOf(col);
    checkIndexRange(oldIndex);
    
    // if not already lowest priority
    if (oldIndex < sortingOrder->size() - 1)
    {
      sortingOrder->removeAt(oldIndex);
      sortingOrder->insert(oldIndex + 1, col);
    }
  }
  
  
  void CnetTableColumnList::lower(int visibleColumnIndex)
  {
    lower(indexOf(getVisibleColumns()[visibleColumnIndex]));
  }
  
  
  void CnetTableColumnList::raise(CnetTableColumn const * col)
  {
    int oldIndex = sortingOrder->indexOf(col);
    checkIndexRange(oldIndex);
    
    // if not already highest priority
    if (oldIndex > 0)
    {
      sortingOrder->removeAt(oldIndex);
      sortingOrder->insert(oldIndex - 1, col);
    }
  }
  
  
  void CnetTableColumnList::raise(int visibleColumnIndex)
  {
    raise(indexOf(getVisibleColumns()[visibleColumnIndex]));
  }

  
  int CnetTableColumnList::size() const
  {
    ASSERT(cols);
    return cols->size();
  }
  
  
  CnetTableColumnList & CnetTableColumnList::operator=(
      CnetTableColumnList other)
  {
    swap(*cols, *other.cols);
    swap(*sortingOrder, *other.sortingOrder);
    
    return *this;
  }


  /**
   * @returns minX, maxX in a pair
   */
  QPair<int, int> CnetTableColumnList::getVisibleXRange(int visibleColumn)
  {
    int minX = 0;
    int maxX = 0;

    CnetTableColumnList visibleCols = getVisibleColumns();

    if (visibleColumn < visibleCols.size() && visibleColumn >= 0)
    {
      int indent = 0;

      for (int i = 0; i < visibleColumn; i++)
        indent += visibleCols[i]->getWidth() - 1;

      minX = indent;
      maxX = minX + visibleCols[visibleColumn]->getWidth() - 1;
    }

    return QPair<int, int>(minX, maxX);
  }


  CnetTableColumnList CnetTableColumnList::getVisibleColumns()
  {
    CnetTableColumnList visibleColumns;

//     cerr << "CnetTableColumnList::getVisibleColumns() this: " << this << "\n";
//     cerr << "CnetTableColumnList::getVisibleColumns() cols size: " << cols->size() << "\n";

    for (int i = 0; i < size(); i++)
      if (cols->at(i)->isVisible())
        visibleColumns.append(cols->at(i));
    
//     cerr << "CnetTableColumnList::getVisibleColumns() visibleColumns: " << visibleColumns.size() << "\n";

    // fix sorting order
    *visibleColumns.sortingOrder = *sortingOrder;
    for (int i = sortingOrder->size() - 1; i >= 0; i--)
      if (!visibleColumns.contains((*visibleColumns.sortingOrder)[i]))
        visibleColumns.sortingOrder->removeAt(i);
      
//     cerr << "CnetTableColumnList::getVisibleColumns() visibleColumns: " << visibleColumns.size() << "\n";

    return visibleColumns;
  }


  int CnetTableColumnList::getVisibleWidth() const
  {
    int width = 0;

    for (int i = 0; i < size(); i++)
      if (cols->at(i)->isVisible())
        width += cols->at(i)->getWidth() - 1;
    // For the border...
    width -= 2;

    return width;
  }
  
  
  QList< CnetTableColumn const * > CnetTableColumnList::getSortingOrder() const
  {
    ASSERT(sortingOrder);
    return *sortingOrder;
  }
  
  
  void CnetTableColumnList::checkIndexRange(int index)
  {
    ASSERT(cols);
    
    if (index < 0 || index >= cols->size())
    {
      iString msg = "index [";
      msg += index;
      msg += "] is out of range.  Size of list is: ";
      msg += cols->size();
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }
  
  
  void CnetTableColumnList::nullify()
  {
    cols = NULL;
    sortingOrder = NULL;
  }
}
