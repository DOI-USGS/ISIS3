#ifndef ImageTreeWidget_H
#define ImageTreeWidget_H

#include <QPointer>
#include <QTreeWidget>

class QProgressBar;

namespace Isis {
  class Directory;
  class DisplayProperties;
  class Image;
  class ImageList;
  class ImageTreeWidgetItem;
  class ProgressBar;

  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2012-09-12 Steven Lambright - createGroup() will no longer fail if a NULL
   *                           image list item is provided, it will instead create the
   *                           group at the root level.
   *   @history 2012-09-17 Steven Lambright - Optimized for 50,000+ images. Added
   *                           m_displayPropsToTreeItemLookup (changed n/2 operation to log(n)).
   *                           Added queued selection changed (changed some n^2 operations to n).
   *   @history 2013-03-19 Steven Lambright - Added setDefaultFileListCols() and set column
   *                           defaults in constructor.
   *   @history 2013-06-27 Tracie Sucharski - Fixed bug which resulted in a seg fault when
   *                           attempting to add a group to the file list.  This was introduced
   *                           in the changes for "a.out".  Fixes #1693.
   *   @history 2013-07-02 Tracie Sucharski - Restored old qmos functionality for inserting new
   *                           groups and fixed bug dragging and dropping groups.  Fixes #1697.
   *
   * Selection changes are very slow (time complexity in our code is delta-selected, but there is a
   *     very high constant that scales to the tree size - maybe an N complexity? making a total
   *     time complexity of [delta selected]*[items in view] -- as bad as N^2).
   *       https://bugreports.qt-project.org/browse/QTBUG-26143.
   *     If we want this tree to no longer be a performance problem, we're going to have to rewrite
   *     it completely.
   */
  class ImageTreeWidget : public QTreeWidget {
      Q_OBJECT

    public:

      enum TreeItemContainerType {
        ImageGroupType = 1,
        ImageListNameType
      };

      ImageTreeWidget(Directory *directory = 0, QWidget *parent = 0);
      virtual ~ImageTreeWidget();

      QList<QAction *> actions();

      QTreeWidgetItem *addGroup(QString imageListName, QString groupName = "", int index = -1);

      QTreeWidgetItem *createGroup(QTreeWidgetItem *imageListItem, QString groupName = "",
                                   int index = -1);
      QTreeWidgetItem *createImageListNameItem(QString name);

      void refit();

      void disableSort();
      void enqueueReadDisplayProperties(ImageTreeWidgetItem *);

      QList<QAction *> getViewActions();
      bool groupInList(QList<QTreeWidgetItem *>);
      void updateViewActs();

      ImageTreeWidgetItem *prepCube(ImageList *imageList, Image *image);

      ImageList imagesInView();

    public:
      /**
       * @author 2012-??-?? ???
       *
       * @internal
       */
      class ImagePosition {
        public:
          ImagePosition();
          ImagePosition(int outerIndex, int innerIndex);
          ImagePosition(const ImagePosition &other);

          virtual ~ImagePosition();

          void setPosition(int outerIndex, int innerIndex);

          int group() const;
          int index() const;
          bool isValid() const;

          void swap(ImagePosition &other);

          bool operator<(const ImagePosition &rhs);
          ImagePosition &operator=(const ImagePosition &rhs);

        private:
          void init();

        private:
          int m_group;
          int m_index;
      };

    signals:
      void queueSelectionChanged();
      void queueReadDisplayProperties();

    protected:
      void dropEvent(QDropEvent *event);
      void mousePressEvent(QMouseEvent *event);
      void contextMenuEvent(QContextMenuEvent *event);

    private slots:
      QTreeWidgetItem *addGroup();
      void deleteSelectedGroups();
      void imageDeleted(QObject *image);
      void onItemChanged(QTreeWidgetItem *, int);
      void onSelectionChanged();
      void onQueuedReadDisplayProperties();
      void onQueuedSelectionChanged();
      void propertiesChanged(DisplayProperties *);
      void renameSelectedGroup();
      void requestCloseSelected();
      void toggleColumnVisible();
      void updateDragAndDropability();
      void setDefaultFileListCols();

    private:
      bool mosaicItemInList(QList<QTreeWidgetItem *>);
      ImageList selectedDisplays();

      QTreeWidgetItem *imageListTreeItem(QString imageListName);
      ImageTreeWidgetItem *treeItem(DisplayProperties *);
      ImageTreeWidgetItem *treeItem(Image *);

      Directory *m_directory;
      QList<QAction *> m_viewActs;
      QPointer<QAction> m_setFileListColsAct;
      QMap<DisplayProperties *, ImageTreeWidgetItem *> m_displayPropsToTreeItemLookup;
      bool m_queuedSelectionChanged;

      QList<ImageTreeWidgetItem *> m_queuedReadDisplayPropertiesItems;
  };
};

#endif

