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
   */
  class ExportControlNetWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      ExportControlNetWorkOrder(Project *project);
      ExportControlNetWorkOrder(const ExportControlNetWorkOrder &other);
      ~ExportControlNetWorkOrder();

      virtual ExportControlNetWorkOrder *clone() const;

      bool isExecutable(QList<Control *> controls);


      bool execute();

      void asyncRedo();
      void postSyncRedo();

    private:
      ExportControlNetWorkOrder &operator=(const ExportControlNetWorkOrder &rhs);

      QString m_warning;
  };
}

#endif // ExportControlNetWorkOrder_H
