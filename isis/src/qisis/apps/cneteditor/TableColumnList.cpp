#include "IsisDebug.h"

#include "TableColumnList.h"

#include <iostream>

#include <QList>
#include <QPair>

#include "TableColumn.h"


using std::cerr;
using std::swap;


namespace Isis
{
  namespace CnetViz
  {
    TableColumnList::TableColumnList()
    {
      nullify();
      
      cols = new QList< TableColumn * >;
      sortingOrder = new QList< TableColumn * >;
    }
    
    
    TableColumnList::TableColumnList(TableColumnList const & other)
    {
      nullify();
      
      cols = new QList< TableColumn * >(*other.cols);
      sortingOrder = new QList< TableColumn * >(*other.sortingOrder);
    }


    TableColumnList::~TableColumnList()
    {
      delete cols;
      cols = NULL;
      
      delete sortingOrder;
      sortingOrder = NULL;
    }
    
    
    TableColumn * & TableColumnList::operator[](int index)
    {
      checkIndexRange(index);
      
      return (*cols)[index];
    }


    TableColumn * & TableColumnList::operator[](QString title)
    {
      for (int i = 0; i < cols->size(); i++)
        if (cols->at(i)->getTitle() == title)
          return (*cols)[i];
        
      iString msg = "There is no column with a title of [";
      msg += iString(title);
      msg += "] inside this column list";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    
    void TableColumnList::append(TableColumn * newCol)
    {
      if (!newCol)
      {
        iString msg = "Attempted to add NULL column to the columnlist";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }
      
      cols->append(newCol);
      sortingOrder->append(newCol);
      connect(newCol, SIGNAL(sortOutDated()), this, SIGNAL(sortOutDated()));
    }
    
    
    void TableColumnList::prepend(TableColumn * newCol)
    {
      cols->prepend(newCol);
      sortingOrder->append(newCol);
    }
    
    
    int TableColumnList::indexOf(TableColumn const * someCol) const
    {
      int index = -1;
      for (int i = 0; index < 0 && i < cols->size(); i++)
        if ((*cols)[i] == someCol)
          index = i;
        
      return index;
    }
    
    
    bool TableColumnList::contains(TableColumn const * someCol) const
    {
      return indexOf(someCol) != -1;
    }
    

    bool TableColumnList::contains(QString columnTitle) const
    {
      bool foundTitle = false;
      for (int i = 0; i < cols->size() && !foundTitle; i++)
        foundTitle = (cols->at(i)->getTitle() == columnTitle);
      return foundTitle;
    }
    
    
    void TableColumnList::lower(TableColumn * col, bool emitSortOutDated)
    {
      int oldIndex = sortingOrder->indexOf(col);
      checkIndexRange(oldIndex);
      
      // if not already lowest priority
      if (oldIndex < sortingOrder->size() - 1)
      {
        sortingOrder->removeAt(oldIndex);
        sortingOrder->insert(oldIndex + 1, col);
      }
      
      if (emitSortOutDated)
        emit sortOutDated();
    }
    
    
    void TableColumnList::lower(int visibleColumnIndex, bool emitSortOutDated)
    {
      lower(indexOf(getVisibleColumns()[visibleColumnIndex]), emitSortOutDated);
    }
    
    
    void TableColumnList::raise(TableColumn * col, bool emitSortOutDated)
    {
      int oldIndex = sortingOrder->indexOf(col);
      checkIndexRange(oldIndex);
      
      // if not already highest priority
      if (oldIndex > 0)
      {
        sortingOrder->removeAt(oldIndex);
        sortingOrder->insert(oldIndex - 1, col);
      }
      
      if (emitSortOutDated)
        emit sortOutDated();
    }
    
    
    void TableColumnList::raise(int visibleColumnIndex, bool emitSortOutDated)
    {
      raise(indexOf(getVisibleColumns()[visibleColumnIndex]), emitSortOutDated);
    }
    
    
    void TableColumnList::raiseToTop(TableColumn * col)
    {
      while (sortingOrder->at(0) != col)
        raise(col, false);
      
      emit sortOutDated();
    }
    
    
    void TableColumnList::raiseToTop(int visibleColumnIndex)
    {
      raiseToTop(indexOf(getVisibleColumns()[visibleColumnIndex]));
    }


    int TableColumnList::size() const
    {
      ASSERT(cols);
      return cols->size();
    }
    
    
    TableColumnList & TableColumnList::operator=(
        TableColumnList other)
    {
      swap(*cols, *other.cols);
      swap(*sortingOrder, *other.sortingOrder);
      
      return *this;
    }


    /**
    * @returns minX, maxX in a pair
    */
    QPair<int, int> TableColumnList::getVisibleXRange(int visibleColumn)
    {
      int minX = 0;
      int maxX = 0;

      TableColumnList visibleCols = getVisibleColumns();

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


    TableColumnList TableColumnList::getVisibleColumns()
    {
      TableColumnList visibleColumns;

  //     cerr << "TableColumnList::getVisibleColumns() this: " << this << "\n";
  //     cerr << "TableColumnList::getVisibleColumns() cols size: " << cols->size() << "\n";

      for (int i = 0; i < size(); i++)
        if (cols->at(i)->isVisible())
          visibleColumns.append(cols->at(i));
      
  //     cerr << "TableColumnList::getVisibleColumns() visibleColumns: " << visibleColumns.size() << "\n";

      // fix sorting order
      *visibleColumns.sortingOrder = *sortingOrder;
      for (int i = sortingOrder->size() - 1; i >= 0; i--)
        if (!visibleColumns.contains((*visibleColumns.sortingOrder)[i]))
          visibleColumns.sortingOrder->removeAt(i);
        
  //     cerr << "TableColumnList::getVisibleColumns() visibleColumns: " << visibleColumns.size() << "\n";

      return visibleColumns;
    }


    int TableColumnList::getVisibleWidth() const
    {
      int width = 0;

      for (int i = 0; i < size(); i++)
        if (cols->at(i)->isVisible())
          width += cols->at(i)->getWidth() - 1;
      // For the border...
      width -= 2;

      return width;
    }
    
    
    QList< TableColumn * > TableColumnList::getSortingOrder()
    {
      ASSERT(sortingOrder);
      
      QList< TableColumn * > validSortingOrder;
      for (int i = 0; i < sortingOrder->size(); i++)
        if (sortingOrder->at(i)->getTitle().size())
          validSortingOrder.append(sortingOrder->at(i));
        
      return validSortingOrder;
    }
    
    
    QStringList TableColumnList::getSortingOrderAsStrings() const
    {
      QStringList sortingOrderStrings;
      for (int i = 0; i < sortingOrder->size(); i++)
        if (sortingOrder->at(i)->getTitle().size())
          sortingOrderStrings.append(sortingOrder->at(i)->getTitle());
      
      return sortingOrderStrings;
    }


    void TableColumnList::setSortingOrder(QStringList newOrder)
    {
      for (int i = newOrder.size() - 1; i >= 0; i--)
        if (contains(newOrder[i]))
          raiseToTop(operator[](newOrder[i]));
    }

    
    void TableColumnList::checkIndexRange(int index)
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
    
    
    void TableColumnList::nullify()
    {
      cols = NULL;
      sortingOrder = NULL;
    }
  }
}
