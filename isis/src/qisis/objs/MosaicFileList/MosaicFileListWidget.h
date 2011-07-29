#ifndef MosaicFileListWidget_H
#define MosaicFileListWidget_H

#include <QWidget>

class QProgressBar;
class QSettings;
class QTreeWidgetItem;

namespace Isis {
  class CubeDisplayProperties;
  class MosaicTreeWidget;
  class MosaicTreeWidgetItem;
  class PvlObject;

  /**
   * @brief A colored, grouped cube list
   *
   * @author 2011-07-29 Steven Lambright
   *
   * @internal
   *    @history 2011-07-29 Steven Lambright - Expansion state is now stored in
   *                            the project file. This change will cause older
   *                            versions of qmos to fail to read newer project
   *                            files. References #275.
   */
  class MosaicFileListWidget : public QWidget {
      Q_OBJECT
    public:
      MosaicFileListWidget(QSettings &settings, QWidget *parent = 0);
      virtual ~MosaicFileListWidget();

      QProgressBar *getProgress();
      void fromPvl(PvlObject &pvl);
      PvlObject toPvl() const;

      QList<QAction *> getViewActions();

    public slots:
      void addCubes(QList<CubeDisplayProperties *>);

    private:
      MosaicTreeWidgetItem *takeItem(QString filename,
                                     QList<QTreeWidgetItem *> &items);
      MosaicTreeWidget *p_tree; //!< Tree item associated with this mosaic item
  };
}

#endif

