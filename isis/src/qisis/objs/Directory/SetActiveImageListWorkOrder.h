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
   *  an action in a Project.
   *
   *  This work order allows the user to set the active ImageList in the
   *  project.  Views that need to operate on a common ImageList, ie. footprint2dview,
   *  ControlPointEditView, etc. can get the active ImageList from project.
   *
   * @author 2016-06-27 Tracie Sucharski
   *
   * @internal
   *   @history 2017-04-05 Ian Humphrey - Added isUndoable() implementation to indicate that this
   *                           work order is not undoable. Separated setup and execution into
   *                           setupExecution() and execute(). Fixes #4734.
   *   @history 2017-04-11 Ian Humphrey - Removed isUndoable() and instead set inherited
   *                           m_isUndoable to false in constructor to indicate it is not undoable.
   *                           References #4734.
   *   @history 2017-08-03 Cole Neubauer - Created a try catch around a previously unprotected
   *                           error to handle errors thrown in the workorder that halted
   *                           execution. Fixes #5026
   *   @history 2017-10-18 Adam Paquette - Added a logical check in the isExecutable function
   *                           to check for single images vs image lists. Fixes #5138.
   */

  class SetActiveImageListWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      SetActiveImageListWorkOrder(Project *project);
      SetActiveImageListWorkOrder(const SetActiveImageListWorkOrder &other);
      ~SetActiveImageListWorkOrder();

      virtual SetActiveImageListWorkOrder *clone() const;

      virtual bool isExecutable(ImageList *imageList);

      virtual bool setupExecution();
      virtual void execute();

    private:
      SetActiveImageListWorkOrder &operator=(const SetActiveImageListWorkOrder &rhs);
  };
}
#endif
