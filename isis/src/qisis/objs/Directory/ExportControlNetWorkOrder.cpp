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
#include "ExportControlNetWorkOrder.h"

#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>

#include "Control.h"
#include "ControlList.h"
#include "ControlNet.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Project.h"

namespace Isis {

  ExportControlNetWorkOrder::ExportControlNetWorkOrder(Project *project) :
      WorkOrder(project) {
    QAction::setText(tr("&Export Control Network..."));
  }


  ExportControlNetWorkOrder::ExportControlNetWorkOrder(const ExportControlNetWorkOrder &other) :
      WorkOrder(other) {
  }


  ExportControlNetWorkOrder::~ExportControlNetWorkOrder() {

  }


  ExportControlNetWorkOrder *ExportControlNetWorkOrder::clone() const {
    return new ExportControlNetWorkOrder(*this);
  }


  /**
   * Currently, this work order only works with either no data (file menu) or with a single 
   *   control network.
   * 
   * @param controls The current context we're inquiring about
   * 
   * @return bool True if this work order functions with the given control list
   */
  bool ExportControlNetWorkOrder::isExecutable(ControlList *controls) {
    return (controls->count() == 1);
  }


  /**
   * Prompts the user for input. If there is no context, we ask the user to select a control. Once 
   *   we have a control (via context or asking the user), we then ask for a output cnet file name.
   *   The relevant data is stored in internalData().
   * 
   * @return bool 
   */
  bool ExportControlNetWorkOrder::execute() {
    bool success = WorkOrder::execute();

    if (success) {
      QStringList internalData;

      Control *control = NULL;
      if (controlList()->isEmpty()) {
        QMap<Control *, QString> cnetChoices;
        foreach (ControlList *list, project()->controls()) {
          foreach (Control *control, *list) {
            cnetChoices[control] = tr("%1/%2").arg(list->name())
                .arg(control->displayProperties()->displayName());
          }
        }

        QStringList cnetNames = cnetChoices.values();
        qSort(cnetNames);

        QString choice = QInputDialog::getItem(NULL, tr("Select Control"),
            tr("Please choose a control to export."), cnetNames, 0, false, &success);

        control = cnetChoices.key(choice);
        internalData.append(control->id());
      }
      else {
        control = controlList()->first();
      }

      QString destination =
          QFileDialog::getSaveFileName(NULL, QString("Export Control Network"),
          "./" + FileName(control->fileName()).name());

      if (destination.isEmpty()) {
        success = false;
      }
      internalData.append(destination);

      setInternalData(internalData);
    }

    return success;
  }


  /**
   * Use internalData() and write the control network into the output file. Stores errors in 
   *   m_warning which will be reported in postSyncRedo().
   */
  void ExportControlNetWorkOrder::asyncRedo() {
    QString destination;
    Control *control = NULL;

    if (controlList()->isEmpty()) {
      destination = internalData()[1];

      QString controlId = internalData()[0];
      control = project()->control(controlId);
    }
    else {
      destination = internalData()[0];
      control = controlList()->first();
    }

    try {
      control->controlNet()->Write(destination);
    }
    catch (IException &e) {
      m_warning = e.toString();
    }
  }


  /**
   * Display any warnings that occurred during the asynchronous computations.
   */
  void ExportControlNetWorkOrder::postSyncRedo() {
    if (!m_warning.isEmpty()) {
      project()->warn(m_warning);
      m_warning.clear();
    }
  }
}
