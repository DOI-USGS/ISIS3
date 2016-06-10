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
#include "OpenProjectWorkOrder.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QtConcurrentMap>

#include "Cube.h"
#include "CubeAttribute.h"
#include "ProgressBar.h"
#include "Project.h"

namespace Isis {

  /**
   * Constructs an OpenProjectWorkOrder
   *
   * @param project The project 
   */
  OpenProjectWorkOrder::OpenProjectWorkOrder(Project *project) :
      WorkOrder(project) {
    QAction::setText(tr("&Open Project"));

    setCreatesCleanState(true);

    QStringList args = QCoreApplication::arguments();

    if (args.count() == 2) {
      connect(this, SIGNAL(openProjectFromCommandLine(QString)),
              project, SLOT(open(QString)), Qt::QueuedConnection);
      emit openProjectFromCommandLine(args.last());
    }
  }


  /**
   * Copy constructor
   *
   * @param other The other OpenProjectWorkOrder to initialize from
   */
  OpenProjectWorkOrder::OpenProjectWorkOrder(const OpenProjectWorkOrder &other) :
      WorkOrder(other) {
  }


  /**
   * Destructor
   */
  OpenProjectWorkOrder::~OpenProjectWorkOrder() {

  }


  /**
   * Clones the current OpenProjectWorkOrder
   *
   * @return @b OpenProjectWorkOrder * The OpenProjectWorkOrder clone
   */
  OpenProjectWorkOrder *OpenProjectWorkOrder::clone() const {
    return new OpenProjectWorkOrder(*this);
  }


  /**
   * Executes the OpenProjectWorkOrder
   *
   * @return @b bool true if the successfully executed
   */
  bool OpenProjectWorkOrder::execute() {
    bool success = WorkOrder::execute();

    // We dislike the progress bar
    delete progressBar();

    // If more than this work order is in the history, don't allow this operation
    if (success && project()->workOrderHistory().count()) {
      QMessageBox::critical(NULL, tr("Unable To Open a Project"),
                            tr("If you have modified your current project, you cannot open a new "
                               "project because this is not yet implemented"));
      success = false;
    }
    else if (success) {
      QString projectName = QFileDialog::getExistingDirectory(qobject_cast<QWidget *>(parent()),
                                                              tr("Open Project"));

      if (!projectName.isEmpty()) {
        project()->open(projectName);
      }
      else {
        success = false;
      }
    }

    return success;
  }
}
