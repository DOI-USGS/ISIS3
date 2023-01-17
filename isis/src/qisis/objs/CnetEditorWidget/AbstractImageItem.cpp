/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractImageItem.h"

#include <iostream>

#include <QPair>
#include <QString>
#include <QVariant>

#include "ControlNet.h"


namespace Isis {
  AbstractImageItem::AbstractImageItem(QString imageSerial, ControlNet *net,
                                       int avgCharWidth, AbstractTreeItem *parent)
        : AbstractTreeItem(parent) {
    m_imageAndNet = new QPair<QString, ControlNet *>(imageSerial, net);
    calcDataWidth(avgCharWidth);
  }


  AbstractImageItem::~AbstractImageItem() {
    if (m_imageAndNet) {
      delete m_imageAndNet;
    }
    m_imageAndNet = NULL;
  }


  QVariant AbstractImageItem::getData() const {
    if (m_imageAndNet) {
      return QVariant(m_imageAndNet->first);
    }
    return QVariant();
  }


  QVariant AbstractImageItem::getData(QString columnTitle) const {
    return QVariant();
  }


  void AbstractImageItem::setData(QString const &columnTitle,
      QString const &newData) {
  }


  bool AbstractImageItem::isDataEditable(QString columnTitle) const {
    return false;
  }


  void AbstractImageItem::deleteSource() { }


  AbstractTreeItem::InternalPointerType AbstractImageItem::getPointerType() const {
    return AbstractTreeItem::ImageAndNet;
  }


  void *AbstractImageItem::getPointer() const {
    return m_imageAndNet;
  }


  bool AbstractImageItem::hasImage(QString imageSerial) const {
    if(!m_imageAndNet) {
      return false;
    }
    return (QString::compare(m_imageAndNet->first, imageSerial) == 0) ||
            AbstractTreeItem::hasImage(imageSerial);
  }


  /**
   * This method is required to be implemented by the parent AbstractTreeItem class,
   * but for this it's a NOP.
   */
  void AbstractImageItem::sourceDeleted() { }
}
