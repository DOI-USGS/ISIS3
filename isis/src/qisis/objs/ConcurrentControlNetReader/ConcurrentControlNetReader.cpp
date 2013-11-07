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

#include "ConcurrentControlNetReader.h"

#include <algorithm>
#include <iostream>

#include <QDebug>
#include <QFuture>
#include <QFutureWatcher>
#include <QString>
#include <QtConcurrentRun>
#include <QtConcurrentMap>
#include <QtCore>
#include <QTimer>

#include "Control.h"
#include "ControlNet.h"
#include "FileName.h"
#include "IString.h"
#include "Progress.h"


using std::cerr;
using std::cout;
using std::swap;


namespace Isis {

  /**
   * Allocates memory at construction instead of as needed
   */
  ConcurrentControlNetReader::ConcurrentControlNetReader() {
    nullify();

    m_mappedRunning = false;

    m_progressBar = new ProgressBar("Reading Control Nets");
    m_watcher = new QFutureWatcher<Control *>;

    initProgress();

    connect(m_watcher, SIGNAL(finished()), this, SLOT(mappedFinished()));
  }


  /**
   * This destructor will cancel all running threads and block.
   */
  ConcurrentControlNetReader::~ConcurrentControlNetReader() {

    if (m_watcher)
    {
      m_watcher->cancel();
      m_watcher->waitForFinished();
    }

    delete m_watcher;
    m_watcher = NULL;

    delete m_progressBar;
  }


  ProgressBar *ConcurrentControlNetReader::progressBar() {
    return m_progressBar;
  }


  /**
   * @param filename The filename of the network to read
   */
  void ConcurrentControlNetReader::read(QString filename) {
    m_backlog.append(filename);

    if (!m_progressBar.isNull()) {
      m_progressBar->setRange(0, m_progressBar->maximum() + 1);
    }

    start();
  }


  /**
   * @param filenames The filenames of the networks to read
   */
  void ConcurrentControlNetReader::read(QStringList filenames) {
    m_backlog.append(filenames);

    if (!m_progressBar.isNull()) {
      m_progressBar->setRange(0, m_progressBar->maximum() + filenames.size());
    }

    start();
  }


  /**
   * Initializes members to NULL
   */
  void ConcurrentControlNetReader::nullify() {
    m_watcher = NULL;
  }


  void ConcurrentControlNetReader::initProgress() {
    if (m_progressBar) {
      m_progressBar->setVisible(false);
      m_progressBar->setRange(0, 100);
      m_progressBar->setValue(0);
    }
  }


  void ConcurrentControlNetReader::start() {
    if (!m_backlog.isEmpty() && !m_mappedRunning) {
      if (!m_progressBar.isNull()) {
        m_progressBar->setVisible(true);
      }

      delete m_progressUpdateTimer;

      QList< QPair<FileName, Progress *> > functorInput;
      foreach (QString backlogFileName, m_backlog) {
        Progress *progress = new Progress;
        progress->DisableAutomaticDisplay();
        m_progress.append(progress);

        functorInput.append(qMakePair(FileName(backlogFileName), progress));
      }

      QFuture< Control * > networks = QtConcurrent::mapped(
          functorInput,
          FileNameToControlFunctor(QThread::currentThread()));

      m_watcher->setFuture(networks);
      m_mappedRunning = true;
      m_backlog.clear();

      m_progressUpdateTimer = new QTimer;
      connect(m_progressUpdateTimer, SIGNAL(timeout()),
              this, SLOT(updateProgressValue()));
      m_progressUpdateTimer->start(100);
    }
  }

  void ConcurrentControlNetReader::updateProgressValue() {
    if (!m_mappedRunning) {
      foreach (Progress *progress, m_progress) {
        delete progress;
      }

      m_progress.clear();
    }

    int progressMin = 0;
    int progressMax = (m_progress.count() + m_backlog.count()) * 1000;
    int progressCurrent = 0;

    foreach (Progress *progress, m_progress) {
      if (progress->MaximumSteps()) {
        if (progress->CurrentStep() < progress->MaximumSteps()) {
          double progressPercent = progress->CurrentStep() / (double)progress->MaximumSteps();

          progressCurrent += qFloor(progressPercent * 1000);
        }
        else {
          progressCurrent += 1000;
        }
      }
    }

    if (m_progressBar) {
      if (progressMax > 0) {
        m_progressBar->setRange(progressMin, progressMax);
        m_progressBar->setValue(progressCurrent);
      }
      else {
        m_progressBar->setRange(0, 100);
        m_progressBar->setValue(100);
      }
    }
  }


  void ConcurrentControlNetReader::mappedFinished() {
    m_mappedRunning = false;

    delete m_progressUpdateTimer;
    updateProgressValue();

    QList<Control *> networks(m_watcher->future().results());
    emit networksReady(networks);

    if (!m_backlog.isEmpty()) {
      start();
    }
    else {
      initProgress();
    }
  }


  ConcurrentControlNetReader::FileNameToControlFunctor::FileNameToControlFunctor(
      QThread *targetThread) {
    m_targetThread = targetThread;
  }


  ConcurrentControlNetReader::FileNameToControlFunctor::FileNameToControlFunctor(
      const FileNameToControlFunctor & other) {
    m_targetThread = other.m_targetThread;
  }


  ConcurrentControlNetReader::FileNameToControlFunctor::~FileNameToControlFunctor() {
    m_targetThread = NULL;
  }


  Control * ConcurrentControlNetReader::FileNameToControlFunctor::operator()(
      const QPair<FileName, Progress *> &fileNameAndProgress) const {
    QString fileNameString = fileNameAndProgress.first.expanded();
    Progress *progress = fileNameAndProgress.second;

    ControlNet *newCnet = new ControlNet(fileNameString, progress);
    Control *result = new Control(newCnet, fileNameString);

    newCnet->setParent(result);
    result->moveToThread(m_targetThread);

    return result;
  }


  ConcurrentControlNetReader::FileNameToControlFunctor &
      ConcurrentControlNetReader::FileNameToControlFunctor::operator=(
        const FileNameToControlFunctor &rhs) {
    m_targetThread = rhs.m_targetThread;
    return *this;
  }
}

