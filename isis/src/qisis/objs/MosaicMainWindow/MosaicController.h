#ifndef MosaicWidgetController_H
#define MosaicWidgetController_H

#include <QObject>

#include <QPointer>

class QAction;
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

#include "ImageList.h"
#include "PvlObject.h"
#include "MosaicSceneWidget.h"
#include "ImageFileListWidget.h"

namespace Isis {
  class ControlNet;
  class FileName;
  class ImageReader;
  class MosaicSceneWidget;
  class ProgressBar;
  class PvlObject;

  /**
   * @brief
   *
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Stacy Alley
   *
   * @internal
   *   @history 2010-05-10 Christopher Austin - added cnet connectivity
   *                                 functionality and fixed a few design issues
   *   @history 2010-10-26 Tracie Sucharski Added missing includes to cpp after
   *                                  removing includes from ControlNet.h.
   *   @history 2011-08-08 Steven Lambright - Refactored. Now uses scene widget's
   *                                 preloadFromPvl.
   *   @history 2011-08-12 Steven Lambright - Export options now come from the
   *                           scene and the file list, not directly from this
   *                           controller. Fixes #342
   *   @history 2011-09-26 Steven Lambright - Calling openCubes many times in
   *                           a row now works.
   *   @history 2011-12-05 Steven Lambright - Added fixes for maximum number of
   *                           simultaneously open files. It now stays lower and
   *                           has an option to become drastically lower.
   *   @history 2011-12-16 Steven Lambright - Applies fixes for maximum number
   *                           of open files to opening project files and fixed
   *                           progress to be more accurate. Fixes #635.
   *   @history 2012-09-17 Steven Lambright - Added compatibility with old versions of project
   *                           files, updated to use changed mosaic scene widget constructor.
   *   @history 2013-03-19 Steven Lambright - Added option for changing default file list
   *                           columns in the settings menu.
   */

  class MosaicController : public QObject {
      Q_OBJECT

    public:
      MosaicController(QStatusBar *status, QSettings &settings);
      virtual ~MosaicController();

      MosaicSceneWidget *getMosaicScene() {
        return m_scene;
      }

      MosaicSceneWidget *getMosaicWorldScene() {
        return m_worldScene;
      }

      ImageFileListWidget *getImageFileList() {
        return m_fileList;
      }

      void addExportActions(QMenu &fileMenu);

      QProgressBar *getProgress();
      void saveProject();

      QList<QAction *> getSettingsActions();
      void saveSettings(QSettings &settings);

    signals:
      /**
       * Emitted when new images are available.
       */
      void imagesAdded(ImageList images);
      void imagesAdded(ImageList *images);

      void allImagesClosed();

    public slots:
      void saveProject(QString filename);
      void readProject(QString filename);
      void openImages(QStringList filenames);
      void openProjectImages(PvlObject projectImages);

    private slots:
      void changeMaxThreads();
      void imageClosed(QObject * image);
      void imagesReady(ImageList);
      void saveList();

    private:
      void applyMaxThreadCount();
      void convertV1ToV2(Pvl &project);

    private:
      ImageList m_images;

      QPointer<ImageFileListWidget> m_fileList;
      QPointer<MosaicSceneWidget> m_scene;
      QPointer<MosaicSceneWidget> m_worldScene;
      QPointer<ImageReader> m_imageReader;

      int m_maxThreads;

      // Cameras are not re-entrant and so this mutex will make sure they
      //   aren't overly abused
      QMutex *m_mutex;
  };
}

#endif

