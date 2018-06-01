#include "IsisDebug.h"

#include "AbstractImageItem.h"

#include <iostream>

#include <QString>
#include <QVariant>

#include "ControlNet.h"


namespace Isis {
  AbstractImageItem::AbstractImageItem(QString imageSerial,
      int avgCharWidth, AbstractTreeItem *parent)
    : AbstractTreeItem(parent), m_imageSerial(imageSerial) {
    calcDataWidth(avgCharWidth);
  }


  AbstractImageItem::~AbstractImageItem() { }


  QVariant AbstractImageItem::getData() const {
    return QVariant(m_imageSerial);
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
    return AbstractTreeItem::ImageSerial;
  }


  void *AbstractImageItem::getPointer() const {
    return &m_imageSerial;
  }


  bool AbstractImageItem::hasImage(QString imageSerial) const {
    return m_imageSerial == imageSerial || AbstractTreeItem::hasImage(imageSerial);
  }


  /**
   * This method is required to be implemented by the parent AbstractTreeItem class,
   * but for this it's a NOP.
   */
  void AbstractImageItem::sourceDeleted() { }
}
