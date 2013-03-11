#include "ImageGroupTreeWidgetItem.h"

#include <QDebug>

#include "ImageList.h"

namespace Isis {

  /**
   * ImageGroupTreeWidgetItem constructor.
   * ImageGroupTreeWidgetItem is derived from QTreeWidgetItem
   *
   *
   * @param parent
   */
  ImageGroupTreeWidgetItem::ImageGroupTreeWidgetItem(ImageList *imageList, QTreeWidget *parent) :
      QTreeWidgetItem(parent, UserType) {
    m_imageList = imageList;

    setText(0, m_imageList->name());
    updateCount(m_imageList->count());
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
    setIcon(0, QIcon(":pictures"));

    connect(m_imageList, SIGNAL(destroyed(QObject *)), this, SLOT(deleteLater()));
    connect(m_imageList, SIGNAL(countChanged(int)), this, SLOT(updateCount(int)));
  }


  ImageGroupTreeWidgetItem::~ImageGroupTreeWidgetItem() {
    m_imageList = NULL;
  }


  ImageList *ImageGroupTreeWidgetItem::imageList() {
    return m_imageList;
  }


  void ImageGroupTreeWidgetItem::selectionChanged() {
    foreach (Image *image, *m_imageList) {
      image->displayProperties()->setSelected(isSelected());
    }
  }


  void ImageGroupTreeWidgetItem::updateCount(int newCount) {
    setToolTip(0, tr("%1 Images").arg(newCount));
  }
}
