#ifndef RemoveImagesWorkOrder_H
#define RemoveImagesWorkOrder_H
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
  class ImageList;
  class Project;

  /**
   * Removes selected images from current project.
   * This executes synchronously and is not undoable.
   *
   * @author 2016-07-28 Tracie Sucharski
   *
   * @internal
   *   @history 2017-04-07 Marjorie Hahn - Updated method names and documentation
   *                           to match new work order design.
   *
   *   @history 2017-04-16 J Bonn - Updated to new workorder design #4764.
   *   @history 2017-07-25 Cole Neubauer - Added project()->setClean call #4969
   *   @history 2017-07-31 Cole Neubauer - Fixed images not removing correctly Fixes #4998
   *   @history 2017-08-08 Marjorie Hahn - Modified execute() to check if the currently
   *                           selected item to be deleted is actually an image list, so that
   *                           each image can be removed one by one. Fixes #5074.
   *   @history 2017-08-11 Cole Neubauer - Removed isUndoable and set parent member variable
   *                          Fixes #5064
   *   @history 2017-10-01 Cole Neubauer - Reimplemented remove images count Fixes #4998
   */
  class RemoveImagesWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      RemoveImagesWorkOrder(Project *project);
      RemoveImagesWorkOrder(const RemoveImagesWorkOrder &other);
      ~RemoveImagesWorkOrder();

      virtual RemoveImagesWorkOrder *clone() const;

      virtual bool isExecutable(ImageList *images);

      bool setupExecution();

    protected:
      void execute();

    private:
      RemoveImagesWorkOrder &operator=(const RemoveImagesWorkOrder &rhs);

  };
}
#endif
