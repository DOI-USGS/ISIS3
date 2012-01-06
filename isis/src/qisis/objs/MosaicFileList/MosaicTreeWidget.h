#ifndef MosaicTreeWidget_H
#define MosaicTreeWidget_H

#include <QTreeWidget>

class QProgressBar;

namespace Isis {
  class CubeDisplayProperties;
  class MosaicTreeWidgetItem;
  class ProgressBar;

  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class MosaicTreeWidget : public QTreeWidget {
      Q_OBJECT

    public:
      MosaicTreeWidget(QWidget *parent = 0);
      virtual ~MosaicTreeWidget();
      void addCubes(QList<CubeDisplayProperties *> cubes);
      QTreeWidgetItem *addGroup(QString groupName, int index = -1);
      void refit();

      QProgressBar *getProgress();
      void disableSort();

      QList<QAction *> getViewActions();
      void updateViewActs();

    public slots:

    protected:
      void dropEvent(QDropEvent *event);
      void mousePressEvent(QMouseEvent *event);
      void contextMenuEvent(QContextMenuEvent *event);

    private slots:
      QTreeWidgetItem *addGroup();
      void cubeChanged(CubeDisplayProperties *);
      void cubeDeleted(QObject *cubeDisplay);
      void deleteSelectedGroups();
      void onItemChanged(QTreeWidgetItem *, int);
      void onSelectionChanged();
      void renameSelectedGroup();
      void requestCloseSelected();
      void toggleColumnVisible();
      void updateDragAndDropability();

    private:
      MosaicTreeWidgetItem *prepCube(CubeDisplayProperties *cube);
      bool groupInList(QList<QTreeWidgetItem *>);
      bool mosaicItemInList(QList<QTreeWidgetItem *>);
      QList<CubeDisplayProperties *> selectedDisplays();

      MosaicTreeWidgetItem *treeItem(CubeDisplayProperties *);
      ProgressBar *p_progress;

      QList<QAction *> p_viewActs;
  };
};

#endif

