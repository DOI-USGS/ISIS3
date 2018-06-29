#ifndef ExportControlNetWorkOrder_H
#define ExportControlNetWorkOrder_H

#include "WorkOrder.h"


namespace Isis {
  /**
   * @brief Write a project control network to a user-specified location
   *
   * Takes a control and writes it's controlNet to disk at a user-specified location. This works
   *   both with and without context (context menus and file menu).
   *
   *  internalData() stores:
   *    Control ID [OPTIONAL] - need context if this isn't present (see controlList())
   *    Output File Name [REQUIRED]
   *
   *
   * @author 2012-09-26 Tracie Sucharski
   *
   * @internal
   *   @history 2017-04-11 Ian Humphrey - Updated to match work order redesign. Replaced asyncRedo
   *                           and postSyncRedo with execute and postExecution. Separated
   *                           setup and action into setupExecution and execute. Fixes #4763.
   *   @history 2017-05-01 Tyler Wilson - Modified the setupExecution() function so that
   *                           it no longer causes a segmentation fault when the user attempts
   *                           to export a control network from the file menu.  Fixes #4760.
   *   @history 2017-11-02 Tyler Wilson - Added a null pointer check on the controls variable in
   *                           isExecutable to prevent potential seg faults.  References #4760.
   *   @history 2018-03-13 Tracie Sucharski - Added Undo text to prevent runtime warning. Also
   *                           correct redmine ticket number in previous history entry.
   *   @history 2018-03-30 Tracie Sucharski - Copy the control net instead of writing.  This will
   *                           be faster and will prevent another control net from being read into
   *                           memory.
   */
  class ExportControlNetWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      ExportControlNetWorkOrder(Project *project);
      ExportControlNetWorkOrder(const ExportControlNetWorkOrder &other);
      ~ExportControlNetWorkOrder();

      virtual ExportControlNetWorkOrder *clone() const;

      bool isExecutable(ControlList *controls);

      virtual bool setupExecution();
      virtual void execute();

    protected:
      virtual void postExecution();

    private:
      ExportControlNetWorkOrder &operator=(const ExportControlNetWorkOrder &rhs);

      QString m_warning; //!< Stores any errors that may have occurred during export.
  };
}

#endif // ExportControlNetWorkOrder_H
