/**
 * @file
 * $Revision: 1.19 $
 * $Date: 2010/03/22 19:44:53 $
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
#include "ImportControlNetWorkOrder.h"

#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QtConcurrentMap>

#include "Control.h"
#include "ControlList.h"
#include "ControlNet.h"
#include "IException.h"
#include "Progress.h"
#include "Project.h"
#include "ProjectItem.h"
#include "ProjectItemModel.h"

namespace Isis {

  ImportControlNetWorkOrder::ImportControlNetWorkOrder(Project *project) :
      WorkOrder(project) {

    // This is an asynchronous workorder
    m_isSynchronous = false;

    m_watcher = NULL;

    QAction::setText(tr("Import &Control Networks..."));

    setModifiesDiskState(true);

    m_watcher = new QFutureWatcher<Control *>;
    connect(m_watcher, SIGNAL(resultReadyAt(int)), this, SLOT(cnetReady(int)));
  }


  ImportControlNetWorkOrder::ImportControlNetWorkOrder(const ImportControlNetWorkOrder &other) :
      WorkOrder(other) {

    m_watcher = NULL;
    m_watcher = new QFutureWatcher<Control *>;
    connect(m_watcher, SIGNAL(resultReadyAt(int)), this, SLOT(cnetReady(int)));

  }


  ImportControlNetWorkOrder::~ImportControlNetWorkOrder() {
    delete m_watcher;
    m_watcher = NULL;
  }


  ImportControlNetWorkOrder *ImportControlNetWorkOrder::clone() const {
    return new ImportControlNetWorkOrder(*this);
  }


  /**
   * @brief Sets up the work order for execution.
   *
   * This method prompts the user for a control net to open. That control net is then
   * saved using setInternalData data. This method was renamed from execute() to setupExecution()
   *
   * @see WorkOrder::setupExecution()
   *
   * @return bool Returns a boolean. This boolean is true if the internal data was set correctly.
   */
  bool ImportControlNetWorkOrder::setupExecution() {
    QUndoCommand::setText(tr("Import Control Networks"));

    WorkOrder::setupExecution();

    QStringList cnetFileNames = QFileDialog::getOpenFileNames(
        qobject_cast<QWidget *>(parent()),
        tr("Import Control Networks"), "",
        tr("Isis control nets (*.net);;All Files (*)"));

    if (!cnetFileNames.isEmpty()) {
      QUndoCommand::setText(tr("Import %1 Control Networks").arg(cnetFileNames.count()));
    }

    setInternalData(cnetFileNames);

    return internalData().count() > 0;
  }

  /**
   * @brief Imports the control network asynchronously.
   *
   * This method asynchronously imports the control net. This method replaces both
   * syncRedo() and asyncRedo().
   */
  void ImportControlNetWorkOrder::execute() {

    QDir cnetFolder = project()->addCnetFolder("controlNetworks");

    QStringList cnetFileNames = internalData();

    QList< QPair<FileName, Progress *> > cnetFileNamesAndProgress;
    foreach (FileName fileName, cnetFileNames) {
      Progress *readProgress = new Progress;
      cnetFileNamesAndProgress.append(qMakePair(fileName, readProgress));
      readProgress->DisableAutomaticDisplay();
      m_readProgresses.append(readProgress);
    }

    m_watcher->setFuture(QtConcurrent::mapped(cnetFileNamesAndProgress,
                                              CreateControlsFunctor(project(), cnetFolder)));

    while (!m_watcher->isFinished()) {
      setProgressRange(0, 100 * m_readProgresses.count());
      int totalProgress = 0;

      for (int i = 0; i < m_readProgresses.count(); i++) {
        Progress *progress = m_readProgresses[i];

        if (m_watcher->future().isResultReadyAt(i)) {
          totalProgress += 100;
        }
        else if (progress->MaximumSteps() > 0) {
          double progressPercent = progress->CurrentStep() / (double)progress->MaximumSteps();
          //  Estimating the Read is 90% and Write 10% (ish)
          totalProgress += qRound(progressPercent * 90);
        }
      }

      setProgressValue(totalProgress);

      QThread::yieldCurrentThread();
    }
  }

  /**
   * @brief Clears progress.
   *
   * This method clears the progresses created in execute(). This method was renamed
   * from postSyncRedo() to postExecution().
   */
  void ImportControlNetWorkOrder::postExecution() {

    foreach (Progress *progress, m_readProgresses) {
      delete progress;
    }
    m_readProgresses.clear();
  }

  /**
   * @brief Deletes the control network
   *
   * This method deletes the control network from the project. This method is was
   * renamed from undoSyncRedo() to undoExecution().
   */
  void ImportControlNetWorkOrder::undoExecution() {
    if (m_watcher->isFinished()) {
      ControlList *list = project()->controls().last();
      // Remove the controls from disk.
      list->deleteFromDisk(project());
      // Remove the controls from the model, which updates the tree view.
      ProjectItem *currentItem =
          project()->directory()->model()->findItemData(QVariant::fromValue(list));
      project()->directory()->model()->removeItem(currentItem);
    }
  }


  ImportControlNetWorkOrder::CreateControlsFunctor::CreateControlsFunctor(
      Project *project, QDir destinationFolder) {
    m_project = project;
    m_destinationFolder = destinationFolder;
  }


  Control *ImportControlNetWorkOrder::CreateControlsFunctor::operator()(
      const QPair<FileName, Progress *> &cnetFileNameAndProgress) {

    QString cnetFileName = cnetFileNameAndProgress.first.original();
    ControlNet *cnet = new ControlNet();
    cnet->SetMutex(m_project->mutex());
    cnet->ReadControl(cnetFileName, cnetFileNameAndProgress.second);

    QString baseFilename = FileName(cnetFileName).name();
    QString destination = m_destinationFolder.canonicalPath() + "/" + baseFilename;

    cnet->Write(destination);

    Control *control = new Control(cnet, destination);
    return control;
  }


  void ImportControlNetWorkOrder::cnetReady(int ready) {

    Control *control = m_watcher->resultAt(ready);
    project()->addControl(control);
  }
}
