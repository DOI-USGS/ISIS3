#ifndef ShapeReader_H
#define ShapeReader_H

#include <QObject>

#include <QPointer>

// This is the parent of the inner class
#include <functional>

#include "ShapeDisplayProperties.h"
#include "ShapeList.h"
#include "ProgressBar.h"
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
   * @author 2016-07-25 Tracie Sucharski
   *
   * @ingroup Visualization Tools
   * 
   * @internal 
   */
  class ShapeReader : public QObject {
      Q_OBJECT

    public:
      ShapeReader(QMutex *cameraMutex, bool requireFootprints = false, QObject *parent = NULL);
      virtual ~ShapeReader();

      QList<QAction *> actions(ShapeDisplayProperties::Property relevantDispProperties);
      QProgressBar *progress();

    signals:
      void shapesReady(ShapeList shapes);

    public slots:
      void read(PvlObject shapesObj);
      void read(QStringList shapeFileNames);
      void setSafeFileOpen(bool safeFileOpen);

    private slots:
      void shapesReady(int);
      void mappedFinished();
      //void setSmallNumberOfOpenShapes(bool useSmallNumber);

    private:
      Q_DISABLE_COPY(ShapeReader)

      template <typename Iterator>
      void read(Iterator begin, Iterator end) {
        int numNewEntries = 0;
        while (begin != end) {
          m_backlog.append(qVariantFromValue(*begin));
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
      QFutureWatcher<Shape *> * m_watcher;

      bool m_safeFileOpen;
      bool m_openFilled;
      int m_defaultAlpha;
      bool m_requireFootprints;

      bool m_mappedRunning;

    private:
      /**
       * Converts from file name or project representation to Shape *. This is designed to work
       *   with QtConcurrentMap.
       *
       * @author 2012-??-?? ???
       *
       * @internal
       */
      class VariantToShapeFunctor : public std::function<
          Shape *(const QVariant &)> {

        public:
          VariantToShapeFunctor(QMutex *cameraMutex, bool requireFootprints, QThread *targetThread,
              bool openFilled, int defaultAlpha);
          Shape *operator()(const QVariant &);

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

