#ifndef ConcurrentControlNetReader_h
#define ConcurrentControlNetReader_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <functional>

#include <QObject>
#include <QPair>
#include <QPointer>
#include <QString>
#include <QStringList>

#include "ProgressBar.h"

template<typename A> class QAtomicPointer;
template<typename A> class QFutureWatcher;

namespace Isis {
  class Control;
  class FileName;
  class Progress;
  class ProgressBar;


  /**
   * @brief This reads a control net in the background
   *
   * This class provides progress for reading a ControlNet.  Reading is done
   * in separate threads using QtConcurrent.  When reading large Pvl networks
   * the progress will hang at zero percent for a while.
   *
   * @author 2012-06-11 Ken Edmundson, Steven Lambright
   *
   * @internal
   *   @history 2017-08-09 Summer Stapleton - Added a try-catch block to handle invalid control
   *                           networks. Fixes #5068.
   */
  class ConcurrentControlNetReader : public QObject {
      Q_OBJECT

    public:
      ConcurrentControlNetReader();
      ~ConcurrentControlNetReader();
      void read(QString filename);
      void read(QStringList filenames);
      ProgressBar *progressBar();

    signals:
      void networksReady(QList<Control *>);

    private:
      void nullify();
      void start();

    private slots:
      void updateProgressValue();
      void mappedFinished();

    private:
      void initProgress();

    private:
      //! provides SIGNALS / SLOTS for FileNameToControlFunctor
      QFutureWatcher<Control *> * m_watcher;
      QStringList m_backlog;
      bool m_mappedRunning;
      QList<Progress *> m_progress;
      QPointer<ProgressBar> m_progressBar;
      QPointer<QTimer> m_progressUpdateTimer;

      /**
       * @brief
       *
       * @author ????-??-?? ???
       *
       * @internal
       */
      class FileNameToControlFunctor : public std::function<
          Control *(const QPair<FileName, Progress *> &)> {
        public:
          FileNameToControlFunctor(QThread *);
          FileNameToControlFunctor(const FileNameToControlFunctor &);
          ~FileNameToControlFunctor();
          Control * operator()(const QPair<FileName, Progress *> &) const;
          FileNameToControlFunctor & operator=(const FileNameToControlFunctor &);

        private:
          QThread *m_targetThread;
      };
  };
}

#endif
