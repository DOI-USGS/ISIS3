#ifndef BundleObservationViewWorkOrder_H
#define BundleObservationViewWorkOrder_H
/**
 * @file
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
