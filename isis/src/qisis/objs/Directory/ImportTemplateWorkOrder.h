#ifndef ImportTemplateWorkOrder_H
#define ImportTemplateWorkOrder_H
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
 #include "WorkOrder.h"
 #include "ProjectItem.h"

 namespace Isis {
   /**
    * @brief Add templates to a project
    *
    * Asks the user for a template and its type and copies it into the project.
    *
    * @author 2017-07-31 Christopher Combs
    *
    * @internal
    */
    class ImportTemplateWorkOrder : public WorkOrder {
        Q_OBJECT
      public:
        ImportTemplateWorkOrder(Project *project);
        ImportTemplateWorkOrder(const ImportTemplateWorkOrder &other);
        ~ImportTemplateWorkOrder();

        virtual ImportTemplateWorkOrder *clone() const;

        virtual bool isExecutable(ProjectItem *item);
        bool setupExecution();
        void execute();
        void undoExecution();

      private:
        ImportTemplateWorkOrder &operator=(const ImportTemplateWorkOrder &rhs);

        QList<FileName> m_newFileList;
        QString m_lastChosenFileType; //!< The file type filter chosen in the QFileDialog
    };
 }

 #endif
