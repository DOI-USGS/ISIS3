#ifndef ImportMapTemplateWorkOrder_H
#define ImportMapTemplateWorkOrder_H
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
 #include "Template.h"
 #include "TemplateList.h"

 namespace Isis {
   /**
    * @brief Add map templates to a project
    *
    * Asks the user for a map template and copies it into the project.
    *
    * @author 2018-07-05 Summer Stapleton
    * @internal
    *    @history 2018-07-05 Summer Stapleton - Created ImportMapTemplateWorkOrder class
    *
    */
    class ImportMapTemplateWorkOrder : public WorkOrder {
        Q_OBJECT
      public:
        ImportMapTemplateWorkOrder(Project *project);
        ImportMapTemplateWorkOrder(const ImportMapTemplateWorkOrder &other);
        ~ImportMapTemplateWorkOrder();

        virtual ImportMapTemplateWorkOrder *clone() const;

        virtual bool isExecutable(ProjectItem *item);
        bool setupExecution();
        void execute();
        void undoExecution();

      private:
        ImportMapTemplateWorkOrder &operator=(const ImportMapTemplateWorkOrder &rhs);

        TemplateList *m_list;
    };
 }

 #endif
