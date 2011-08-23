#include "CnetTableColumn.h"

#include <algorithm>
#include <iostream>

#include <QString>


using std::cerr;
using std::swap;


namespace Isis
{
  CnetTableColumn::CnetTableColumn(QString text, bool readOnlyStatus,
      bool affectsNetStructure)
  {
    nullify();

    title = new QString(text);
    visible = true;
    readOnly = readOnlyStatus;
    affectsNetworkStructure = affectsNetStructure;
    ascendingSortOrder = true;
  }


  CnetTableColumn::CnetTableColumn(const CnetTableColumn & other)
  {
    nullify();

    title = new QString(*other.title);
    visible = other.visible;
    readOnly = other.readOnly;
    width = other.width;
  }


  CnetTableColumn::~CnetTableColumn()
  {
    delete title;
    title = NULL;
  }


  QString CnetTableColumn::getTitle() const
  {
    return *title;
  }


  void CnetTableColumn::setTitle(QString text)
  {
    *title = text;
  }


  CnetTableColumn & CnetTableColumn::operator=(CnetTableColumn other)
  {
    swap(*title, *other.title);
    swap(visible, other.visible);
    swap(readOnly, other.readOnly);
    swap(width, other.width);

    return *this;
  }


  bool CnetTableColumn::isVisible() const
  {
    return visible;
  }


  void CnetTableColumn::setVisible(bool visibility)
  {
    visible = visibility;
    emit visibilityChanged();
  }


  int CnetTableColumn::getWidth() const
  {
    return width;
  }


  void CnetTableColumn::setWidth(int newWidth)
  {
    width = newWidth;
    emit widthChanged();
  }


  bool CnetTableColumn::isReadOnly() const
  {
    return readOnly;
  }
  
  
  bool CnetTableColumn::hasNetworkStructureEffect() const
  {
    return affectsNetworkStructure;
  }
  
  
  bool CnetTableColumn::sortAscending() const
  {
    return ascendingSortOrder;
  }
  
  
  void CnetTableColumn::setSortAscending(bool ascending)
  {
    ascendingSortOrder = ascending;
    emit sortOutDated();
  }


  void CnetTableColumn::nullify()
  {
    title = NULL;
  }
}

