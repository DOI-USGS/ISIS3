/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractParentItem.h"

#include <QList>

#include "RootItem.h"


namespace Isis {
  AbstractParentItem::AbstractParentItem(AbstractTreeItem *parent) :
    AbstractTreeItem(parent) {
    m_children = NULL;
    m_firstVisibleChild = NULL;
    m_lastVisibleChild = NULL;
    m_children = new QList< AbstractTreeItem * >;
  }


  AbstractParentItem::~AbstractParentItem() {
    if (m_children) {
      for (int i = 0; i < m_children->size(); i++) {
        delete(*m_children)[i];
        (*m_children)[i] = NULL;
      }
      delete m_children;
      m_children = NULL;
    }

    if (m_firstVisibleChild) {
      m_firstVisibleChild = NULL;
    }

    if (m_lastVisibleChild) {
      m_lastVisibleChild = NULL;
    }
  }


  AbstractTreeItem *AbstractParentItem::childAt(int row) const {
    return m_children->value(row);
  }


  QList< AbstractTreeItem * > AbstractParentItem::getChildren() const {
    return *m_children;
  }


  AbstractTreeItem *AbstractParentItem::getFirstVisibleChild() const {
    return m_firstVisibleChild;
  }


  AbstractTreeItem *AbstractParentItem::getLastVisibleChild() const {
    return m_lastVisibleChild;
  }


  int AbstractParentItem::indexOf(AbstractTreeItem *child) const {
    return m_children->indexOf(child);
  }


  int AbstractParentItem::childCount() const {
    return m_children->size();
  }


  void AbstractParentItem::addChild(AbstractTreeItem *child) {

    //     if (!m_firstVisibleChild && child->isVisible())
    //     {
    //       m_firstVisibleChild = child;
    //       m_lastVisibleChild = child;
    //     }
    //
    //     AbstractTreeItem * childWithNewNext = NULL;
    //     if (m_lastVisibleChild && m_firstVisibleChild != child)
    //     {
    //       childWithNewNext = m_lastVisibleChild;
    //     }

    m_children->append(child);
    child->setParent(this);

    //     if (childWithNewNext && child->isVisible())
    //     {
    //       childWithNewNext->setNextVisiblePeer(child);
    //       m_lastVisibleChild = child;
    //     }
  }


  void AbstractParentItem::setFirstVisibleChild(AbstractTreeItem *child) {
    m_firstVisibleChild = child;
  }


  void AbstractParentItem::setLastVisibleChild(AbstractTreeItem *child) {
    m_lastVisibleChild = child;
  }
}
