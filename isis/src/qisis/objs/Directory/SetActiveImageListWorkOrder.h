#ifndef SetActiveImageListWorkOrder_H
#define SetActiveImageListWorkOrder_H
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

namespace Isis {
  class ImageList;

  /**
   * @brief  This is a child of class WorkOrder which is used for anything that performs
   *  an action in a Project.  This work order allows the user to set the active ImageList in the
   *  project.  Views that need to operate on a common ImageList, ie. footprint2dview,
   *  ControlPointEditView, etc. can get the active ImageList from project.
   *
   * @author 2016-06-27 Tracie Sucharski
   *
   * @internal
   *
   */

  class SetActiveImageListWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      SetActiveImageListWorkOrder(Project *project);
      SetActiveImageListWorkOrder(const SetActiveImageListWorkOrder &other);
      ~SetActiveImageListWorkOrder();

      virtual SetActiveImageListWorkOrder *clone() const;

      virtual bool isExecutable(ImageList *imageList);
      bool execute();

    private:
      SetActiveImageListWorkOrder &operator=(const SetActiveImageListWorkOrder &rhs);
  };
}
#endif

