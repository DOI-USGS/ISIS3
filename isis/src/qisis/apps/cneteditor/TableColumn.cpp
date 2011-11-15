#include "TableColumn.h"

#include <algorithm>
#include <iostream>

#include <QString>


using std::cerr;
using std::swap;


namespace Isis
{
  namespace CnetViz
  {
    TableColumn::TableColumn(QString text, bool readOnlyStatus,
        bool affectsNetStructure)
    {
      nullify();

      title = new QString(text);
      visible = true;
      readOnly = readOnlyStatus;
      affectsNetworkStructure = affectsNetStructure;
      ascendingSortOrder = true;
    }


    TableColumn::TableColumn(const TableColumn & other)
    {
      nullify();

      title = new QString(*other.title);
      visible = other.visible;
      readOnly = other.readOnly;
      width = other.width;
    }


    TableColumn::~TableColumn()
    {
      delete title;
      title = NULL;
    }


    QString TableColumn::getTitle() const
    {
      return *title;
    }


    void TableColumn::setTitle(QString text)
    {
      *title = text;
    }


    TableColumn & TableColumn::operator=(TableColumn other)
    {
      swap(*title, *other.title);
      swap(visible, other.visible);
      swap(readOnly, other.readOnly);
      swap(width, other.width);

      return *this;
    }


    bool TableColumn::isVisible() const
    {
      return visible;
    }


    void TableColumn::setVisible(bool visibility)
    {
      visible = visibility;
      emit visibilityChanged();
    }


    int TableColumn::getWidth() const
    {
      return width;
    }


    void TableColumn::setWidth(int newWidth)
    {
      width = newWidth;
      emit widthChanged();
    }


    bool TableColumn::isReadOnly() const
    {
      return readOnly;
    }
    
    
    bool TableColumn::hasNetworkStructureEffect() const
    {
      return affectsNetworkStructure;
    }
    
    
    bool TableColumn::sortAscending() const
    {
      return ascendingSortOrder;
    }
    
    
    void TableColumn::setSortAscending(bool ascending)
    {
      ascendingSortOrder = ascending;
      emit sortOutDated();
    }


    void TableColumn::nullify()
    {
      title = NULL;
    }
  }
}
