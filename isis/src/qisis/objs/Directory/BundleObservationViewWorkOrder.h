#ifndef BundleObservationViewWorkOrder_H
#define BundleObservationViewWorkOrder_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "WorkOrder.h"

#include "FileItem.h"

namespace Isis {
  class Project;
  class BundleObservation;
  /**
   *
   * This is a child of the WorkOrder class which is used for anything that performs an action in a
   * Project.  This WorkOrder adds a BundleObservationView to the project.  This runs synchronously
   * and is not undoable.
   *
   * @author 2017-05-04 Tracie Sucharski
   *
   * @internal
   *   @history 2017-07-24 Cole Neuabuer - Set m_isSavedToHistory to false on construction
   *                           Fixes #4715
   *  @history 2017-07-25 Cole Neubauer - Added project()->setClean call #4969
   */
  class BundleObservationViewWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      BundleObservationViewWorkOrder(Project *project);
      BundleObservationViewWorkOrder(const BundleObservationViewWorkOrder &other);
      ~BundleObservationViewWorkOrder();

      virtual BundleObservationViewWorkOrder *clone() const;

      virtual bool isExecutable(FileItemQsp fileItem);
      virtual bool setupExecution();
      virtual void execute();

    private:
      BundleObservationViewWorkOrder &operator=(const BundleObservationViewWorkOrder &rhs);
  };
}
#endif
