#ifndef ConcurrentControlNetReader_h
#define ConcurrentControlNetReader_h
/**
 * @file
 * $Revision: 1.9 $
 * $Date: 2009/07/15 17:33:52 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <QObject>
#include <QPair>
#include <QPointer>
#include <QString>
#include <QStringList>

#include "ControlNetFile.h"
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
   *                         networks. Fixes #5068.
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
      class FileNameToControlFunctor : public std::unary_function<
          const QPair<FileName, Progress *> &, Control *> {
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
