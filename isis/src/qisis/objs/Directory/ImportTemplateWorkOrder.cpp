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
#include "ImportTemplateWorkOrder.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QDebug>

#include "Project.h"
#include "ProjectItemModel.h"
#include "Template.h"
#include "TemplateList.h"

namespace Isis {
  /**
   * Creates a work order to import a template.
   *
   * @param *project Pointer to the project this work order belongs to
   */
  ImportTemplateWorkOrder::ImportTemplateWorkOrder(Project *project) :
      WorkOrder(project) {

    m_isUndoable = true;
    m_list = NULL;

    QAction::setText(tr("Import Template"));
    QUndoCommand::setText(tr("Import Template"));
    setModifiesDiskState(true);

  }


  /**
   * Creates a copy of the other ImportTemplateWorkOrder
   *
   * @param &other ImportTemplateWorkOrder to copy the state from
   */
  ImportTemplateWorkOrder::ImportTemplateWorkOrder(const ImportTemplateWorkOrder &other) :
      WorkOrder(other) {
  }


  /**
   * Destructor
   */
  ImportTemplateWorkOrder::~ImportTemplateWorkOrder() {
    m_list = NULL;
  }


  /**
   * This method clones the current ImportTemplateWorkOrder and returns it.
   *
   * @return ImportTemplateWorkOrder Clone
   */
  ImportTemplateWorkOrder *ImportTemplateWorkOrder::clone() const {
    return new ImportTemplateWorkOrder(*this);
  }


  /**
   * This method returns true if the user clicked on a project tree node with the text
   * "Templates".
   * This is used by Directory::supportedActions(DataType data) to determine what actions are
   * appended to context menus.
   *
   * @param item The ProjectItem that was clicked
   *
   * @return bool True if the user clicked on a project tree node named "Templates"
   */
  bool ImportTemplateWorkOrder::isExecutable(ProjectItem *item) {
    QString itemType = item->text();
    setInternalData(QStringList(itemType));

    return (itemType == "Maps" || itemType == "Registrations" || itemType == "Templates");
  }


  /**
   * @brief Sets up the work order for execution.
   *
   * This method prompts the user for a template to open.
   *
   * @see WorkOrder::setupExecution()
   *
   * @return bool Returns true if we have at least one template file name.
   */
  bool ImportTemplateWorkOrder::setupExecution() {
    WorkOrder::setupExecution();

    QString itemType;
    QString filterText =
        "Please select a file type;; Maps (*.def *.map *.pvl);; Registrations (*.def *.pvl)";

    // If clicked "File"->"Import"->"Import Templates"
    if (internalData().isEmpty()) {
      itemType = "Templates";
    }
    // If clicked "Import Templates" from under "Maps" or "Registrations" ProjectItems rightclicks
    else {
      itemType = internalData().at(0);

      if (itemType == "Maps") { filterText = "Maps (*.def *.map *.pvl)"; }
      else if (itemType == "Registrations") { filterText = "Registrations (*.def *.pvl)"; }

      internalData().clear();
    }

    QString selectedFilter;
    QStringList templateFileNames;

    templateFileNames = QFileDialog::getOpenFileNames(
        qobject_cast<QWidget *>(parent()),
        "Import " + itemType,
        QString(),
        filterText,
        &selectedFilter);

    if (!templateFileNames.isEmpty() && !selectedFilter.isEmpty()) {
      QUndoCommand::setText(tr("Import %1 Template(s)").arg(templateFileNames.count()));
    }
    else {
      return false;
    }

    // The user must choose a filter to import any file. The type is saved in m_fileType
    // Currently, the only options for this will be "maps" and "registrations"
    m_fileType = selectedFilter.remove(QRegularExpression(" \\(.*\\)")).toLower();
    setInternalData(templateFileNames);

    return true;
  }


  /**
   * @brief Imports the templates
   *
   * This method copies the template files into the appropriate directory according to the
   * chosen filter from setupExecution's QFileDialog. If the file already exists in the chosen
   * directory, it will not be copied over.
   */
  void ImportTemplateWorkOrder::execute() {
    QDir templateFolder = project()->addTemplateFolder(m_fileType + "/import");
    QStringList templateFileNames = internalData();

    m_list = new TemplateList(templateFolder.dirName(), m_fileType, m_fileType + "/" +templateFolder.dirName() );

    foreach (FileName filename, templateFileNames) {
      QFile::copy(filename.expanded(), templateFolder.path() + "/" + filename.name());
      m_list->append(new Template(templateFolder.path() + "/" + filename.name(), m_fileType, templateFolder.dirName()));
    }

    if (!m_list->isEmpty()) {
     project()->addTemplates(m_list);
     project()->setClean(false);
    }


  }


  /**
   * @brief Deletes the previously imported templates
   *
   * This method deletes the templates from both the directory they were copied to
   * and the ProjectItemModel
   */
  void ImportTemplateWorkOrder::undoExecution() {
    if (m_list && project()->templates().size() > 0) {
      m_list->deleteFromDisk( project() );
      ProjectItem *currentItem =
          project()->directory()->model()->findItemData(QVariant::fromValue(m_list));
      project()->directory()->model()->removeItem(currentItem);
    }
    foreach ( Template *currentTemplate, *m_list) {
      delete currentTemplate;
    }
    delete m_list;
    m_list = NULL;
  }

}
