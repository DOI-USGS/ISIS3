/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "TableColumn.h"

#include <algorithm>
#include <iostream>

#include <QString>


using std::swap;


namespace Isis {
  TableColumn::TableColumn(QString text, bool m_readOnlyStatus,
      bool affectsNetStructure) {
    nullify();

    m_title = new QString(text);
    m_visible = true;
    m_readOnly = m_readOnlyStatus;
    m_affectsNetworkStructure = affectsNetStructure;
    m_ascendingSortOrder = true;
  }


  TableColumn::TableColumn(const TableColumn &other) {
    nullify();

    m_title = new QString(*other.m_title);
    m_visible = other.m_visible;
    m_readOnly = other.m_readOnly;
    m_width = other.m_width;
  }


  TableColumn::~TableColumn() {
    delete m_title;
    m_title = NULL;
  }


  QString TableColumn::getTitle() const {
    return *m_title;
  }


  void TableColumn::setTitle(QString text) {
    *m_title = text;
  }


  TableColumn &TableColumn::operator=(TableColumn other) {
    swap(*m_title, *other.m_title);
    swap(m_visible, other.m_visible);
    swap(m_readOnly, other.m_readOnly);
    swap(m_width, other.m_width);

    return *this;
  }


  bool TableColumn::isVisible() const {
    return m_visible;
  }


  void TableColumn::setVisible(bool visibility) {
    m_visible = visibility;
    emit visibilityChanged();
  }


  int TableColumn::getWidth() const {
    return m_width;
  }


  void TableColumn::setWidth(int newWidth) {
    m_width = newWidth;
    emit widthChanged();
  }


  bool TableColumn::isReadOnly() const {
    return m_readOnly;
  }


  bool TableColumn::hasNetworkStructureEffect() const {
    return m_affectsNetworkStructure;
  }


  bool TableColumn::sortAscending() const {
    return m_ascendingSortOrder;
  }


  void TableColumn::setSortAscending(bool ascending) {
    m_ascendingSortOrder = ascending;
    emit sortOutDated();
  }


  void TableColumn::nullify() {
    m_title = NULL;
  }
}
