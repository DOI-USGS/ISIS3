#ifndef ImageGroupTreeWidgetItem_H
#define ImageGroupTreeWidgetItem_H

#include <QObject>
#include <QTreeWidgetItem>


namespace Isis {

  class ImageList;

  /**
   * @author 2012-06-19 Tracie Sucharski
   *
   * @internal
   *   @history 2012-07-30 Kimberly Oyama and Steven Lambright - Added imageList()
   *                           and selectionChanged().
   */
  class ImageGroupTreeWidgetItem : public QObject, public QTreeWidgetItem {
    Q_OBJECT
    public:
      ImageGroupTreeWidgetItem(ImageList *imageList, QTreeWidget *parent = 0);
      virtual ~ImageGroupTreeWidgetItem();

      ImageList *imageList();
      void selectionChanged();

    private slots:
      void updateCount(int newCount);

    private:
      ImageList *m_imageList;
  };
}

#endif

