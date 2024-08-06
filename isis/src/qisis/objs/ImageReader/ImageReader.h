#ifndef ImageReader_H
#define ImageReader_H

#include <QObject>

#include <QPointer>

// This is the parent of the inner class
#include <functional>

// We're using an internal enum for actions()
#include "ImageDisplayProperties.h"
// This is in a signal
#include "ImageList.h"
// This is inlined in the templated read()
#include "ProgressBar.h"
// This is in a slot
#include "PvlObject.h"

template <typename A> class QFutureWatcher;
class QMutex;
class QProgressBar;
class QStringList;
class QVariant;

namespace Isis {
  class PvlObject;

  /**
   * @brief
   *
   * @author 2012-??-?? ???
   *
   * @ingroup Visualization Tools
   * 
   * @internal 
   *   @history 2013-07-01 Tracie Sucharski - Clean up ImageList in mappedFinished method to get
   *                           rid of Null Image pointers from the list.  Fixes #1693, #1696.
   *   @history 2016-07-08 Tracie Sucharski - If the requireFootprints is set to false do not even
   *                           attempt to create a footprint.  This was changed from always initing
   *                           the footprint, but only throwing error if requireFootprints was true.
   */
  class ImageReader : public QObject {
      Q_OBJECT

    public:
      ImageReader(QMutex *cameraMutex, bool requireFootprints = true, QObject *parent = NULL);
      virtual ~ImageReader();

      QList<QAction *> actions(ImageDisplayProperties::Property relevantDispProperties);
      QProgressBar *progress();

    signals:
      void imagesReady(ImageList images);

    public slots:
      void askDefaultAlpha();
      void read(PvlObject imagesObj);
      void read(QStringList imageFileNames);
      void setOpenFilled(bool openFilled);
      void setSafeFileOpen(bool safeFileOpen);

    private slots:
      void imageReady(int);
      void mappedFinished();
      //void setSmallNumberOfOpenImages(bool useSmallNumber);

    private:
      Q_DISABLE_COPY(ImageReader)

      template <typename Iterator>
      void read(Iterator begin, Iterator end) {
        int numNewEntries = 0;
        while (begin != end) {
          m_backlog.append(QVariant::fromValue(*begin));
          numNewEntries++;
          begin++;
        }

        m_progress->setRange(0, m_progress->maximum() + numNewEntries);

        start();
      }

      void initProgress();
      void start();

      void readSettings();
      void writeSettings();

    private:
      /**
       * Variant Internal Format: (QString|PvlObject). This stores what we
       *   haven't started reading yet in QtConcurrent.
       */
      QPointer<QAction> m_askAlphaAct;
      QList<QVariant> m_backlog;
      QMutex *m_cameraMutex;
      QPointer<QAction> m_openFilledAct;
      QPointer<ProgressBar> m_progress;
      QPointer<QAction> m_safeFileOpenAct;
      QFutureWatcher<Image *> * m_watcher;

      bool m_safeFileOpen;
      bool m_openFilled;
      int m_defaultAlpha;
      bool m_requireFootprints;

      bool m_mappedRunning;

    private:
      /**
       * Converts from file name or project representation to Image *. This is designed to work
       *   with QtConcurrentMap.
       *
       * @author 2012-??-?? ???
       *
       * @internal
       */
      class VariantToImageFunctor : public std::function<
          Image *(const QVariant &)> {

        public:
          VariantToImageFunctor(QMutex *cameraMutex, bool requireFootprints, QThread *targetThread,
              bool openFilled, int defaultAlpha);
          Image *operator()(const QVariant &);

        private:
          QMutex *m_mutex;
          QThread *m_targetThread;

          int m_defaultAlpha;
          bool m_openFilled;
          bool m_requireFootprints;
      };
  };
};

#endif

