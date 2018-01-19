#ifndef TemplateEditViewWorkOrder_H
#define TemplateEditViewWorkOrder_H
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

namespace Isis {
  class Template;
  class ProjectItem;

  /**
   * @brief This work order allows the user to view and edit a template.
   *
   * This is a child of class WorkOrder which is used for anything that performs
   * an action in a Project.  This work order allows the user to view target body info.
   *
   * @author 2015-12-05 Christopher Combs
   *
   * @internal
   *   @history 2017-12-05 Christopher Combs - Original version. Based on TargetGetInfoWorkOrder.
   */
  class TemplateEditViewWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      TemplateEditViewWorkOrder(Project *project);
      TemplateEditViewWorkOrder(const TemplateEditViewWorkOrder &other);
      ~TemplateEditViewWorkOrder();

      virtual TemplateEditViewWorkOrder *clone() const;

      virtual bool isExecutable(ProjectItem *projectitem);
      virtual bool setupExecution();
      virtual void execute();

    protected:
      bool dependsOn(WorkOrder *other) const;

    private:
      TemplateEditViewWorkOrder &operator=(const TemplateEditViewWorkOrder &rhs);
  };
}
#endif
