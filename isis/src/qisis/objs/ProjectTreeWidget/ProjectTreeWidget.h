#ifndef ProjectTreeWidget_H
#define ProjectTreeWidget_H

#include <QPointer>
#include <QStringList>
#include <QTreeWidget>


namespace Isis {
  class BundleResults;
  class Control;
  class ControlList;
  class Directory;
  class ImageList;

  /**
   *
   * @author 2012-06-05 Ken Edmundson
   *
   * @internal
   *   @history 2012-07-30 Kimberly Oyama and Steven Lambright - Improved context menu support and
   *                           selection of image list.
   *   @history 2012-12-19 Steven Lambright and Stuart Sides - Finished initial implementation of
   *                           rename project use case. Text will now remain in editing mode if the
   *                           project name is invalid.
   *   @history 2014-07-14 Kimberly Oyama - Added work order for Result tree item. The context menu
   *                           will only have "View Correlation Matrix" for now. This work order
   *                           will move when we figure out the rest of the tree structure for the
   *                           bundle adjust items (settings, results, statistics, etc.).
   */
  class ProjectTreeWidget : public QTreeWidget {
      Q_OBJECT

    public:
      ProjectTreeWidget(Directory *directory = 0, QWidget *parent = 0);
      virtual ~ProjectTreeWidget();

    signals:
      void delayedEnableEditing(QTreeWidgetItem *itemToEnableEditingOn);

    public slots:
      void addControlGroup(ControlList *controlList);
      void addControl(Control *control);
      void addImageGroup(ImageList *images);
      void addBundleResults(BundleResults *bundleResults);

    protected:
      void contextMenuEvent(QContextMenuEvent *event);

    private:
      void initProjectTree();

    private slots:
      void enableEditing(QTreeWidgetItem *item);

      void onItemChanged(QTreeWidgetItem *item, int column);
      void onProjectNameChanged();
      void onSelectionChanged();

    private:
      Directory *m_directory;

      QTreeWidgetItem *m_projectItem;
      QTreeWidgetItem *m_cnetsParentItem;
      QTreeWidgetItem *m_imagesParentItem;
      QTreeWidgetItem *m_shapeParentItem;
      QTreeWidgetItem *m_targetParentItem;
      QTreeWidgetItem *m_sensorsParentItem;
      QTreeWidgetItem *m_spacecraftParentItem;
//       QTreeWidgetItem *m_sensorsParentItem;
      QTreeWidgetItem *m_resultsParentItem;

      bool m_ignoreEdits;
  };
};

#endif

