#ifndef MosaicWidgetController_H
#define MosaicWidgetController_H

#include <QObject>

template <typename A> class QFutureWatcher;
template <typename A> class QList;
class QMenu;
class QMutex;
class QProgressBar;
class QSettings;
class QStatusBar;

// This is required since we have a slot with a QStringList
#include <QStringList>

// This is the parent of the inner class
#include <functional>

namespace Isis {
  class ControlNet;
  class CubeDisplayProperties;
  class MosaicFileListWidget;
  class MosaicSceneWidget;
  class ProgressBar;
  class PvlObject;

  /**
   * @brief
   *
   * @ingroup Visualization Tools
   *
   * @author Stacy Alley
   *
   * @internal
   *
   *  @history 2010-05-10 Christopher Austin - added cnet connectivity
   *                                functionality and fixed a few design issues
   *  @history 2010-10-26 Tracie Sucharski Added missing includes to cpp after
   *                                removing includes from ControlNet.h.
   */

  class MosaicController : public QObject {
      Q_OBJECT

    public:
      MosaicController(QStatusBar *status, QSettings &settings);
      virtual ~MosaicController();

      MosaicSceneWidget *getMosaicScene() {
        return p_scene;
      }

      MosaicSceneWidget *getMosaicScene2() {
        return p_scene2;
      }

      MosaicFileListWidget *getMosaicFileList() {
        return p_fileList;
      }

      void addExportActions(QMenu &fileMenu);

      QProgressBar *getProgress();
      void saveProject();

    signals:
      /**
       * Emitted when new cubes are available.
       */
      void cubesAdded(QList<CubeDisplayProperties *>);

      void allCubesClosed();

    public slots:
      void saveProject(QString filename);
      void readProject(QString filename);
      void openCubes(QStringList filenames);
      void cubeDisplayReady(int);

    private slots:
      void cubeClosed(QObject * cubeDisplay);
      void exportView();
      void loadFinished();
      void saveList();
      void updateProgress(int);

    private:
      void flushCubes(bool force = false);

    private:
      QList<CubeDisplayProperties *> p_cubes;
      QList<CubeDisplayProperties *> p_unannouncedCubes;

      MosaicFileListWidget *p_fileList;
      MosaicSceneWidget *p_scene;
      MosaicSceneWidget *p_scene2;
      ProgressBar *p_progress;
      PvlObject *m_projectPvl;

      // Cameras are not re-entrant and so this mutex will make sure they
      //   aren't overly abused
      QMutex *m_mutex;

      QFutureWatcher< CubeDisplayProperties * > * m_watcher;

      class FilenameToDisplayFunctor : public std::unary_function<
          const QString &, CubeDisplayProperties *> {

        public:
          FilenameToDisplayFunctor(QMutex *cameraMutex, QThread *targetThread);
          CubeDisplayProperties *operator()(const QString &);

        private:
          QMutex *m_mutex;
          QThread *m_targetThread;
      };

      class ProjectToDisplayFunctor : public std::unary_function<
          const PvlObject &, CubeDisplayProperties *> {

        public:
          ProjectToDisplayFunctor(QMutex *cameraMutex, QThread *targetThread);
          CubeDisplayProperties *operator()(const PvlObject &);

        private:
          QMutex *m_mutex;
          QThread *m_targetThread;
      };
  };
};

#endif

